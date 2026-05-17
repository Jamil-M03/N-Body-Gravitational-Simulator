cmake_minimum_required(VERSION 3.15)
project(nbody CXX)

# --- Language standard ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # use -std=c++20, not -std=gnu++20

# --- Default to Release if no build type was specified ---
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release)
endif()

# --- Auto-fetch nlohmann/json.hpp if not already vendored ---
set(JSON_HEADER "${CMAKE_SOURCE_DIR}/third_party/nlohmann/json.hpp")
if(NOT EXISTS "${JSON_HEADER}")
    message(STATUS "json.hpp not found; downloading...")
    file(DOWNLOAD
        "https://raw.githubusercontent.com/nlohmann/json/v3.11.3/single_include/nlohmann/json.hpp"
        "${JSON_HEADER}"
        SHOW_PROGRESS
        TLS_VERIFY ON
        STATUS DL_STATUS
    )
    list(GET DL_STATUS 0 DL_CODE)
    if(NOT DL_CODE EQUAL 0)
        message(FATAL_ERROR "Could not download json.hpp. "
                            "Either supply it manually at third_party/nlohmann/json.hpp "
                            "or check your internet connection.")
    endif()
endif()

# --- The executable ---
file(GLOB SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(nbody ${SOURCES})

target_include_directories(nbody PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/third_party"
)

# --- Warnings: equivalent of -Wall -Wextra across compilers ---
if(MSVC)
    target_compile_options(nbody PRIVATE /W4 /permissive-)
else()
    target_compile_options(nbody PRIVATE -Wall -Wextra)
endif()