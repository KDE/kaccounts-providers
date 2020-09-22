/*
 *  SPDX-FileCopyrightText: 2019 Rituka Patwal <ritukapatwal21@gmail.com>
 *  SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef QMLHELPER_H
#define QMLHELPER_H

#include <QObject>
#include <QStringList>
#include <QtWidgets>
#include <QWebEngineView>
#include <QWebEnginePage> 
#include <QWebEngineHttpRequest> 
#include <QWebEngineUrlRequestInterceptor>
#include <QQuickWebEngineProfile>

namespace KIO
{
    class Job;
};

class KJob;

class NextcloudUrlIntercepter : public QWebEngineUrlRequestInterceptor
{
    void interceptRequest(QWebEngineUrlRequestInfo &info) override;
};

class NextcloudController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isWorking READ isWorking NOTIFY isWorkingChanged)
    Q_PROPERTY(bool isLoginComplete READ isLoginComplete NOTIFY isLoginCompleteChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
    Q_PROPERTY(State state MEMBER m_state NOTIFY stateChanged)
    Q_PROPERTY(QQuickWebEngineProfile *webengineProfile MEMBER m_webengineProfile CONSTANT)
    Q_PROPERTY(QString loginUrl MEMBER m_loginUrl NOTIFY loginUrlChanged)

public:

    enum State {
        ServerUrl = 0,
        WebLogin,
        Services,
    };
    Q_ENUM(State)

    NextcloudController(QObject *parent = 0);
    ~NextcloudController();

    Q_INVOKABLE void checkServer(const QString &server);
    Q_INVOKABLE void finish(bool contactsEnabled);
    bool isWorking();
    bool isLoginComplete();
    QString errorMessage() const;

Q_SIGNALS:
    void isWorkingChanged();
    void errorMessageChanged();
    void isLoginCompleteChanged();
    void wizardFinished(const QString &username, const QString &password, const QVariantMap &data);
    void stateChanged();
    void loginUrlChanged();

private Q_SLOTS:
    void fileChecked(KJob *job);
    void dataReceived(KIO::Job *job, const QByteArray &data);
    void authCheckResult(KJob *job);
    void finalUrlHandler(const QUrl &url);

private:
    void checkServer(const QUrl &url);
    void figureOutServer(const QUrl &url);
    void setWorking(bool start);
    void serverCheckResult();
    void openWebView();
    void wrongUrlDetected();

    QByteArray m_json;
    QString m_errorMessage;
    QString m_server;
    QString m_username;
    QString m_password;
    QUrl m_finalUrl;
    QWebEngineView * m_view = new QWebEngineView;
    QStringList m_disabledServices;
    bool m_isWorking = false;
    bool m_isLoginComplete = false;
    State m_state = ServerUrl;
    QQuickWebEngineProfile *m_webengineProfile;
    NextcloudUrlIntercepter m_urlIntercepter;
    QString m_loginUrl;

};

#endif // QMLHELPER_H
