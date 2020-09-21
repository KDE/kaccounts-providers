/*
 *   Copyright 2015 (C) Martin Klapetek <mklapetek@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2

ColumnLayout {
    id: basicInfoLayout
    property bool canContinue: true

    //FIXME at some point this should become a list of disabled services
    property alias contactsEnabled: contactsService.checked

    Label {
        text: i18n("Choose services to enable");
    }

    CheckBox {
        id: contactsService
        text: i18n("Contacts")
    }

    // Just an item padder
    Item {
        Layout.fillHeight: true
    }
}
