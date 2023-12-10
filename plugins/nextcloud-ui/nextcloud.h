/*
 *  SPDX-FileCopyrightText: 2019 Rituka Patwal <ritukapatwal21@gmail.com>
 *  SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
 *  SPDX-FileCopyrightText: 2012 Alejandro Fiestas Olivares <afiestas@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef NEXTCLOUD_H
#define NEXTCLOUD_H

#include <KPluginMetaData>
#include <QHash>
#include <QUrl>

#include <KAccounts/KAccountsUiPlugin>

class QQmlApplicationEngine;

class NextcloudWizard : public KAccounts::KAccountsUiPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.kaccounts.UiPlugin")
    Q_INTERFACES(KAccounts::KAccountsUiPlugin)

public:
    explicit NextcloudWizard(QObject *parent = nullptr);
    virtual ~NextcloudWizard();

    virtual void init(KAccountsUiPlugin::UiType type) override;
    virtual void setProviderName(const QString &providerName) override;
    virtual void showNewAccountDialog() override;
    virtual void showConfigureAccountDialog(const quint32 accountId) override;
    virtual QStringList supportedServicesForConfig() const override;

private:
    QHash<QString, int> m_services;
    QQmlApplicationEngine *m_engine;
    KPluginMetaData m_data;
};

#endif // NEXTCLOUD_H
