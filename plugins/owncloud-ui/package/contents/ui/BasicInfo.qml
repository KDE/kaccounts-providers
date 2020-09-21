/*
 *   Copyright 2015 (C) Martin Klapetek <mklapetek@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import org.kde.plasma.core 2.0 as PlasmaCore

ColumnLayout {
    id: basicInfoLayout
    property bool canContinue: !helper.isWorking && helper.noError && nameText.text.length > 0 && passwordText.text.length > 0
    property bool canRestartTimer: nameText.text.length > 0 && passwordText.text.length > 0 && serverText.text.length > 0

    Timer {
        id: checkServerTimer
        interval: 1000
        repeat: false
        running: false

        onTriggered: {
            helper.checkServer(nameText.text, passwordText.text, serverText.text);
        }
    }

    TextField {
        id: nameText
        Layout.fillWidth: true
//         clearButtonShown: true
        placeholderText: "Username"

        onTextChanged: {
            if (basicInfoLayout.canRestartTimer) {
                checkServerTimer.restart();
            }
        }
    }

    TextField {
        id: passwordText
        Layout.fillWidth: true
//         clearButtonShown: true
        placeholderText: "Password"
        echoMode: TextInput.Password

        onTextChanged: {
            if (basicInfoLayout.canRestartTimer) {
                checkServerTimer.restart();
            }
        }
    }

    TextField {
        id: serverText
        Layout.fillWidth: true
//         clearButtonShown: true
        placeholderText: "Server"

        onTextChanged: {
            if (basicInfoLayout.canRestartTimer) {
                checkServerTimer.restart();
            }
        }
    }

    Label {
        id: errorLabel
        Layout.fillWidth: true
        visible: text.length > 0 && !checkServerTimer.running
        text: helper.errorMessage
        wrapMode: Text.WordWrap
    }

    Item {
        Layout.fillWidth: true
        Layout.fillHeight: true

        BusyIndicator {
            id: busy
            width: parent.height > parent.width ? Math.round(parent.width/2) : Math.round(parent.height/2)
            height: width
            anchors.centerIn: parent
            running: helper.isWorking
            visible: running
        }

        PlasmaCore.IconItem {
            width: busy.width
            height: width
            anchors.centerIn: parent
            source: "dialog-ok"
            visible: !helper.isWorking && helper.noError && !errorLabel.visible
        }
    }
}
