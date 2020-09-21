/*
 *  SPDX-FileCopyrightText: 2019 Rituka Patwal <ritukapatwal21@gmail.com>
 *  SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "nextcloudcontroller.h"

#include <KIO/Job>
#include <KIO/DavJob>
#include <kio/global.h>
#include <KLocalizedString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDesktopServices>

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

    QDesktopServices::setUrlHandler("nc", this, "finalUrlHandler");
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

//To check if url is correct
void NextcloudController::checkServer(const QUrl &url)
{
    setWorking(true);
    KIO::TransferJob *job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    job->setUiDelegate(0);
    connect(job, &KIO::DavJob::data, this, &NextcloudController::dataReceived);
    connect(job, &KIO::DavJob::finished, this, &NextcloudController::fileChecked);
}

void NextcloudController::dataReceived(KIO::Job *job, const QByteArray &data)
{
    Q_UNUSED(job);
    m_json.append(data);
}

void NextcloudController::fileChecked(KJob* job)
{
    KIO::TransferJob *kJob = qobject_cast<KIO::TransferJob *>(job);
    if (kJob->error()) {
        wrongUrlDetected();
        return;
    }

    QJsonDocument parser = QJsonDocument::fromJson(m_json);
    QJsonObject map = parser.object();
    if (!map.contains("version")) {
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

void NextcloudController::finalUrlHandler(const QUrl &url){
    // To fetch m_username and m_password from final url
    QString finalURLtoString = url.toString();
    int username_ini_pos = finalURLtoString.indexOf("&user:") + 6;
    int password_ini_pos = finalURLtoString.indexOf("&password:") + 10;
    int username_size = password_ini_pos - username_ini_pos - 10;
    QString username = finalURLtoString.mid(username_ini_pos, username_size);
    QString password = finalURLtoString.mid(password_ini_pos);
    // To replace %40 with @
    int position = username.indexOf("%40");
    username.replace(position, 3, "@");

    m_username = username;
    m_password = password;  

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
    url.setPath(url.path() + '/' + "remote.php/webdav");
    // Send a basic PROPFIND command to test access
    const QString requestStr = QStringLiteral(
            "<d:propfind xmlns:d=\"DAV:\">"
            "<d:prop>"
            "<d:current-user-principal />"
            "</d:prop>"
            "</d:propfind>");

    KIO::DavJob *job = KIO::davPropFind(url, QDomDocument(requestStr), "1", KIO::HideProgressInfo);
    connect(job, &KIO::DavJob::finished, this, &NextcloudController::authCheckResult);
    connect(job, &KIO::DavJob::data, this, &NextcloudController::dataReceived);

    QVariantMap metadata{{"cookies","none"}, {"no-cache",true}};

    job->setMetaData(metadata);
    job->setUiDelegate(0);
    job->start();

    Q_EMIT errorMessageChanged();
}

void NextcloudController::authCheckResult(KJob *job)
{
    KIO::DavJob *kJob = qobject_cast<KIO::DavJob*>(job);

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

void NextcloudController::finish(const QStringList disabledServices)
{
    QVariantMap data;
    data.insert("server", m_server);

    QUrl serverUrl(m_server);

    data.insert("dav/host", serverUrl.host());
    data.insert("dav/storagePath", QStringLiteral("/remote.php/dav/files/%1").arg(m_username));
    data.insert("dav/contactsPath", QStringLiteral("/remote.php/dav/addressbooks/users/%1").arg(m_username));

    for (const QString &service : disabledServices) {
        data.insert("__service/" + service, false);
    }

    Q_EMIT wizardFinished(m_username, m_password, data);
}

QVariantList NextcloudController::availableServices() const
{
    // TODO Find a way to not hardcode this
    return {
        QVariant::fromValue(Service{QStringLiteral("nextcloud-contacts"), i18n("Contacts"), i18n("Synchronize contacts")}),
        QVariant::fromValue(Service{QStringLiteral("nextcloud-storage"), i18n("Storage"), i18n("Integrate into file manager")})
    };
}
