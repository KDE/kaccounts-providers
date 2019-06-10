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
#include <QDebug>
#include <QtWidgets>
#include <QtNetwork>
#include <QWebEngineView>
#include <QWebEnginePage> 
#include <QWebEngineHttpRequest> 
#include <QWebEngineProfile>
#include <QFileSystemWatcher> 
#include <QDesktopServices>

QmlHelper::QmlHelper(QObject *parent)
    : QObject(parent),
      m_isWorking(false),
      m_noError(false),
      m_isLoginComplete(false),
      m_errorMessage(QString()){}

QmlHelper::~QmlHelper(){}

void QmlHelper::checkServer(const QString &path)
{   
    qDebug()<<"                        "<<m_isLoginComplete<<"     "<<m_isWorking;
    m_errorMessage.clear();
    Q_EMIT errorMessageChanged();

    m_json.clear();
    QString urlString = path;

    //To remove "/'s" from the end
    while(urlString.endsWith("/")){
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
    qDebug() << "Checking for nextCloud instance at" << url;
    setWorking(true);
    KIO::TransferJob *job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    job->setUiDelegate(0);
    connect(job, SIGNAL(data(KIO::Job*,QByteArray)), SLOT(dataReceived(KIO::Job*,QByteArray)));
    connect(job, SIGNAL(finished(KJob*)), this, SLOT(fileChecked(KJob*)));
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
        qDebug() << job->errorString();
        qDebug() << job->errorText();
        wrongUrlDetected();
        return;
    }

    QJsonDocument parser = QJsonDocument::fromJson(m_json);
    QJsonObject map = parser.object();
    if (!map.contains("version")) {
        wrongUrlDetected();
        qDebug() << "No json";
        return;
    }
    
    QUrl url = KIO::upUrl(kJob->url());
    m_server = url.toString();

    //call webview for login
    openWebView();
}

//when url entered by user is wrong
void QmlHelper::wrongUrlDetected(){
    m_noError = false;
    Q_EMIT noErrorChanged();

    m_errorMessage = i18n("Unable to connect to nextCloud at the given server URL. Please check the server URL.");
    setWorking(false);

    Q_EMIT errorMessageChanged();
}


//Opens Webview for nextcloud login
void QmlHelper::openWebView(){

    QWebEngineHttpRequest request;
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

    //delete cookies
    m_view->page()->profile()->setPersistentCookiesPolicy(QWebEngineProfile::NoPersistentCookies);
    QDesktopServices::setUrlHandler("nc", this, "finalUrlHandler");

    m_view->show();
    m_view->resize(424, 650);
}

void QmlHelper::finalUrlHandler(const QUrl &url){
    m_final_url = url;
    qDebug()<<"Final URL: "<<m_final_url;
    m_view->close();
    delete m_view;

    //code to fetch m_username and m_password from final url
    QString finalURLtoString = m_final_url.toString();
    int username_ini_pos = finalURLtoString.indexOf("&user:") + 6;
    int password_ini_pos = finalURLtoString.indexOf("&password:") + 10;
    int username_size = password_ini_pos - username_ini_pos - 10;
    QString username = finalURLtoString.mid(username_ini_pos, username_size);
    QString password = finalURLtoString.mid(password_ini_pos);

        //To replace %40 with @
        int position = username.indexOf("%40");
        username.replace(position, 3, "@");

    qDebug()<<"username: "<<username<<" password:  "<<password;

    m_username = username;
    m_password = password;
    //end

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

    qDebug() << "Server URL ok, checking auth...";

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
    connect(job, SIGNAL(finished(KJob*)), this, SLOT(authCheckResult(KJob*)));
    connect(job, SIGNAL(data(KIO::Job*,QByteArray)), SLOT(dataReceived(KIO::Job*,QByteArray)));

    QVariantMap metadata{{"cookies","none"}, {"no-cache",true}};

    job->setMetaData(metadata);
    job->setUiDelegate(0);
    job->start();

    Q_EMIT errorMessageChanged();
}



void QmlHelper::authCheckResult(KJob *job)
{
    if (job->error()) {
        qDebug() << job->errorString();
        qDebug() << job->errorText();
    }

    KIO::DavJob *kJob = qobject_cast<KIO::DavJob*>(job);
    qDebug() << "Auth job finished, received error page:" << kJob->isErrorPage();

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

    qDebug()<<contactsEnabled;
    if (!contactsEnabled) {
        data.insert("__service/nextcloud-contacts", false);
    }
    qDebug()<<"F   I   N   A   L     D   A   T   A :   \n"<<data<<"\n\n";

    QUrl carddavUrl(m_server);
    carddavUrl.setPath(carddavUrl.path() + QString("/remote.php/carddav/addressbooks/%1").arg(m_username));

    data.insert("carddavUrl", carddavUrl);

    Q_EMIT wizardFinished(m_username, m_password, data);
}

#include "qmlhelper.moc"
