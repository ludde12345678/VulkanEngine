#include "Window.h"
#include "../App/appContext.h"
#include "../Renderer/Vulkan/VUtils.h"


void initImgui(AppContext &appCtx) {
  
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)

    ImGui_ImplGlfw_InitForVulkan(appCtx.window.window, true);
    ImGui_ImplVulkan_InitInfo info{};
    info.ApiVersion = VK_API_VERSION_1_4;              // Pass in your value of VkApplicationInfo::apiVersion, otherwise will default to header version.
    
    VulkanContext& vctx = appCtx.renderer.vulkanContext;
    info.Instance = vctx.instance;
    info.PhysicalDevice = vctx.physicalDevice;
    info.Device = vctx.device;
    info.QueueFamily = vctx.queueContext.graphicsQueueFamily;
    info.Queue = vctx.queueContext.graphicsQueue;
    info.PipelineCache = VK_NULL_HANDLE;
    info.DescriptorPool = appCtx.renderer.ImGuiDescPool;
    info.MinImageCount = 2;
    info.ImageCount = vctx.swapchainContext.imageCount;
    info.Allocator = nullptr;
    info.UseDynamicRendering = true;
    info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    info.CheckVkResultFn = [](VkResult err)
    {
        if (err != VK_SUCCESS)
            std::cerr << "ImGui Vulkan error: " << err << "\n";
    };

    VkPipelineRenderingCreateInfo pipelineRenderingCreateInfo{};
    pipelineRenderingCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;

    pipelineRenderingCreateInfo.colorAttachmentCount = 1;
    pipelineRenderingCreateInfo.pColorAttachmentFormats =
        &vctx.swapchainContext.imageFormat;

    pipelineRenderingCreateInfo.depthAttachmentFormat = vctx.swapchainContext.depthFormat;


    info.PipelineInfoMain.PipelineRenderingCreateInfo =
        pipelineRenderingCreateInfo;


    ImGui_ImplVulkan_Init(&info);


}
void imGuiUpdate(AppContext& AppCtx) {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    auto& shading = AppCtx.frame.Uniforms.shading;
    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

    if (ImGui::BeginTabBar("MainTabs")) {

        if (ImGui::BeginTabItem("Shading")) {

            ImGui::ColorPicker4("Diffuse Color", &shading.diffuseColor.x);
            ImGui::ColorPicker4("Specular Color", &shading.spectralColor.x);
            ImGui::ColorPicker4("Ambient Color", &shading.ambientColor.x);

            ImGui::DragFloat("Specular Power", &shading.spectralPower, 1.0f, 10.0f, 100.0f);

            ImGuiCheckbox("Toggle Diffuse", shading.toggleDiffuse);
            ImGuiCheckbox("Toggle Specular", shading.toggleSpecular);
            ImGuiCheckbox("Toggle Ambient", shading.toggleAmbient);


            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Camera")) {
            ImGui::DragFloat3(
                "Camera Psoition",
                &shading.camerapos.x,
                0.05f
            );
            ImGui::DragFloat3(
                "Light Position",
                &shading.lightpos.x,
                0.05f
            );
        
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Debug")) {
            ImGuiCheckbox("Display Normals", shading.debugNormals);


            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }





    ImGui::End();
    // Rendering
    ImGui::Render();

}
void ImGuiCheckbox(const char* label, uint32_t& value)
{
    bool temp = value != 0;

    if (ImGui::Checkbox(label, &temp))
        value = temp ? 1u : 0u;
}


void initWindow(AppContext& AppCtx) {
    glfwInit();
    if (!glfwInit())
    {
        std::cout << "GLFW failed\n";
        
    }

    // GLFW Error callback
    glfwSetErrorCallback(error_callback);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // create window
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
    GLFWwindow* window;
    if (GLFWConfig::FULLSCREEN) {
        window = glfwCreateWindow(2560, 1440, "LE window", glfwGetPrimaryMonitor(), NULL);
    }
    else
    {
        window = glfwCreateWindow((int)(2560 * main_scale), (int)(1440 * main_scale), "Vulkan Renderer", nullptr, nullptr);
    }
    if (!glfwVulkanSupported())
    {
        throw std::runtime_error("GLFW: Vulkan Not Supported\n");
    }




    glfwSetWindowUserPointer(window, &AppCtx);
    setupGLFWCallbacks(window);
    WindowContext ctx{};
    ctx.window = window;
    AppCtx.window = ctx;

}
void setupGLFWCallbacks(GLFWwindow* window) {

    glfwSetKeyCallback(window, GLFW_KeyCallback);
    glfwSetCursorPosCallback(window, GLFW_CursorPosCallback);
    glfwSetMouseButtonCallback(window, GLFW_MouseButtonCallback);
}
void pollWindow(GLFWwindow* window) {
    glfwPollEvents();
}