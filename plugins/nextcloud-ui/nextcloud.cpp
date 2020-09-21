/*
 *  SPDX-FileCopyrightText: 2019 Rituka Patwal <ritukapatwal21@gmail.com>
 *  SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
 *  SPDX-FileCopyrightText: 2012 Alejandro Fiestas Olivares <afiestas@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

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
