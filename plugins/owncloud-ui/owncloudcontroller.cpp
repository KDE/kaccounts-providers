/*
 *  SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "owncloudcontroller.h"

#include <KIO/Job>
#include <KIO/DavJob>
#include <kio/global.h>
#include <KLocalizedString>

#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

#include "../cloudurls.h"

OwncloudController::OwncloudController(QObject *parent)
    : QObject(parent),
      m_errorMessage(QString()),
      m_isWorking(false)
{
}

OwncloudController::~OwncloudController()
{
}

void OwncloudController::checkServer(const QString &username, const QString &password, const QString &path)
{
    m_errorMessage.clear();
    Q_EMIT errorMessageChanged();

    m_username = username;
    m_password = password;

    checkServer(createStatusUrl(path));
}

void OwncloudController::checkServer(const QUrl &url)
{
    qDebug() << "Checking for ownCloud instance at" << url;
    setWorking(true);
    KIO::TransferJob *job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    job->setUiDelegate(0);
    connect(job, SIGNAL(data(KIO::Job*,QByteArray)), SLOT(dataReceived(KIO::Job*,QByteArray)));
    connect(job, SIGNAL(finished(KJob*)), this, SLOT(fileChecked(KJob*)));
}

void OwncloudController::figureOutServer(const QUrl& url)
{
    if (/*url == QLatin1String("/") ||*/ url.isEmpty()) {
        serverCheckResult(false);
        return;
    }

    m_json.clear();

    qDebug() << "Received url to figure out:" << url;
    // This needs 2x up cause first it just removes the status.php
    // and only the second call actually moves up
    QUrl urlUp = KIO::upUrl(KIO::upUrl(url));
    urlUp.setPath(urlUp.path() + '/' + "status.php");

    if (urlUp != url) {
        checkServer(urlUp.adjusted(QUrl::NormalizePathSegments));
    } else {
        serverCheckResult(false);
    }
}

void OwncloudController::dataReceived(KIO::Job *job, const QByteArray &data)
{
    Q_UNUSED(job);
    m_json.append(data);
}

void OwncloudController::fileChecked(KJob* job)
{
    KIO::TransferJob *kJob = qobject_cast<KIO::TransferJob *>(job);
    if (kJob->error()) {
        qDebug() << job->errorString();
        qDebug() << job->errorText();
        figureOutServer(kJob->url());
        return;
    }

    QJsonDocument parser = QJsonDocument::fromJson(m_json);
    QJsonObject map = parser.object();
    if (!map.contains("version")) {
        figureOutServer(kJob->url());
        qDebug() << "No json";
        return;
    }

    m_server = kJob->url().adjusted(QUrl::RemoveFilename).toString();
    qDebug() << "ownCloud appears to be running at the specified URL";
    serverCheckResult(true);
}

void OwncloudController::setWorking(bool start)
{
    if (start == m_isWorking) {
        return;
    }

    m_isWorking = start;
    Q_EMIT isWorkingChanged();
}

void OwncloudController::serverCheckResult(bool result)
{
    if (!result) {
        m_errorMessage = i18n("Unable to connect to ownCloud at the given server URL. Please check the server URL.");
        setWorking(false);
    } else {
        m_errorMessage.clear();

        qDebug() << "Server URL ok, checking auth...";

        m_json.clear();

        QUrl url(m_server);

        url.setUserName(m_username);
        url.setPassword(m_password);

        if (!url.path().endsWith(QLatin1String("/"))) {
            url.setPath(url.path() + '/');
        }

        url.setPath(url.path() + "remote.php/webdav");

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
    }

    Q_EMIT errorMessageChanged();

}

void OwncloudController::authCheckResult(KJob *job)
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
    }

    Q_EMIT errorMessageChanged();

    if (!kJob->isErrorPage()) {
        m_state = Services;
        Q_EMIT stateChanged();
    }

    setWorking(false);
}

bool OwncloudController::isWorking()
{
    return m_isWorking;
}

QString OwncloudController::errorMessage() const
{
    return m_errorMessage;
}

void OwncloudController::finish(const QStringList &disabledServices)
{
    QVariantMap data;
    data.insert("server", m_server);

    for (const QString &service : disabledServices) {
        data.insert("__service/" + service, false);
    }

    QUrl carddavUrl(m_server);
    carddavUrl.setPath(carddavUrl.path() + QString("/remote.php/carddav/addressbooks/%1").arg(m_username));

    data.insert("carddavUrl", carddavUrl);
    data.insert("dav/host", carddavUrl.host());
    data.insert("dav/storagePath", QStringLiteral("/remote.php/webdav"));

    Q_EMIT wizardFinished(m_username, m_password, data);
}

void OwncloudController::cancel()
{
    Q_EMIT wizardCancelled();
}

QVariantList OwncloudController::availableServices() const
{
    // TODO Find a way to not hardcode this
    return {
        QVariant::fromValue(Service{QStringLiteral("owncloud-storage"), i18n("Storage"), i18n("Storage integration")})
    };
}
