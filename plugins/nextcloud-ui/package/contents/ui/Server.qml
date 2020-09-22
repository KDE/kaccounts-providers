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

    header: Kirigami.InlineMessage {
        type: Kirigami.MessageType.Error
        text: helper.errorMessage
        visible: text.length > 0
    }

    Kirigami.FormLayout {
        anchors.centerIn: parent
        visible: !busy.running
        TextField {
            id: serverText
            placeholderText: "https://nextcloud.provider.com"
            Kirigami.FormData.label: i18n("Server address:")
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
                onClicked: {
                    helper.checkServer(serverText.text)
                }
            }
         }
    }
}
