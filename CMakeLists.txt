cmake_minimum_required(VERSION 3.25)

project(VulkanEngine LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

add_executable(VulkanEngine "src/Engine/App/app.cpp"  "src/Engine/Window/Window.cpp" "src/Engine/Window/GLFWCallbacks.h" "src/Engine/Window/Window.h" "src/Engine/Renderer/Vulkan/VInstance.cpp" "src/Engine/Renderer/Vulkan/VInstance.h" "src/Engine/Window/GLFWCallbacks.cpp" "src/Engine/Renderer/Vulkan/VCallbacks.h" "src/main.cpp" "src/Engine/App/app.h" "src/Engine/Renderer/Renderer.cpp" "src/Engine/Renderer/Renderer.h" "src/Engine/Renderer/Vulkan/VDevice.h" "src/Engine/Renderer/Vulkan/VDevice.cpp" "src/Engine/Renderer/Vulkan/VSwapchain.h" "src/Engine/Renderer/Vulkan/VSwapchain.cpp" "src/Engine/Renderer/Vulkan/VContext.h" "src/Engine/Renderer/Vulkan/VCommands.cpp" "src/Engine/Renderer/Vulkan/VCommands.h" "src/Engine/Renderer/Vulkan/VSync.cpp" "src/Engine/Renderer/Vulkan/VSync.h" "src/Engine/Renderer/Vulkan/VConfig.h" "src/Engine/Renderer/Shaders.cpp" "src/Engine/Renderer/Shaders.h" "src/Engine/Window/WindowContext.h"   "src/Engine/Scene/VertexLayouts.h" "src/Engine/Renderer/Vulkan/MemoryHandler/memory.h" "src/Engine/Renderer/Vulkan/MemoryHandler/memory.cpp" "src/Engine/Renderer/Vulkan/MemoryHandler/MemStructs.h" "src/Engine/Renderer/Vulkan/VImage.cpp" "src/Engine/Renderer/Vulkan/VImage.h" "src/Engine/Renderer/Pipeline.cpp" "src/Engine/Renderer/Pipeline.h" "src/Engine/Renderer/MeshRenderer.cpp" "src/Engine/Renderer/MeshRenderer.h" "src/Engine/Renderer/Vulkan/VUtils.h" "src/Engine/Renderer/ResourceUploader.cpp" "src/Engine/Renderer/ResourceUploader.h" "src/Engine/Scene/Scene.h" "src/Engine/Scene/Object.h" "src/Engine/App/EngineData.h"   "src/Engine/Renderer/ShaderData.h" "src/Engine/Renderer/Descriptors.cpp" "src/Engine/Renderer/Desciptors.h" "src/Engine/Renderer/RendererContext.h" "src/Engine/AssetLoader/AssetLoader.cpp" "src/Engine/AssetLoader/AssetLoader.h" "src/Engine/Window/GLFWConfig.h" "src/Engine/AssetLoader/AssetConfig.h" "src/Engine/App/appConfig.h" "src/Engine/App/appContext.h" "src/Engine/Scene/SceneManager.h" "src/Engine/Scene/SceneManager.cpp" "src/Engine/App/camera.cpp" "src/Engine/App/camera.h" "src/Engine/App/input.cpp" "src/Engine/App/input.h")

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


target_link_libraries(VulkanEngine PRIVATE
glfw
glm::glm
volk::volk
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