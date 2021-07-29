#include "UI.h"

void UI::initialize()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& Imgui_io = ImGui::GetIO(); (void)Imgui_io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void UI::showDemoWindow()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow(NULL);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::renderUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
    if (show_ui) {
        if (!ImGui::Begin("Settings", &show_ui, 0))
        {
            ImGui::End();
        }
        else {
            const char* items[] = { "Default", "World position buffer", "Normal buffer", "Albedo", "Depth buffer",
                "Specularity", "Shadows", "Cascade split depths", "Wireframe"};
            ImGui::Combo("Display buffer", &Renderer::render_mode, items, IM_ARRAYSIZE(items));
            ImGui::Separator();
            ImGui::Text("Skybox to render:");
            ImGui::Spacing();
            ImGui::RadioButton("Default", &Renderer::skybox_mode, Renderer::SKYBOX_DEFAULT); ImGui::SameLine();
            ImGui::RadioButton("Use shadow map", &Renderer::skybox_mode, Renderer::SKYBOX_SHADOW_MAP);
            ImGui::Separator();
            ImGui::Text("Draw frustum outline:");
            ImGui::Spacing();
            ImGui::RadioButton("No frustum", &Renderer::frustum_outline_mode, Renderer::NO_FRUSTUM_OUTLINE); ImGui::SameLine();
            ImGui::RadioButton("View frustum", &Renderer::frustum_outline_mode, Renderer::VIEW_FRUSTUM_OUTLINE); ImGui::SameLine();
            ImGui::RadioButton("Cascade frustum", &Renderer::frustum_outline_mode, Renderer::CASCADE_FRUSTUM_OUTLINE);
            ImGui::End();
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    else {
        ImGui::EndFrame();
    }
}

bool UI::show_ui = true;
