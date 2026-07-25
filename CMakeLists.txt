cmake_minimum_required(VERSION 3.25)

project(VulkanEngine LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

add_executable(VulkanEngine "src/Engine/App/app.cpp"  "src/Engine/Window/Window.cpp" "src/Engine/Window/GLFWCallbacks.h" "src/Engine/Window/Window.h" "src/Engine/Renderer/Vulkan/VInstance.cpp" "src/Engine/Renderer/Vulkan/VInstance.h" "src/Engine/Window/GLFWCallbacks.cpp" "src/Engine/Renderer/Vulkan/VCallbacks.h" "src/main.cpp" "src/Engine/App/app.h" "src/Engine/Renderer/Renderer.cpp" "src/Engine/Renderer/Renderer.h" "src/Engine/Renderer/Vulkan/VDevice.h" "src/Engine/Renderer/Vulkan/VDevice.cpp" "src/Engine/Renderer/Vulkan/VSwapchain.h" "src/Engine/Renderer/Vulkan/VSwapchain.cpp" "src/Engine/Renderer/Vulkan/VContext.h" "src/Engine/Renderer/Vulkan/VCommands.cpp" "src/Engine/Renderer/Vulkan/VCommands.h" "src/Engine/Renderer/Vulkan/VSync.cpp" "src/Engine/Renderer/Vulkan/VSync.h" "src/Engine/Renderer/Vulkan/VConfig.h" "src/Engine/Renderer/Vulkan/shaders.cpp" "src/Engine/Renderer/Vulkan/shaders.h" "src/Engine/Window/GLFWContext.h")

# Find and link packages from vcpkg manifest
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

# Link the imported targets (name may vary; adjust if CMake reports a missing target)
target_link_libraries(VulkanEngine PRIVATE
glfw
glm::glm
Vulkan::Vulkan

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


add_custom_target(build_shaders DEPENDS ${SHADER_OUT_NAMES})

add_dependencies(${PROJECT_NAME} build_shaders)