cmake_minimum_required(VERSION 3.20)

project(FTXUI_API_CLIENT LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)
include(FetchContent)

FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
    GIT_TAG v7.0.2
)

FetchContent_MakeAvailable(ftxui)

add_executable(ftxui-api-client
    src/main.cpp
    src/simplerapiclient.cpp
)

target_link_libraries(ftxui-api-client
    PRIVATE
        CURL::libcurl
        ftxui::screen
        ftxui::dom
        ftxui::component
)