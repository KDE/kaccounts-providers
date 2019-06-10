project (nextcloud-ui-plugin)

include_directories (${CMAKE_CURRENT_BUILD_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
)

set (nextcloud_plugin_kaccounts_SRCS
     nextcloud.cpp
     qmlhelper.cpp
)

add_library (nextcloud_plugin_kaccounts MODULE
             ${nextcloud_plugin_kaccounts_SRCS}
)

target_link_libraries (nextcloud_plugin_kaccounts
                       Qt5::Core
                       Qt5::WebEngineWidgets
                       KF5::KIOCore
                       KF5::I18n
                       KF5::Declarative
                       KAccounts
                       ${ACCOUNTSQT_LIBRARIES}
                       ${SIGNONQT_LIBRARIES}
)

# Install:
install (TARGETS nextcloud_plugin_kaccounts
         DESTINATION ${PLUGIN_INSTALL_DIR}/kaccounts/ui
)
kpackage_install_package(package org.kde.kaccounts.nextcloud genericqml)
