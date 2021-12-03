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
#include <QVariant>

class MastodonController : public QObject
{
    Q_OBJECT
public:
    MastodonController(QObject *parent = nullptr);

    Q_INVOKABLE void cancel();
    Q_INVOKABLE void checkServer(const QString &instanceUrl);

Q_SIGNALS:
    void stateChanged();
    void wizardCancelled();
    void startAuthSession(const QVariantMap &data);
};

#endif // QMLHELPER_H
