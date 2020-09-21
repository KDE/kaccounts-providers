/*
 *   Copyright 2019 (C) Rituka Patwal <ritukapatwal21@gmail.com>
 *   Copyright 2015 (C) Martin Klapetek <mklapetek@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.2
import org.kde.kirigami 2.5 as Kirigami
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5

Kirigami.ApplicationWindow {
    id: ncAccountRoot
    objectName: "_root"

    width: Kirigami.Units.gridUnit * 25
    height: Kirigami.Units.gridUnit * 15

    pageStack.initialPage: mainPageComponent

    Component {
        id: mainPageComponent

        Kirigami.Page {
            title: i18n("Nextcloud Login")

            Loader {
                anchors.fill: parent
                source: helper.isLoginComplete ? Qt.resolvedUrl("Services.qml") : Qt.resolvedUrl("Server.qml")
            }
        }
    }
}
