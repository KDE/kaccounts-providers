add_library(nextcloud_plugin_kaccounts MODULE
    nextcloud.cpp
    nextcloudcontroller.cpp
)

target_link_libraries(nextcloud_plugin_kaccounts
                       Qt::Core
                       KF6::KIOCore
                       KF6::I18n
                       KF6::Package
                       QCoro::Core
                       KAccounts6
)

target_link_libraries(nextcloud_plugin_kaccounts Qt::WebEngineQuick)

install(TARGETS nextcloud_plugin_kaccounts
         DESTINATION ${KDE_INSTALL_PLUGINDIR}/kaccounts/ui
)
kpackage_install_package(package org.kde.kaccounts.nextcloud genericqml)

ecm_install_icons(
ICONS
    sc-apps-kaccounts-nextcloud.svg
DESTINATION
    ${KDE_INSTALL_ICONDIR}
)
