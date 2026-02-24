cmake_minimum_required(VERSION 3.20)

project(CPPCalorieTracker LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(CPPCalorieTracker
    src/main.cpp
    src/DatabaseManager.cpp
    external/sqlite/sqlite3.c
)

target_include_directories(CPPCalorieTracker
    PRIVATE
        ${CMAKE_SOURCE_DIR}/external/sqlite
)