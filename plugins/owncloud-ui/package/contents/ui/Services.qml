/*
 *  SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
 *  SPDX-FileCopyrightText: 2019 Rituka Patwal <ritukapatwal21@gmail.com>
 *  SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.2
import org.kde.kirigami as Kirigami
import org.kde.kirigami.delegates as KD
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5

Kirigami.ScrollablePage {
    id: root
    title: i18n("Services")

    property var disabledServices: []

    ListView {
        model: helper.availableServices

        clip: true

        delegate: KD.CheckSubtitleDelegate {
            width: ListView.view.width

            text: modelData.name
            subtitle: modelData.description

            highlighted: pressed || down

            onToggled: {
                if (checked) {
                    const idx = root.disabledServices.indexOf(modelData.id);
                    if (idx > -1) {
                        root.disabledServices.splice(idx, 1);
                    }
                } else {
                    root.disabledServices.push(modelData.id);
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
