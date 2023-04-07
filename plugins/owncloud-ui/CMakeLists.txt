add_library(owncloud_plugin_kaccounts MODULE
     owncloud.cpp
     owncloudcontroller.cpp
)

target_link_libraries(owncloud_plugin_kaccounts
                       Qt::Core
                       Qt::Quick
                       KF6::KIOCore
                       KF6::I18n
                       KF6::Package
                       KAccounts6
)

install(TARGETS owncloud_plugin_kaccounts
         DESTINATION ${KDE_INSTALL_PLUGINDIR}/kaccounts/ui
)
kpackage_install_package(package org.kde.kaccounts.owncloud genericqml)

ecm_install_icons(
ICONS
    256-apps-kaccounts-owncloud.png
DESTINATION
    ${KDE_INSTALL_ICONDIR}
)
