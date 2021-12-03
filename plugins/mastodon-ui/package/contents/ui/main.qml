/*
 *  SPDX-FileCopyrightText: 2021 Nicolas Fella <nicolas.fella@gmx.de>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.2
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5

import org.kde.kirigami 2.5 as Kirigami

Kirigami.ApplicationWindow {
    id: ncAccountRoot

    minimumWidth: Kirigami.Units.gridUnit * 20 // matches the width of the PinePhone
    minimumHeight: Kirigami.Units.gridUnit * 35
    width: Kirigami.Units.gridUnit * 24 // making it a bit larger if available to make it more pleasant
    height: minimumHeight

    pageStack.initialPage: Kirigami.Page {
        title: i18n("Mastodon Login")

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
                source: "mastodon_microblog"
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
                    id: serverText
                    placeholderText: "https://mastodon.social"
                    Kirigami.FormData.label: i18n("Instance:")
                }
            }
        }

        footer: ToolBar {
            RowLayout {
                anchors.fill: parent

                Button {
                    text: i18n("Next")
                    Layout.alignment: Qt.AlignRight
                    enabled: serverText.text.length > 0 // TODO Do a more thorough validation of the URL
                    onClicked: {
                        helper.checkServer(serverText.text)
                    }
                }
            }
        }
    }

    onClosing: {
        helper.cancel()
    }
}
