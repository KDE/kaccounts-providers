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

Kirigami.ScrollablePage {
    id: root
    title: i18n("Services")

    property var disabledServices: []

    ListView {
        model: helper.availableServices

        clip: true

        // Cheap copy of Kirigami.BasicListItem with CheckBox instead of Icon
        delegate: Kirigami.AbstractListItem {
            id: listItem
            highlighted: false
            onClicked: serviceCheck.toggle()

            RowLayout {
                CheckBox {
                    id: serviceCheck
                    Layout.alignment: Qt.AlignVCenter
                    checked: true
                    onCheckedChanged: {
                        if (serviceCheck.checked) {
                            const idx = root.disabledServices.indexOf(modelData.id);
                            if (idx > -1) {
                                root.disabledServices.splice(idx, 1);
                            }
                        } else {
                            root.disabledServices.push(modelData.id)
                        }
                    }
                }

                ColumnLayout {
                    spacing: 0
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignVCenter

                    Label {
                        Layout.fillWidth: true
                        text: modelData.name
                        color: listItem.pressed ? listItem.activeTextColor : listItem.textColor
                        elide: Text.ElideRight
                    }

                    Label {
                        Layout.fillWidth: true
                        text: modelData.description
                        color: listItem.pressed ? listItem.activeTextColor : listItem.textColor
                        elide: Text.ElideRight
                        font: Kirigami.Theme.smallFont
                        opacity: 0.7
                        visible: text.length > 0
                    }
                }
            }
        }
    }

    footer: ToolBar {
         RowLayout {
            anchors.fill: parent

            Button {
                text: i18n("Finish")
                Layout.alignment: Qt.AlignRight
                onClicked: {
                    helper.finish(root.disabledServices)
                }
            }
         }
    }
}
