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

    width: Kirigami.Units.gridUnit * 25
    height: Kirigami.Units.gridUnit * 15

    pageStack.initialPage: mainPageComponent

    Component {
        id: mainPageComponent

        Kirigami.Page {
            title: i18n("Nextcloud Login")

            ColumnLayout {
                spacing: 10
                anchors.fill: parent

                visible: !helper.isLoginComplete

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
                    text: i18n("Finish")

                    onClicked: {
                        helper.finish(contactsService.checked);
                    }
                }
            }
        }
    }
}
