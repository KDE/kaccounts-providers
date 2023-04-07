/*
 *  SPDX-FileCopyrightText: 2012 Alejandro Fiestas Olivares <afiestas@kde.org>
 *  SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "owncloud.h"
#include "owncloudcontroller.h"

#include <KLocalizedContext>
#include <KLocalizedString>
#include <KPackage/PackageLoader>

#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QWindow>

OwnCloudWizard::OwnCloudWizard(QObject *parent)
    : KAccountsUiPlugin(parent)
{
    qmlRegisterUncreatableType<OwncloudController>("org.kde.kaccounts.owncloud", 1, 0, "OwncloudController", QStringLiteral("Only for enums"));
}

OwnCloudWizard::~OwnCloudWizard()
{
}

void OwnCloudWizard::init(KAccountsUiPlugin::UiType type)
{
    if (type == KAccountsUiPlugin::NewAccountDialog) {
        const QString packagePath(QStringLiteral("org.kde.kaccounts.owncloud"));
        m_engine = new QQmlApplicationEngine(this);

        auto ctx = new KLocalizedContext(m_engine);
        ctx->setTranslationDomain(packagePath);
        m_engine->rootContext()->setContextObject(ctx);

        KPackage::Package package = KPackage::PackageLoader::self()->loadPackage(QStringLiteral("KPackage/GenericQML"));
        package.setPath(packagePath);
        m_data = package.metadata();

        OwncloudController *helper = new OwncloudController(m_engine);

        connect(helper, &OwncloudController::wizardFinished, this, [this](const QString &username, const QString &password, const QVariantMap &data) {
            m_engine->deleteLater();
            Q_EMIT success(username, password, data);
        });

        connect(helper, &OwncloudController::wizardCancelled, this, [this] {
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

void OwnCloudWizard::setProviderName(const QString &providerName)
{
    Q_UNUSED(providerName)
}

void OwnCloudWizard::showNewAccountDialog()
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

void OwnCloudWizard::showConfigureAccountDialog(const quint32 accountId)
{
    Q_UNUSED(accountId)
}

QStringList OwnCloudWizard::supportedServicesForConfig() const
{
    return QStringList();
}
