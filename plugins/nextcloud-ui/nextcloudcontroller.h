/*
 *  SPDX-FileCopyrightText: 2019 Rituka Patwal <ritukapatwal21@gmail.com>
 *  SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef QMLHELPER_H
#define QMLHELPER_H

#include <QNetworkAccessManager>
#include <QObject>
#include <QQuickWebEngineProfile>
#include <QStringList>
#include <QVariant>
#include <QWebEngineUrlRequestInterceptor>

#include <QCoroCore>

namespace KIO
{
class Job;
}

class KJob;

class Service
{
    Q_GADGET
    Q_PROPERTY(QString id MEMBER m_id)
    Q_PROPERTY(QString name MEMBER m_name)
    Q_PROPERTY(QString description MEMBER m_description)

public:
    QString m_id;
    QString m_name;
    QString m_description;
};

class NextcloudUrlIntercepter : public QWebEngineUrlRequestInterceptor
{
    void interceptRequest(QWebEngineUrlRequestInfo &info) override;
};

class NextcloudController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isWorking READ isWorking NOTIFY isWorkingChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
    Q_PROPERTY(State state MEMBER m_state NOTIFY stateChanged)
    Q_PROPERTY(QQuickWebEngineProfile *webengineProfile MEMBER m_webengineProfile CONSTANT)
    Q_PROPERTY(QString loginUrl MEMBER m_loginUrl NOTIFY loginUrlChanged)
    Q_PROPERTY(QVariantList availableServices READ availableServices CONSTANT)

public:
    enum State {
        ServerUrl = 0,
        WebLogin,
        Services,
    };
    Q_ENUM(State)

    NextcloudController(QObject *parent = nullptr);
    ~NextcloudController();

    Q_INVOKABLE void checkServer(const QString &server);
    Q_INVOKABLE void finish(const QStringList disabledServices);
    Q_INVOKABLE void cancel();
    bool isWorking();
    bool isLoginComplete();
    QString errorMessage() const;
    QVariantList availableServices() const;

Q_SIGNALS:
    void isWorkingChanged();
    void errorMessageChanged();
    void wizardFinished(const QString &username, const QString &password, const QVariantMap &data);
    void stateChanged();
    void loginUrlChanged();
    void wizardCancelled();

private Q_SLOTS:
    void finalUrlHandler(const QUrl &url);

private:
    QCoro::Task<void> checkServer(const QUrl &url);
    void figureOutServer(const QUrl &url);
    void setWorking(bool start);
    QCoro::Task<void> serverCheckResult();
    void wrongUrlDetected();

    QString m_errorMessage;
    QString m_server;
    QString m_username;
    QString m_password;
    QStringList m_disabledServices;
    bool m_isWorking = false;
    State m_state = ServerUrl;
    QQuickWebEngineProfile *m_webengineProfile;
    NextcloudUrlIntercepter m_urlIntercepter;
    QString m_loginUrl;
    QNetworkAccessManager m_nam;
};

#endif // QMLHELPER_H
