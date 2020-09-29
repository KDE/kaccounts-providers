/*
 *  SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef QMLHELPER_H
#define QMLHELPER_H

#include <QObject>
#include <QStringList>

namespace KIO
{
    class Job;
};
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


class OwncloudController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(State state MEMBER m_state NOTIFY stateChanged)
    Q_PROPERTY(bool isWorking READ isWorking NOTIFY isWorkingChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
    Q_PROPERTY(QVariantList availableServices READ availableServices CONSTANT)

public:

    enum State {
        Server = 0,
        Services
    };
    Q_ENUM(State)

    explicit OwncloudController(QObject *parent = 0);
    ~OwncloudController();

    Q_INVOKABLE void checkServer(const QString &username, const QString &password, const QString &server);
    Q_INVOKABLE void finish(const QStringList &disabledServices);
    bool isWorking();
    QString errorMessage() const;
    State state() const;
    QVariantList availableServices() const;

Q_SIGNALS:
    void isWorkingChanged();
    void errorMessageChanged();
    void wizardFinished(const QString &username, const QString &password, const QVariantMap &data);
    void stateChanged();

private Q_SLOTS:
    void fileChecked(KJob *job);
    void dataReceived(KIO::Job *job, const QByteArray &data);
    void authCheckResult(KJob *job);

private:
    void checkServer(const QUrl &url);
    void figureOutServer(const QUrl &url);
    void setWorking(bool start);
    void serverCheckResult(bool result);

    QByteArray m_json;
    QString m_errorMessage;
    QString m_server;
    QString m_username;
    QString m_password;
    QStringList m_disabledServices;
    bool m_isWorking;
    State m_state = Server;

};

#endif // QMLHELPER_H
