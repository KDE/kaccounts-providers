/*
 *  SPDX-FileCopyrightText: 2019 Rituka Patwal <ritukapatwal21@gmail.com>
 *  SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "nextcloudcontroller.h"

#include <KIO/DavJob>
#include <KIO/Job>
#include <KLocalizedString>
#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrlQuery>
#include <kio/global.h>

#include "../cloudurls.h"

// Document for login flow :  https://docs.nextcloud.com/server/stable/developer_manual/client_apis/LoginFlow/index.html

void NextcloudUrlIntercepter::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    info.setHttpHeader("OCS-APIREQUEST", "true");
}

NextcloudController::NextcloudController(QObject *parent)
    : QObject(parent)
    , m_webengineProfile(new QQuickWebEngineProfile(this))
{
    m_webengineProfile->setUrlRequestInterceptor(&m_urlIntercepter);
    m_webengineProfile->setHttpUserAgent(QStringLiteral("KAccounts Nextcloud Login"));

    QDesktopServices::setUrlHandler(QStringLiteral("nc"), this, "finalUrlHandler");
}

NextcloudController::~NextcloudController()
{
}

void NextcloudController::checkServer(const QString &path)
{
    m_errorMessage.clear();
    Q_EMIT errorMessageChanged();

    m_json.clear();

    checkServer(createStatusUrl(path));
}

// To check if url is correct
void NextcloudController::checkServer(const QUrl &url)
{
    setWorking(true);
    KIO::TransferJob *job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    job->setUiDelegate(nullptr);
    connect(job, &KIO::DavJob::data, this, &NextcloudController::dataReceived);
    connect(job, &KIO::DavJob::finished, this, &NextcloudController::fileChecked);
}

void NextcloudController::dataReceived(KIO::Job *job, const QByteArray &data)
{
    Q_UNUSED(job);
    m_json.append(data);
}

void NextcloudController::fileChecked(KJob *job)
{
    KIO::TransferJob *kJob = qobject_cast<KIO::TransferJob *>(job);
    if (kJob->error()) {
        wrongUrlDetected();
        return;
    }

    QJsonDocument parser = QJsonDocument::fromJson(m_json);
    QJsonObject map = parser.object();
    if (!map.contains(QStringLiteral("version"))) {
        wrongUrlDetected();
        return;
    }

    QUrl url = KIO::upUrl(kJob->url());
    m_server = url.toString();

    m_loginUrl = m_server + QStringLiteral("/index.php/login/flow");
    Q_EMIT loginUrlChanged();

    m_state = WebLogin;
    Q_EMIT stateChanged();
}

// When url entered by user is wrong
void NextcloudController::wrongUrlDetected()
{
    m_errorMessage = i18n("Unable to connect to Nextcloud at the given server URL. Please check the server URL.");
    setWorking(false);
    Q_EMIT errorMessageChanged();
}

// Open Webview for nextcloud login.

void NextcloudController::finalUrlHandler(const QUrl &url)
{
    // url is of the form: nc://login/server:<server>&user:<loginname>&password:<password>

    QUrlQuery urlQuery;
    urlQuery.setQueryDelimiters(QLatin1Char(':'), QLatin1Char('&'));
    urlQuery.setQuery(url.path(QUrl::FullyEncoded).mid(1));

    m_username = urlQuery.queryItemValue(QStringLiteral("user"), QUrl::FullyDecoded);
    m_password = urlQuery.queryItemValue(QStringLiteral("password"), QUrl::FullyDecoded);

    serverCheckResult();
}

void NextcloudController::setWorking(bool start)
{
    if (start == m_isWorking) {
        return;
    }

    m_isWorking = start;
    Q_EMIT isWorkingChanged();
}

void NextcloudController::serverCheckResult()
{
    m_errorMessage.clear();
    m_json.clear();

    QUrl url(m_server);
    url.setUserName(m_username);
    url.setPassword(m_password);
    url = url.adjusted(QUrl::StripTrailingSlash);
    url.setPath(url.path() + QLatin1Char('/') + QLatin1String("remote.php/webdav"));
    // Send a basic PROPFIND command to test access
    const QString requestStr = QStringLiteral(
        "<d:propfind xmlns:d=\"DAV:\">"
        "<d:prop>"
        "<d:current-user-principal />"
        "</d:prop>"
        "</d:propfind>");

    KIO::DavJob *job = KIO::davPropFind(url, QDomDocument(requestStr), QStringLiteral("1"), KIO::HideProgressInfo);
    connect(job, &KIO::DavJob::finished, this, &NextcloudController::authCheckResult);
    connect(job, &KIO::DavJob::data, this, &NextcloudController::dataReceived);

    QVariantMap metadata{
        {QStringLiteral("cookies"), QStringLiteral("none")},
        {QStringLiteral("no-cache"), true},
    };

    job->setMetaData(metadata);
    job->setUiDelegate(nullptr);
    job->start();

    Q_EMIT errorMessageChanged();
}

void NextcloudController::authCheckResult(KJob *job)
{
    KIO::DavJob *kJob = qobject_cast<KIO::DavJob *>(job);

    if (kJob->isErrorPage()) {
        m_errorMessage = i18n("Unable to authenticate using the provided username and password");
    } else {
        m_errorMessage.clear();
        m_state = Services;
        Q_EMIT stateChanged();
    }

    Q_EMIT errorMessageChanged();

    setWorking(false);
}

bool NextcloudController::isWorking()
{
    return m_isWorking;
}

QString NextcloudController::errorMessage() const
{
    return m_errorMessage;
}

void NextcloudController::cancel()
{
    Q_EMIT wizardCancelled();
}

void NextcloudController::finish(const QStringList disabledServices)
{
    QVariantMap data;
    data.insert(QStringLiteral("server"), m_server);

    QUrl serverUrl(m_server);

    QUrl carddavUrl(serverUrl.adjusted(QUrl::StripTrailingSlash));
    carddavUrl.setPath(carddavUrl.path() + QStringLiteral("/remote.php/carddav/addressbooks/%1").arg(m_username));

    QUrl webdavUrl(serverUrl.adjusted(QUrl::StripTrailingSlash));
    webdavUrl.setPath(webdavUrl.path() + QStringLiteral("/remote.php/dav/files/%1").arg(m_username));

    data.insert(QStringLiteral("dav/host"), serverUrl.host());
    data.insert(QStringLiteral("dav/storagePath"), webdavUrl.path());
    data.insert(QStringLiteral("dav/contactsPath"), carddavUrl.path());

    for (const QString &service : disabledServices) {
        data.insert(QStringLiteral("__service/") + service, false);
    }

    Q_EMIT wizardFinished(m_username, m_password, data);
}

QVariantList NextcloudController::availableServices() const
{
    // TODO Find a way to not hardcode this
    return {QVariant::fromValue(Service{QStringLiteral("nextcloud-contacts"), i18n("Contacts"), i18n("Synchronize contacts")}),
            QVariant::fromValue(Service{QStringLiteral("nextcloud-storage"), i18n("Storage"), i18n("Integrate into file manager")})};
}
