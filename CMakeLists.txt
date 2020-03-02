project(kde-account-providers)

cmake_minimum_required(VERSION 3.5)

set(RELEASE_SERVICE_VERSION_MAJOR "20")
set(RELEASE_SERVICE_VERSION_MINOR "03")
set(RELEASE_SERVICE_VERSION_MICRO "70")
set(KACCOUNTS_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

set(QT_REQUIRED_VERSION "5.2.0")
set(KF5_MIN_VERSION "5.4.0")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Intltool REQUIRED)
find_package(KAccounts REQUIRED)

find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED Core Qml)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED KIO
                                                  I18n
                                                  Declarative
                                                  Package)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(FeatureSummary)
include(ECMInstallIcons)
include(KDECompilerSettings NO_POLICY_SCOPE)
if (EXISTS "${CMAKE_SOURCE_DIR}/.git")
   add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x060000)
   add_definitions(-DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x060000)
endif()

add_subdirectory(services)
add_subdirectory(providers)
add_subdirectory(webkit-options)
add_subdirectory(plugins)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
