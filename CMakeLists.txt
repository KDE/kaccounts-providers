cmake_minimum_required(VERSION 3.16)

set(RELEASE_SERVICE_VERSION_MAJOR "25")
set(RELEASE_SERVICE_VERSION_MINOR "11")
set(RELEASE_SERVICE_VERSION_MICRO "70")
set(KACCOUNTS_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(kde-account-providers LANGUAGES CXX VERSION ${KACCOUNTS_VERSION})

set(QT_REQUIRED_VERSION "6.5.0")
set(KF_MIN_VERSION "6.0")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(ECMInstallIcons)
include(KDEClangFormat)
include(KDEGitCommitHooks)
include(FeatureSummary)
include(ECMDeprecationSettings)

find_package(KAccounts6 REQUIRED)

find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED Core Qml)

find_package(Qt6WebEngineQuick ${QT_REQUIRED_VERSION} CONFIG)
set_package_properties(Qt6WebEngineQuick
        PROPERTIES TYPE OPTIONAL
        PURPOSE "Required for Nextcloud Accounts plugin"
        )
set(HAVE_WEBENGINE ${Qt6WebEngineQuick_FOUND})

find_package(KF6 ${KF_MIN_VERSION} REQUIRED KIO I18n Package)

find_package(QCoro6 REQUIRED COMPONENTS Core Network)

qcoro_enable_coroutines()

ecm_set_disabled_deprecation_versions(QT 5.15.2
     KF 5.101.0
    )

add_subdirectory(services)
add_subdirectory(providers)
add_subdirectory(webkit-options)
add_subdirectory(plugins)

ki18n_install(po)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
