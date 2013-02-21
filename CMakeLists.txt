project(kde-account-providers)

cmake_minimum_required(VERSION 2.6)
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

find_package(KDE4 REQUIRED)
find_package(AccountsFileDir REQUIRED)

include(KDE4Defaults)

add_subdirectory(icons)
add_subdirectory(services)
add_subdirectory(providers)