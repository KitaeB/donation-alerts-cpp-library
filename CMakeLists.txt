cmake_minimum_required(VERSION 3.17)

IF (WIN32)
    SET(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
ENDIF()

project(da-cpp-lib VERSION 0.1)

# Указываем стандарт C++
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(CMAKE_BUILD_TYPE Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_library(${PROJECT_NAME} STATIC
    src/daApi.cpp)

# Находим библиотеки
find_package(nlohmann_json REQUIRED)
find_package(cpr REQUIRED)

target_include_directories(${PROJECT_NAME} PUBLIC include)

target_link_libraries(${PROJECT_NAME} PRIVATE
    nlohmann_json::nlohmann_json
    cpr::cpr)

target_compile_definitions(${PROJECT_NAME} PRIVATE AME_TANK_LIB_EXPORTS)