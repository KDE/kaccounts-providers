project(kde-account-providers)

cmake_minimum_required(VERSION 2.8.12)

find_package(ECM 1.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(AccountsFileDir REQUIRED)
find_package(Intltool REQUIRED)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(FeatureSummary)
include(ECMInstallIcons)

add_subdirectory(icons)
add_subdirectory(services)
add_subdirectory(providers)
add_subdirectory(webkit-options)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
