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

class QmlHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isWorking READ isWorking NOTIFY isWorkingChanged)
    Q_PROPERTY(bool noError READ noError NOTIFY noErrorChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

public:
    explicit QmlHelper(QObject *parent = 0);
    ~QmlHelper();

    Q_INVOKABLE void checkServer(const QString &username, const QString &password, const QString &server);
    Q_INVOKABLE void finish(bool contactsEnabled);
    bool isWorking();
    bool noError();
    QString errorMessage() const;

Q_SIGNALS:
    void isWorkingChanged();
    void noErrorChanged();
    void errorMessageChanged();
    void wizardFinished(const QString &username, const QString &password, const QVariantMap &data);

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
    bool m_noError;

};

#endif // QMLHELPER_H
