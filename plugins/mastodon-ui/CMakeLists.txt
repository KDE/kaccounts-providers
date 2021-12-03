add_library(mastodon_plugin_kaccounts MODULE
    mastodon.cpp
    mastodoncontroller.cpp
)

target_link_libraries(mastodon_plugin_kaccounts
                       Qt5::Core
                       KF5::I18n
                       KF5::Declarative
                       KAccounts
)

install(TARGETS mastodon_plugin_kaccounts
         DESTINATION ${PLUGIN_INSTALL_DIR}/kaccounts/ui
)
kpackage_install_package(package org.kde.kaccounts.mastodon genericqml)
