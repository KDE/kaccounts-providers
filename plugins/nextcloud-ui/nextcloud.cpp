/*
 *  SPDX-FileCopyrightText: 2019 Rituka Patwal <ritukapatwal21@gmail.com>
 *  SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
 *  SPDX-FileCopyrightText: 2012 Alejandro Fiestas Olivares <afiestas@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "nextcloud.h"
#include "nextcloudcontroller.h"

#include <KLocalizedContext>
#include <KLocalizedString>
#include <KPackage/PackageLoader>

#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QWindow>

NextcloudWizard::NextcloudWizard(QObject *parent)
    : KAccountsUiPlugin(parent)
{
    qmlRegisterUncreatableType<NextcloudController>("org.kde.kaccounts.nextcloud", 1, 0, "NextcloudController", QStringLiteral("Only for enums"));
}

NextcloudWizard::~NextcloudWizard()
{
}

void NextcloudWizard::init(KAccountsUiPlugin::UiType type)
{
    QtWebEngineQuick::initialize();

    if (type == KAccountsUiPlugin::NewAccountDialog) {
        const QString packagePath(QStringLiteral("org.kde.kaccounts.nextcloud"));
        m_engine = new QQmlApplicationEngine;

        auto ctx = new KLocalizedContext(m_engine);
        ctx->setTranslationDomain(packagePath);
        m_engine->rootContext()->setContextObject(ctx);

        KPackage::Package package = KPackage::PackageLoader::self()->loadPackage(QStringLiteral("KPackage/GenericQML"));
        package.setPath(packagePath);
        m_data = package.metadata();
        NextcloudController *helper = new NextcloudController(m_engine);

        connect(helper, &NextcloudController::wizardFinished, this, [this](const QString &username, const QString &password, const QVariantMap &data) {
            m_engine->deleteLater();
            Q_EMIT success(username, password, data);
        });

        connect(helper, &NextcloudController::wizardCancelled, this, [this] {
            m_engine->deleteLater();
            Q_EMIT canceled();
        });

        m_engine->rootContext()->setContextProperty(QStringLiteral("helper"), helper);
        m_engine->load(QUrl::fromLocalFile(package.filePath("mainscript")));

        if (!m_data.isValid()) {
            return;
        }

        Q_EMIT uiReady();
    }
}

void NextcloudWizard::setProviderName(const QString &providerName)
{
    Q_UNUSED(providerName)
}

void NextcloudWizard::showNewAccountDialog()
{
    QWindow *window = qobject_cast<QWindow *>(m_engine->rootObjects().first());
    if (window) {
        window->setTransientParent(transientParent());
        window->show();
        window->requestActivate();
        window->setTitle(m_data.name());
        window->setIcon(QIcon::fromTheme(m_data.iconName()));
    }
}

void NextcloudWizard::showConfigureAccountDialog(const quint32 accountId)
{
    Q_UNUSED(accountId)
}

QStringList NextcloudWizard::supportedServicesForConfig() const
{
    return QStringList();
}
