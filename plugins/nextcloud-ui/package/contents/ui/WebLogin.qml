/*
 *   Copyright 2019 (C) Rituka Patwal <ritukapatwal21@gmail.com>
 *   Copyright 2015 (C) Martin Klapetek <mklapetek@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.2
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5
import QtWebEngine 1.10

import org.kde.kirigami 2.5 as Kirigami

WebEngineView {
    url: helper.loginUrl
    profile: helper.webengineProfile
}
