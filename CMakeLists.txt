cmake_minimum_required(VERSION 3.25)

project(VulkanEngine LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Runtime)

add_subdirectory(Engine)
add_subdirectory(Runtime)
add_subdirectory(Editor)





