set(nextcloud_plugin_kaccounts_SRCS
     nextcloud.cpp
     nextcloudcontroller.cpp
)

add_library(nextcloud_plugin_kaccounts MODULE
             ${nextcloud_plugin_kaccounts_SRCS}
)

target_link_libraries(nextcloud_plugin_kaccounts
                       Qt5::Core
                       Qt5::WebEngine
                       KF5::KIOCore
                       KF5::I18n
                       KF5::Declarative
                       KAccounts
)

install(TARGETS nextcloud_plugin_kaccounts
         DESTINATION ${PLUGIN_INSTALL_DIR}/kaccounts/ui
)
kpackage_install_package(package org.kde.kaccounts.nextcloud genericqml)

ecm_install_icons(
ICONS
    sc-apps-kaccounts-nextcloud.svg
DESTINATION
    ${ICON_INSTALL_DIR}
)
