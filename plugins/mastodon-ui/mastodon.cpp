/*
 *  SPDX-FileCopyrightText: 2019 Rituka Patwal <ritukapatwal21@gmail.com>
 *  SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
 *  SPDX-FileCopyrightText: 2012 Alejandro Fiestas Olivares <afiestas@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "mastodon.h"
#include "mastodoncontroller.h"

#include <KDeclarative/QmlObject>
#include <KLocalizedString>

#include <QIcon>
#include <QQmlContext>
#include <QQmlEngine>
#include <QWindow>

MastodonWizard::MastodonWizard(QObject *parent)
    : KAccountsUiPlugin(parent)
{
}

MastodonWizard::~MastodonWizard()
{
}

void MastodonWizard::init(KAccountsUiPlugin::UiType type)
{
    if (type == KAccountsUiPlugin::NewAccountDialog) {
        const QString packagePath(QStringLiteral("org.kde.kaccounts.mastodon"));

        m_object = new KDeclarative::QmlObject();
        m_object->setTranslationDomain(packagePath);
        m_object->setInitializationDelayed(true);
        m_object->loadPackage(packagePath);

        MastodonController *helper = new MastodonController(m_object);

        connect(helper, &MastodonController::wizardCancelled, this, [this] {
            m_object->deleteLater();
            Q_EMIT canceled();
        });

        connect(helper, &MastodonController::startAuthSession, this, &MastodonWizard::startAuthSession);

        connect(helper, &MastodonController::startAuthSession, this, [this] {
            m_object->deleteLater();
        });

        m_object->engine()->rootContext()->setContextProperty(QStringLiteral("helper"), helper);

        m_object->completeInitialization();

        if (!m_object->package().metadata().isValid()) {
            return;
        }

        Q_EMIT uiReady();
    }
}

void MastodonWizard::setProviderName(const QString &providerName)
{
    Q_UNUSED(providerName)
}

void MastodonWizard::showNewAccountDialog()
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

void MastodonWizard::showConfigureAccountDialog(const quint32 accountId)
{
    Q_UNUSED(accountId)
}

QStringList MastodonWizard::supportedServicesForConfig() const
{
    return QStringList();
}
