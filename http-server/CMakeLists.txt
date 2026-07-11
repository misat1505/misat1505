cmake_minimum_required(VERSION 3.10)

project(HttpServer LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

# FetchContent for third-party dependencies
include(FetchContent)

# nlohmann/json
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)

# GoogleTest
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.15.0
)
FetchContent_MakeAvailable(googletest)

# -------------------------
# Source files
# -------------------------
set(SRC_CORE
    src/core/exceptions/boomer_initialization_exception.cpp
    src/core/request/form_data.cpp
    src/core/request/form_data_parser.cpp
    src/core/request/request_parser.cpp
    src/core/request/request_start_line.cpp
    src/core/request/request.cpp
    src/core/headers/headers.cpp
    src/core/response/response_start_line.cpp
    src/core/response/response.cpp
    src/core/file_server.cpp
    src/core/router.cpp
    src/core/http_connection.cpp
)

set(SRC_SERVER
    src/boomer.cpp
    examples/main.cpp
)

# -------------------------
# Main executable
# -------------------------
add_executable(${PROJECT_NAME}
    ${SRC_CORE}
    ${SRC_SERVER}
)

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json uuid)

# -------------------------
# Tests
# -------------------------
enable_testing()

set(TEST_SOURCES
    tests/core/exceptions/boomer_initialization_exception_tests.cpp
    tests/core/request/form_data_parser_tests.cpp
    tests/core/request/form_data_tests.cpp
    tests/core/request/request_start_line_tests.cpp
    tests/core/request/request_tests.cpp
    tests/core/request/request_parser_tests.cpp
    tests/core/response/response_start_line_tests.cpp
    tests/core/response/response_tests.cpp
    tests/core/headers/headers_tests.cpp
    tests/core/file_server_tests.cpp
)

add_executable(${PROJECT_NAME}_tests
    ${SRC_CORE}
    ${TEST_SOURCES}
)

target_include_directories(${PROJECT_NAME}_tests PRIVATE include)
target_link_libraries(${PROJECT_NAME}_tests PRIVATE
    gtest_main
    gmock
    gmock_main
    nlohmann_json::nlohmann_json
    uuid
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}_tests)
