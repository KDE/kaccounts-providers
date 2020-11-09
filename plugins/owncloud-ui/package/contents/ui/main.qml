/*
 *  SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
 *  SPDX-FileCopyrightText: 2019 Rituka Patwal <ritukapatwal21@gmail.com>
 *  SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.2
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5

import org.kde.kirigami 2.5 as Kirigami
import org.kde.kaccounts.owncloud 1.0

Kirigami.ApplicationWindow {
    id: ocAccountRoot

    // Minimum size at which the UI looks okay
    minimumWidth: Kirigami.Units.gridUnit * 20 // matches the width of the PinePhone
    minimumHeight: Kirigami.Units.gridUnit * 35
    width: Kirigami.Units.gridUnit * 24 // making it a bit larger if available to make it more pleasant
    height: minimumHeight

    pageStack.initialPage: Qt.resolvedUrl("Server.qml");

    onClosing: {
        helper.cancel()
    }

    Connections {
        target: helper

        function onStateChanged() {
            if (helper.state === OwncloudController.Services) {
                ocAccountRoot.pageStack.replace(Qt.resolvedUrl("Services.qml"))
            }
        }
    }
}
