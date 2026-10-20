cmake_minimum_required(VERSION 3.16)
project(PE LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED Audio)

add_executable(PE src/main.cpp)

target_link_libraries(PE PRIVATE SFML::Graphics SFML::Window SFML::System SFML::Audio)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/assets/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets)
