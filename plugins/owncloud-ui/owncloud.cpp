/*
 *  SPDX-FileCopyrightText: 2012 Alejandro Fiestas Olivares <afiestas@kde.org>
 *  SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "owncloud.h"
#include "owncloudcontroller.h"

#include <KDeclarative/QmlObject>
#include <KLocalizedString>
#include <KPackage/PackageLoader>

#include <QQmlContext>
#include <QQmlEngine>
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
        m_object = new KDeclarative::QmlObject();
        m_object->setTranslationDomain(packagePath);
        m_object->setInitializationDelayed(true);

        KPackage::Package package = KPackage::PackageLoader::self()->loadPackage(QStringLiteral("KPackage/GenericQML"));
        package.setPath(packagePath);
        m_object->setSource(QUrl::fromLocalFile(package.filePath("mainscript")));
        m_data = package.metadata();

        OwncloudController *helper = new OwncloudController(m_object);

        connect(helper, &OwncloudController::wizardFinished, this, [this](const QString &username, const QString &password, const QVariantMap &data) {
            m_object->deleteLater();
            Q_EMIT success(username, password, data);
        });

        connect(helper, &OwncloudController::wizardCancelled, this, [this] {
            m_object->deleteLater();
            Q_EMIT canceled();
        });

        m_object->engine()->rootContext()->setContextProperty(QStringLiteral("helper"), helper);

        m_object->completeInitialization();

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
    QWindow *window = qobject_cast<QWindow *>(m_object->rootObject());
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
