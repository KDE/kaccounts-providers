/*************************************************************************************
 *  Copyright (C) 2019 by Rituka Patwal <ritukapatwal21@gmail.com>                   * 
 *  Copyright (C) 2015 by Martin Klapetek <mklapetek@kde.org>                        *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or                    *
 *  modify it under the terms of the GNU General Public License                      *
 *  as published by the Free Software Foundation; either version 2                   *
 *  of the License, or (at your option) any later version.                           *
 *                                                                                   *
 *  This program is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 *  GNU General Public License for more details.                                     *
 *                                                                                   *
 *  You should have received a copy of the GNU General Public License                *
 *  along with this program; if not, write to the Free Software                      *
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA   *
 *************************************************************************************/

#include "qmlhelper.h"

#include <KIO/Job>
#include <KIO/DavJob>
#include <kio/global.h>
#include <KLocalizedString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtWidgets>
#include <QtNetwork>
#include <QWebEngineView>
#include <QWebEnginePage> 
#include <QWebEngineHttpRequest> 
#include <QWebEngineProfile>
#include <QFileSystemWatcher> 
#include <QDesktopServices>

QmlHelper::QmlHelper(QObject *parent)
    : QObject(parent)
{
}

QmlHelper::~QmlHelper()
{
}

void QmlHelper::checkServer(const QString &path)
{   
    m_errorMessage.clear();
    Q_EMIT errorMessageChanged();

    m_json.clear();
    QString urlString = path;

    //To remove "/'s" from the end
    while(urlString.endsWith("/")) {
        urlString.remove(urlString.length() - 1, 1);
    }

    QUrl url = QUrl::fromUserInput(urlString);
    url.setPath(url.path() + '/' + "status.php");

    if (url.host().isEmpty()) {
        return;
    }

    checkServer(url);
}

//To check if url is correct
void QmlHelper::checkServer(const QUrl &url)
{
    setWorking(true);
    KIO::TransferJob *job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    job->setUiDelegate(0);
    connect(job, &KIO::DavJob::data, this, &QmlHelper::dataReceived);
    connect(job, &KIO::DavJob::finished, this, &QmlHelper::fileChecked);
}

void QmlHelper::dataReceived(KIO::Job *job, const QByteArray &data)
{
    Q_UNUSED(job);
    m_json.append(data);
}

void QmlHelper::fileChecked(KJob* job)
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

    // Call webview for login
    openWebView();
}

// When url entered by user is wrong
void QmlHelper::wrongUrlDetected()
{
    m_noError = false;
    Q_EMIT noErrorChanged();
    m_errorMessage = i18n("Unable to connect to Nextcloud at the given server URL. Please check the server URL.");
    setWorking(false);
    Q_EMIT errorMessageChanged();
}


// Open Webview for nextcloud login. 
// Document for login flow :  https://docs.nextcloud.com/server/stable/developer_manual/client_apis/LoginFlow/index.html
void QmlHelper::openWebView()
{
    QWebEngineHttpRequest request;
    // set proper headers
    request.setUrl(QUrl::fromUserInput(m_server + "/index.php/login/flow"));
    request.setHeader(
            QByteArray::fromStdString("USER_AGENT"), 
            QByteArray::fromStdString("Mozilla/5.0 nextcloud-ui-plugin")
    );
    request.setHeader(
            QByteArray::fromStdString("OCS-APIREQUEST"), 
            QByteArray::fromStdString("true")
    );
        
    m_view->load(request);

    // Delete cookies because it is a one time webview
    m_view->page()->profile()->setPersistentCookiesPolicy(QWebEngineProfile::NoPersistentCookies);
    // To catch the url of scheme *nc* on the final login
    QDesktopServices::setUrlHandler("nc", this, "finalUrlHandler");

    m_view->show();
    m_view->resize(424, 650);
}

void QmlHelper::finalUrlHandler(const QUrl &url){
    m_finalUrl = url;
    m_view->close();
    delete m_view;

    // To fetch m_username and m_password from final url
    QString finalURLtoString = m_finalUrl.toString();
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

void QmlHelper::setWorking(bool start)
{
    if (start == m_isWorking) {
        return;
    }

    m_isWorking = start;
    Q_EMIT isWorkingChanged();
}

void QmlHelper::serverCheckResult()
{
    m_noError = true;
    Q_EMIT noErrorChanged();
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
    connect(job, &KIO::DavJob::finished, this, &QmlHelper::authCheckResult);
    connect(job, &KIO::DavJob::data, this, &QmlHelper::dataReceived);

    QVariantMap metadata{{"cookies","none"}, {"no-cache",true}};

    job->setMetaData(metadata);
    job->setUiDelegate(0);
    job->start();

    Q_EMIT errorMessageChanged();
}

void QmlHelper::authCheckResult(KJob *job)
{
    KIO::DavJob *kJob = qobject_cast<KIO::DavJob*>(job);

    if (kJob->isErrorPage()) {
        m_errorMessage = i18n("Unable to authenticate using the provided username and password");
    } else {
        m_errorMessage.clear();
        m_isLoginComplete = true;
        Q_EMIT isLoginCompleteChanged();
    }

    Q_EMIT errorMessageChanged();

    m_noError = !kJob->isErrorPage();
    Q_EMIT noErrorChanged();
    setWorking(false);
}

bool QmlHelper::isWorking()
{
    return m_isWorking;
}

bool QmlHelper::noError()
{
    return m_noError;
}

QString QmlHelper::errorMessage() const
{
    return m_errorMessage;
}

bool QmlHelper::isLoginComplete()
{
    return m_isLoginComplete;
}

void QmlHelper::finish(bool contactsEnabled)
{
    QVariantMap data;
    data.insert("server", m_server);

    QUrl serverUrl(m_server);

    data.insert("dav/host", serverUrl.host());
    data.insert("dav/storagePath", QStringLiteral("/remote.php/dav/files/%1").arg(m_username));
    data.insert("dav/contactsPath", QStringLiteral("/remote.php/dav/addressbooks/users/%1").arg(m_username));

    if (!contactsEnabled) {
        data.insert("__service/nextcloud-contacts", false);
    }

    Q_EMIT wizardFinished(m_username, m_password, data);
}
