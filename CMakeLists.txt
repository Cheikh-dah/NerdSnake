cmake_minimum_required(VERSION 3.15)
project(TermGenerator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)

find_package(nlohmann_json 3.11.0 QUIET)
if(nlohmann_json_FOUND)
    message(STATUS "Found nlohmann_json: ${nlohmann_json_DIR}")
else()
    message(STATUS "nlohmann_json not found, will use downloaded version")
    include(FetchContent)
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

add_executable(generate_terms
    generate_terms.cpp
    dotenv.cpp
)

target_include_directories(generate_terms PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CURL_INCLUDE_DIRS}
)

target_link_libraries(generate_terms PRIVATE
    ${CURL_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    nlohmann_json::nlohmann_json
)

if(MSVC)
    target_compile_options(generate_terms PRIVATE /W4)
else()
    target_compile_options(generate_terms PRIVATE -Wall -Wextra -Wpedantic)
endif()
