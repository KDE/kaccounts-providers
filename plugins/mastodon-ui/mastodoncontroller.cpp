/*
 *  SPDX-FileCopyrightText: 2019 Rituka Patwal <ritukapatwal21@gmail.com>
 *  SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "mastodoncontroller.h"

#include <KLocalizedString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

MastodonController::MastodonController(QObject *parent)
    : QObject(parent)
{
}

void MastodonController::checkServer(const QString &instanceUrl)
{
    QNetworkAccessManager *nam = new QNetworkAccessManager;

    auto leUrl = QUrl(instanceUrl + QStringLiteral("/api/v1/apps"));

    qDebug() << leUrl;

    QNetworkRequest r(leUrl);
    r.setRawHeader("Content-Type", "application/json");

    QJsonDocument doc;
    QJsonObject obj;
    obj[QStringLiteral("client_name")] = QStringLiteral("KAccounts");
    obj[QStringLiteral("redirect_uris")] = QStringLiteral("http://localhost/kaccountsmastodon");
    obj[QStringLiteral("scopes")] = QStringLiteral("read write follow");
    doc.setObject(obj);

    auto reply = nam->post(r, doc.toJson());

    connect(reply, &QNetworkReply::finished, this, [this, reply, leUrl] {
        qDebug() << reply->error();
        auto replyData = reply->readAll();

        qDebug() << replyData;

        QJsonDocument doc = QJsonDocument::fromJson(replyData);

        QVariantMap data;
        data[QStringLiteral("auth/oauth2/web_server/Host")] = leUrl.host();
        data[QStringLiteral("auth/oauth2/web_server/ClientId")] = doc.object()[QStringLiteral("client_id")].toString();
        data[QStringLiteral("auth/oauth2/web_server/ClientSecret")] = doc.object()[QStringLiteral("client_secret")].toString();

        Q_EMIT startAuthSession(data);
    });
}

void MastodonController::cancel()
{
    Q_EMIT wizardCancelled();
}
