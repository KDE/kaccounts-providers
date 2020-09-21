/*
 *  SPDX-FileCopyrightText: 2012 Alejandro Fiestas Olivares <afiestas@kde.org>
 *  SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "owncloud.h"
#include "owncloudcontroller.h"

#include <KLocalizedString>
#include <KDeclarative/QmlObject>

#include <QQmlEngine>
#include <QQmlContext>
#include <QWindow>

OwnCloudWizard::OwnCloudWizard(QObject *parent)
    : KAccountsUiPlugin(parent)
{
}

OwnCloudWizard::~OwnCloudWizard()
{
}

void OwnCloudWizard::init(KAccountsUiPlugin::UiType type)
{
    if (type == KAccountsUiPlugin::NewAccountDialog) {

        const QString packagePath("org.kde.kaccounts.owncloud");

        m_object = new KDeclarative::QmlObject();
        m_object->setTranslationDomain(packagePath);
        m_object->setInitializationDelayed(true);
        m_object->loadPackage(packagePath);

        OwncloudController *helper = new OwncloudController(m_object);
        connect(helper, &OwncloudController::wizardFinished, this, &OwnCloudWizard::success);
        connect(helper, &OwncloudController::wizardFinished, [=] {
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

void OwnCloudWizard::setProviderName(const QString &providerName)
{
    Q_UNUSED(providerName)
}

void OwnCloudWizard::showNewAccountDialog()
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

void OwnCloudWizard::showConfigureAccountDialog(const quint32 accountId)
{

}

QStringList OwnCloudWizard::supportedServicesForConfig() const
{
    return QStringList();
}
