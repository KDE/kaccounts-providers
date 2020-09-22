/*
 *  SPDX-FileCopyrightText: 2019 Rituka Patwal <ritukapatwal21@gmail.com>
 *  SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.2
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5

import org.kde.kirigami 2.5 as Kirigami
import org.kde.kaccounts.nextcloud 1.0

Kirigami.ApplicationWindow {
    id: ncAccountRoot

    // Minimum size at which the web login UI looks good
    minimumWidth: Kirigami.Units.gridUnit * 24
    minimumHeight: Kirigami.Units.gridUnit * 35
    width: minimumWidth
    height: minimumHeight

    pageStack.initialPage: Qt.resolvedUrl("Server.qml");

    Connections {
        target: helper

        function onStateChanged() {
            if (helper.state === NextcloudController.WebLogin) {
                ncAccountRoot.pageStack.replace(Qt.resolvedUrl("WebLogin.qml"))
            } else if (helper.state === NextcloudController.Services) {
                ncAccountRoot.pageStack.replace(Qt.resolvedUrl("Services.qml"))
            }
        }
    }
}
