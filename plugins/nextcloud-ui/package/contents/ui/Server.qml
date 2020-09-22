/*
 *  SPDX-FileCopyrightText: 2019 Rituka Patwal <ritukapatwal21@gmail.com>
 *  SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.2
import org.kde.kirigami 2.5 as Kirigami
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5

Kirigami.Page {
    title: i18n("Nextcloud Login")

    ColumnLayout {
        spacing: 10
        anchors.fill: parent

        TextField {
            id: serverText
            placeholderText: qsTr("Server address")
            Layout.fillWidth: true
        }
        Label {
            id: errorLabel
            Layout.fillWidth: true
            visible: text.length > 0
            text: helper.errorMessage
            wrapMode: Text.WordWrap
        }
        BusyIndicator {
            id: busy
            width: Kirigami.Units.gridUnit * 2
            height: width
            Layout.alignment: Qt.AlignCenter
            running: helper.isWorking
            visible: running
        }

        Button {
            Layout.fillWidth: true
            id: loginButton
            icon.name: "go-next"
            text: i18n("Next")
            onClicked: helper.checkServer(serverText.text)
        }
    }
}
