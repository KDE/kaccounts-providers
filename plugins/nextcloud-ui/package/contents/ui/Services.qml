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

    title: i18n("Choose services to enable")

    ColumnLayout {
        anchors.fill: parent

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
