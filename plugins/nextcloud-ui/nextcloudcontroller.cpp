/*
 *  SPDX-FileCopyrightText: 2019 Rituka Patwal <ritukapatwal21@gmail.com>
 *  SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "nextcloudcontroller.h"

#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrlQuery>

#include <KIO/Global>
#include <KLocalizedString>

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

    checkServer(createStatusUrl(path));
}

// To check if url is correct
QCoro::Task<void> NextcloudController::checkServer(const QUrl &url)
{
    setWorking(true);

    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::AutoDeleteReplyOnFinishAttribute, true);

    QNetworkReply *reply = m_nam.get(request);

    co_await qCoro(reply, &QNetworkReply::finished);

    if (reply->error() || reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() != 200) {
        wrongUrlDetected();
        co_return;
    }

    QJsonDocument parser = QJsonDocument::fromJson(reply->readAll());
    QJsonObject map = parser.object();
    if (!map.contains(QStringLiteral("version"))) {
        wrongUrlDetected();
        co_return;
    }

    m_server = KIO::upUrl(request.url()).toString();

    m_loginUrl = m_server + QStringLiteral("/index.php/login/flow");
    Q_EMIT loginUrlChanged();

    m_state = WebLogin;
    Q_EMIT stateChanged();

    co_return;
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

QCoro::Task<void> NextcloudController::serverCheckResult()
{
    m_errorMessage.clear();

    QUrl url(m_server);
    url.setUserName(m_username);
    url.setPassword(m_password);
    url = url.adjusted(QUrl::StripTrailingSlash);
    url.setPath(url.path() + QLatin1Char('/') + QLatin1String("remote.php/webdav"));
    // Send a basic PROPFIND command to test access
    const QByteArray requestData =
        "<d:propfind xmlns:d=\"DAV:\">"
        "<d:prop>"
        "<d:current-user-principal />"
        "</d:prop>"
        "</d:propfind>";

    QNetworkRequest request(url);
    request.setRawHeader("Depth", "1");
    request.setAttribute(QNetworkRequest::AutoDeleteReplyOnFinishAttribute, true);

    auto reply = m_nam.sendCustomRequest(request, "PROPFIND", requestData);

    Q_EMIT errorMessageChanged();

    co_await qCoro(reply, &QNetworkReply::finished);

    if (reply->error() != QNetworkReply::NoError) {
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
