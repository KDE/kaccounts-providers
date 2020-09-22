/*
 *  SPDX-FileCopyrightText: 2019 Rituka Patwal <ritukapatwal21@gmail.com>
 *  SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.2
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5
import QtWebEngine 1.10

import org.kde.kirigami 2.5 as Kirigami

Kirigami.Page {

    globalToolBarStyle: Kirigami.ApplicationHeaderStyle.None
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    WebEngineView {
        anchors.fill: parent
        url: helper.loginUrl
        profile: helper.webengineProfile
    }
}
