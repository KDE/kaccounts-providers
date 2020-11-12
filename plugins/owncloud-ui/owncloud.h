/*
 *  SPDX-FileCopyrightText: 2012 Alejandro Fiestas Olivares <afiestas@kde.org>
 *  SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef OWNCLOUD_H
#define OWNCLOUD_H

#include <QHash>
#include <QUrl>

#include <kaccountsuiplugin.h>

namespace KDeclarative
{
class QmlObject;
}

class OwnCloudWizard : public KAccountsUiPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.kaccounts.UiPlugin")
    Q_INTERFACES(KAccountsUiPlugin)

public:
    explicit OwnCloudWizard(QObject *parent = nullptr);
    virtual ~OwnCloudWizard();

    virtual void init(KAccountsUiPlugin::UiType type) override;
    virtual void setProviderName(const QString &providerName) override;
    virtual void showNewAccountDialog() override;
    virtual void showConfigureAccountDialog(const quint32 accountId) override;
    virtual QStringList supportedServicesForConfig() const override;

private:
    QHash<QString, int> m_services;
    KDeclarative::QmlObject *m_object;
};

#endif // OWNCLOUD_H
