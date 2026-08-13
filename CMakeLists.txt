cmake_minimum_required(VERSION 3.25)

project(VulkanEngine LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})


file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.h"
)

add_executable(VulkanEngine ${ENGINE_SOURCES})


target_include_directories(VulkanEngine PRIVATE
    ${CMAKE_SOURCE_DIR}/external/imgui
    ${CMAKE_SOURCE_DIR}/external/imgui/backends
)
target_sources(VulkanEngine PRIVATE
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/imgui_demo.cpp   # optional

    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_vulkan.cpp
)
target_compile_definitions(VulkanEngine PRIVATE
    IMGUI_IMPL_VULKAN_USE_VOLK
    IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    VK_NO_PROTOTYPES
    VK_USE_PLATFORM_WIN32_KHR
)




# Find and link packages from vcpkg manifest
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(volk CONFIG REQUIRED)
#find_package(imgui CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)


target_link_libraries(VulkanEngine PRIVATE
glfw
glm::glm
volk::volk
nlohmann_json::nlohmann_json
)

set(SHADERS_IN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/Shaders")
set(SHADERS_OUT_DIR "${CMAKE_BINARY_DIR}/Shaders")

message(STATUS "Current source dir: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Shader input dir: ${SHADERS_IN_DIR}")

file(GLOB SHADERS CONFIGURE_DEPENDS
    "${SHADERS_IN_DIR}/*.vert"
    "${SHADERS_IN_DIR}/*.frag"
)

message(STATUS "Found shaders:")
foreach(SHADER ${SHADERS})
    message(STATUS "  ${SHADER}")
endforeach()

file(MAKE_DIRECTORY ${SHADERS_OUT_DIR})
foreach(SHADER ${SHADERS})
	get_filename_component(SHADER_NAME ${SHADER} NAME)
	set(SHADER_OUT_NAME "${SHADERS_OUT_DIR}/${SHADER_NAME}.spv")
	list(APPEND SHADER_OUT_NAMES ${SHADER_OUT_NAME})
	add_custom_command(
		MAIN_DEPENDENCY ${SHADER}
		OUTPUT ${SHADER_OUT_NAME}
		COMMAND ${Vulkan_GLSLC_EXECUTABLE}  ${SHADER} "-o" ${SHADER_OUT_NAME}
		VERBATIM)
endforeach()

message(STATUS "Copying asset folder:")
if(EXISTS "${CMAKE_SOURCE_DIR}/Assets")
	message(STATUS "Asset folder found")
else()
	message(STATUS "Asset folder not found")
endif()
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Assets
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets
)


add_custom_target(build_shaders DEPENDS ${SHADER_OUT_NAMES})

add_dependencies(${PROJECT_NAME} build_shaders)