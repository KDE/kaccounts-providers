/*
 *  SPDX-FileCopyrightText: 2019 Rituka Patwal <ritukapatwal21@gmail.com>
 *  SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
 *  SPDX-FileCopyrightText: 2012 Alejandro Fiestas Olivares <afiestas@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <QHash>
#include <QUrl>

#include <kaccountsuiplugin.h>

namespace KDeclarative
{
class QmlObject;
}

class MastodonWizard : public KAccountsUiPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.kaccounts.UiPlugin")
    Q_INTERFACES(KAccountsUiPlugin)

public:
    explicit MastodonWizard(QObject *parent = nullptr);
    virtual ~MastodonWizard();

    virtual void init(KAccountsUiPlugin::UiType type) override;
    virtual void setProviderName(const QString &providerName) override;
    virtual void showNewAccountDialog() override;
    virtual void showConfigureAccountDialog(const quint32 accountId) override;
    virtual QStringList supportedServicesForConfig() const override;

private:
    KDeclarative::QmlObject *m_object;
};
