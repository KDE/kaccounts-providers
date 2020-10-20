add_library(owncloud_plugin_kaccounts MODULE
     owncloud.cpp
     owncloudcontroller.cpp
)

target_link_libraries(owncloud_plugin_kaccounts
                       Qt5::Core
                       KF5::KIOCore
                       KF5::I18n
                       KF5::Declarative
                       KAccounts
)

install(TARGETS owncloud_plugin_kaccounts
         DESTINATION ${PLUGIN_INSTALL_DIR}/kaccounts/ui
)
kpackage_install_package(package org.kde.kaccounts.owncloud genericqml)

ecm_install_icons(
ICONS
    256-apps-kaccounts-owncloud.png
DESTINATION
    ${ICON_INSTALL_DIR}
)
