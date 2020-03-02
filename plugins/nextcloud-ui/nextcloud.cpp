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

#include "nextcloud.h"
#include "qmlhelper.h"

#include <KLocalizedString>
#include <KDeclarative/QmlObject>

#include <QQmlEngine>
#include <QQmlContext>

NextcloudWizard::NextcloudWizard(QObject *parent)
    : KAccountsUiPlugin(parent)
{
}

NextcloudWizard::~NextcloudWizard()
{
}

void NextcloudWizard::init(KAccountsUiPlugin::UiType type)
{
    if (type == KAccountsUiPlugin::NewAccountDialog) {

        const QString packagePath("org.kde.kaccounts.nextcloud");

        m_object = new KDeclarative::QmlObject();
        m_object->setTranslationDomain(packagePath);
        m_object->setInitializationDelayed(true);
        m_object->loadPackage(packagePath);

        QmlHelper *helper = new QmlHelper(m_object);
        connect(helper, &QmlHelper::wizardFinished, this, &NextcloudWizard::success);
        connect(helper, &QmlHelper::wizardFinished, [=] {
            QWindow *window = qobject_cast<QWindow *>(m_object->rootObject());
            if (window) {
                window->close();
            }
            m_object->deleteLater();
        });
        m_object->engine()->rootContext()->setContextProperty("helper", helper);

        m_object->completeInitialization();

        if (!m_object->package().metadata().isValid()) {
            return;
        }

        Q_EMIT uiReady();
    }

}

void NextcloudWizard::setProviderName(const QString &providerName)
{
    //TODO: unused?
    m_providerName = providerName;
}

void NextcloudWizard::showNewAccountDialog()
{
    QWindow *window = qobject_cast<QWindow *>(m_object->rootObject());
    if (window) {
        window->setTransientParent(transientParent());
        window->show();
        window->requestActivate();
        window->setTitle(m_object->package().metadata().name());
        window->setIcon(QIcon::fromTheme(m_object->package().metadata().iconName()));
    }
}

void NextcloudWizard::showConfigureAccountDialog(const quint32 accountId)
{

}

QStringList NextcloudWizard::supportedServicesForConfig() const
{
    return QStringList();
}
