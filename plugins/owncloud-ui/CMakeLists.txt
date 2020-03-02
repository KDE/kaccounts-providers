include_directories(${CMAKE_CURRENT_BUILD_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
)

set(owncloud_plugin_kaccounts_SRCS
     owncloud.cpp
     qmlhelper.cpp
)

add_library(owncloud_plugin_kaccounts MODULE
             ${owncloud_plugin_kaccounts_SRCS}
)

target_link_libraries(owncloud_plugin_kaccounts
                       Qt5::Core
                       KF5::KIOCore
                       KF5::I18n
                       KF5::Declarative
                       KAccounts
                       ${ACCOUNTSQT_LIBRARIES}
                       ${SIGNONQT_LIBRARIES}
)

# Install:
install(TARGETS owncloud_plugin_kaccounts
         DESTINATION ${PLUGIN_INSTALL_DIR}/kaccounts/ui
)
kpackage_install_package(package org.kde.kaccounts.owncloud genericqml)
