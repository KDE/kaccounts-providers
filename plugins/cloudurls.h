/*
   SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>

   SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <QUrl>

QUrl createStatusUrl(const QString &input)
{
    QString fixedUrl;
    if (!input.startsWith(QLatin1String("http://")) && !input.startsWith(QLatin1String("https://"))) {
        fixedUrl.append("https://");
        fixedUrl.append(input);
    } else {
        fixedUrl = input;
    }

    QUrl url(fixedUrl);

    if (!url.path().endsWith(QLatin1Char('/'))) {
        url.setPath(url.path() + '/');
    }

    url.setPath(url.path() + "status.php");

    return url;
}
