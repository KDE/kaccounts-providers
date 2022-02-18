/*
 *  SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
 *  SPDX-FileCopyrightText: 2019 Rituka Patwal <ritukapatwal21@gmail.com>
 *  SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.2
import org.kde.kirigami 2.8 as Kirigami
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5

Kirigami.Page {
    title: i18n("ownCloud Login")

    header: Kirigami.InlineMessage {
        type: Kirigami.MessageType.Error
        text: helper.errorMessage
        visible: text.length > 0
    }

    Column {

        width: parent.width
        height: childrenRect.height
        anchors.centerIn: parent
        visible: !busy.running

        Kirigami.Icon {
            source: "kaccounts-owncloud"
            width: Kirigami.Units.gridUnit * 6
            height: width
            anchors.horizontalCenter: form.horizontalCenter
        }

        Item {
            width: 1
            height: Kirigami.Units.gridUnit
        }

        Kirigami.FormLayout {
            id: form
            width: parent.width

            TextField {
                id: nameText
                Kirigami.FormData.label: i18n("Username:")
            }

            Kirigami.PasswordField {
                id: passwordText
                Kirigami.FormData.label: i18n("Password:")
            }

            TextField {
                id: serverText
                placeholderText: "https://owncloud.provider.com"
                inputMethodHints: Qt.ImhUrlCharactersOnly
                Kirigami.FormData.label: i18n("Server address:")
            }
        }
    }

    BusyIndicator {
        id: busy
        anchors.centerIn: parent
        running: helper.isWorking
    }

    footer: ToolBar {
         RowLayout {
            anchors.fill: parent

            Button {
                text: i18n("Next")
                Layout.alignment: Qt.AlignRight
                enabled: serverText.text.length > 0 && nameText.text.length > 0 && passwordText.text.length > 0 // TODO Do a more thorough validation of the URL
                onClicked: {
                    helper.checkServer(nameText.text, passwordText.text, serverText.text);
                }
            }
         }
    }
}
