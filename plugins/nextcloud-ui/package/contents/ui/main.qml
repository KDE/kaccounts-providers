/*
 *   Copyright 2019 (C) Rituka Patwal <ritukapatwal21@gmail.com>
 *   Copyright 2015 (C) Martin Klapetek <mklapetek@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.2
import org.kde.kirigami 2.5 as Kirigami
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5

Kirigami.ApplicationWindow {
    id: ncAccountRoot
    objectName: "_root"

    width: 400; height: 350

    pageStack.initialPage: mainPageComponent

    Component {
        id: mainPageComponent

        Kirigami.Page {
            id: pane
            title: "Nextcloud Login"
            background: Rectangle {
                Kirigami.Theme.colorSet: Kirigami.Theme.View
                color: Kirigami.Theme.backgroundColor
            }
            ColumnLayout {
                spacing: 10
                anchors.fill: parent

                visible: !helper.isLoginComplete

                TextField {
                    id: serverText
                    placeholderText: qsTr("Server address https://...")
                    Layout.alignment: Qt.AlignCenter
                    Layout.maximumWidth: Kirigami.Units.gridUnit * 20
                    Layout.fillWidth: true
                }
                Label {
                    id: errorLabel
                    Layout.fillWidth: true
                    visible: text.length > 0
                    text: helper.errorMessage
                    wrapMode: Text.WordWrap
                }
                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    BusyIndicator {
                        id: busy
                        width: 50
                        height: 50
                        Layout.alignment: Qt.AlignCenter
                        running: helper.isWorking
                        visible: running
                    }
                }

                ToolButton {
                    id: loginButton
                    icon.name: "plasma-browser-integration"
                    icon.color: "black"
                    icon.width: 120
                    icon.height: 100
                    text:"login"
                    display: ToolButton.TextUnderIcon
                    onClicked:{
                        helper.checkServer(serverText.text);
                    }
                    Layout.alignment: Qt.AlignCenter
                }

            }
            ColumnLayout {
                id: services
                visible: helper.isLoginComplete

                Label {
                    text: i18n("Choose services to enable");
                }

                CheckBox {
                    id: contactsService
                    text: i18n("Contacts")
                }
                Button {
                    id: finishButton
                    Layout.fillWidth: true
                    width: 120
                    height: 50
                    text: i18n("Finish")

                    onClicked: {
                        helper.finish(contactsService.checked);
                    }
                }
            }
        }
    }
}