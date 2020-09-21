/*
 *   Copyright 2015 (C) Martin Klapetek <mklapetek@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2

ApplicationWindow {
    id: kaccountsRoot
    objectName: "_root"

    width: 400; height: 250

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: units.largeSpacing

        Label {
            text: i18n("Add new ownCloud account")
        }

        StackView {
            id: stack

            Layout.fillHeight: true
            Layout.fillWidth: true

            initialItem: BasicInfo {
                id: basicInfoPage
                objectName: "basicInfoPage"
            }
        }

        Component {
            id: servicesComponent

            Services {
                objectName: "servicesPage"
            }
        }

        RowLayout {
            Button {
                id: backButton
                Layout.fillWidth: true
                text: i18n("Back");
                enabled: stack.currentItem.objectName == "servicesPage"

                onClicked: {
                    if (stack.currentItem.objectName == "servicesPage") {
                        stack.pop(servicesComponent);
                    }
                }
            }

            Button {
                id: nextButton
                Layout.fillWidth: true
                text: i18n("Next")
                enabled: basicInfoPage.canContinue //: false
                visible: stack.currentItem == basicInfoPage

                onClicked: {
                    if (stack.currentItem == basicInfoPage) {
                        stack.push(servicesComponent);
                    }
                }
            }

            Button {
                id: finishButton
                Layout.fillWidth: true
                text: i18n("Finish")
                visible: stack.currentItem.objectName == "servicesPage"

                onClicked: {
                    helper.finish(stack.currentItem.contactsEnabled);
                }
            }
        }
    }
}
