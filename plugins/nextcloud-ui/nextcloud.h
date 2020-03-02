/*************************************************************************************
 *  Copyright (C) 2019 by Rituka Patwal <ritukapatwal21@gmail.com>                   *
 *  Copyright (C) 2015 by Martin Klapetek <mklapetek@kde.org>                        *
 *  Copyright (C) 2012 by Alejandro Fiestas Olivares <afiestas@kde.org>              *
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

#ifndef NEXTCLOUD_H
#define NEXTCLOUD_H

#include <QUrl>
#include <QHash>

#include <kaccountsuiplugin.h>

namespace KDeclarative {
    class QmlObject;
}

class NextcloudWizard : public KAccountsUiPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.kaccounts.UiPlugin")
    Q_INTERFACES(KAccountsUiPlugin)

public:
    explicit NextcloudWizard(QObject *parent = 0);
    virtual ~NextcloudWizard();

    virtual void init(KAccountsUiPlugin::UiType type) override;
    virtual void setProviderName(const QString &providerName) override;
    virtual void showNewAccountDialog() override;
    virtual void showConfigureAccountDialog(const quint32 accountId) override;
    virtual QStringList supportedServicesForConfig() const override;

private:
    QString m_providerName;
    QHash<QString, int> m_services;
    KDeclarative::QmlObject *m_object;
};

#endif //NEXTCLOUD_H
