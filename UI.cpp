#include "UI.h"

bool windowHovered = false;
float inputDirLightAngle = lightDirTheta;

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
    
    //const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    //ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
    //ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
    if (show_ui) {
        if (!ImGui::Begin("Settings", &show_ui, 0))
        {
            ImGuiIO& io = ImGui::GetIO();
            windowHovered = io.WantCaptureMouse;
            ImGui::End();
        }
        else {
            ImGuiIO& io = ImGui::GetIO();
            windowHovered = io.WantCaptureMouse;
            Renderer* renderer = Renderer::getInstance();
            if (ImGui::CollapsingHeader("Render/Camera")) {
                const char* items[] = { "Default", "World position buffer", "Normal buffer", "Albedo", "Depth buffer",
                    "Specularity", "Shadows", "Cascade split depths", "Wireframe", "SSAO"};
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
                ImGui::Separator();
                ImGui::Text("Camera mode:");
                ImGui::Spacing();
                ImGui::RadioButton("1", &Renderer::cameraMode, 0); ImGui::SameLine();
                ImGui::RadioButton("2", &Renderer::cameraMode, 1);
                ImGui::Spacing();
                int numSSAOSamplesInput = Renderer::numSSAOSamples;
                float SSAOSampleRadiusInput = Renderer::SSAOSampleRadius;
                bool numSSAOSamplesChanged = false, SSAOSampleRadiusChanged = false;
                ImGui::DragInt("SSAO Samples:", &numSSAOSamplesInput, 1, 16, maxSamples);
                numSSAOSamplesChanged = ImGui::IsItemEdited();
                ImGui::DragFloat("SSAO radius:", &SSAOSampleRadiusInput, 0.01f, 0.05f, 2.5f);
                SSAOSampleRadiusChanged = ImGui::IsItemEdited();
                if (numSSAOSamplesChanged || SSAOSampleRadiusChanged) 
                    renderer->updateSSAOParameters(numSSAOSamplesInput, SSAOSampleRadiusInput);
                if (Renderer::render_mode == Renderer::RENDER_SSAO) {
                    ImGui::Checkbox("Show blur", &Renderer::showBlur);
                }
                if (Renderer::render_mode == Renderer::RENDER_WIREFRAME) {
                    ImGui::Checkbox("Draw bounding boxes", &Renderer::drawBoundingBoxes);
                }
            }
            if (ImGui::CollapsingHeader("Lights")) {
                static int lightNum = 0;

                //Toggling which point light
                // Use AlignTextToFramePadding() to align text baseline to the baseline of framed widgets elements
                // (otherwise a Text+SameLine+Button sequence will have the text a little too high by default!)
                // See 'Demo->Layout->Text Baseline Alignment' for details.
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Point light index:");
                ImGui::SameLine();

                // Arrow buttons with Repeater
                float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
                ImGui::PushButtonRepeat(true);
                if (ImGui::ArrowButton("##left", ImGuiDir_Left)) { lightNum--; }
                ImGui::SameLine(0.0f, spacing);
                if (ImGui::ArrowButton("##right", ImGuiDir_Right)) { lightNum++; }
                ImGui::PopButtonRepeat();
                ImGui::SameLine();
                if (lightNum >= Renderer::numPointLights) lightNum = Renderer::numPointLights - 1;
                if (lightNum < 0) lightNum = 0;
                ImGui::Text("%d", lightNum);
                if (Renderer::numPointLights > 0) {
                    bool inputLightPosChanged = false, inputLightColorChanged = false, inputLightParamChanged = false;
                    glm::vec4 inputLightPos = Renderer::lightPos.at(lightNum);
                    glm::vec4 inputLightColor = Renderer::lightColor.at(lightNum);
                    glm::vec4 inputLightParam = Renderer::lightParam.at(lightNum);
                    ImGui::DragFloat3("Light pos:", glm::value_ptr(inputLightPos), 0.01f, -100.0f, 100.0f);
                    inputLightPosChanged = ImGui::IsItemEdited();
                    ImGui::InputFloat4("Light param:", glm::value_ptr(inputLightParam));
                    inputLightParamChanged = ImGui::IsItemEdited();
                    ImGui::ColorEdit3("Light color:", glm::value_ptr(inputLightColor));
                    inputLightColorChanged = ImGui::IsItemEdited();
                    if (inputLightColorChanged || inputLightPosChanged || inputLightParamChanged)
                        renderer->updatePointLight(lightNum, &inputLightPos, &inputLightColor, &inputLightParam);
                    //ImGui::InputFloat3("Light pos:", vec4a);
                }
                ImGui::Spacing();
                if (Renderer::numPointLights < maxPointLights) {
                    if (ImGui::Button("Push point light")) {
                        renderer->pushPointLight();
                    }
                }
                if (Renderer::numPointLights > 0) {
                    ImGui::SameLine();
                    if (ImGui::Button("Pop point light")) {
                        renderer->popPointLight();
                    }
                }

                ImGui::Spacing();

                ImGui::AlignTextToFramePadding();
                ImGui::Text("Num shadowed point lights:");
                ImGui::SameLine();

                bool numShadowedLightsIncreased = false, numShadowedLightsDecreased = false;
                int numShadowedLightsInput = Renderer::numShadowedLights;
                // Arrow buttons with Repeater
                float spacing2 = ImGui::GetStyle().ItemInnerSpacing.x;
                if (ImGui::Button("<-")) {
                    numShadowedLightsInput--;
                    numShadowedLightsDecreased = true;
                }
                ImGui::SameLine(0.0f, spacing2);
                if (ImGui::Button("->")) {
                    numShadowedLightsInput++;
                    numShadowedLightsIncreased = true;
                }
                if (numShadowedLightsDecreased || numShadowedLightsIncreased) {
                    renderer->updateNumShadowedPointLights(numShadowedLightsInput);
                    //std::cout << "here\n";
                }
                ImGui::Text("%d", Renderer::numShadowedLights);

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                bool dirLightAngleChanged = false, dirLightColorChanged = false;
                ImGui::DragFloat("Dir light angle:", &inputDirLightAngle, 0.01, 0.0f, pi);
                glm::vec4 newLightDir = glm::vec4(cos(inputDirLightAngle) * cos(lightDirPhi),
                    sin(inputDirLightAngle),
                    -sin(lightDirPhi) * cos(inputDirLightAngle),
                    0.0f);
                dirLightAngleChanged = ImGui::IsItemEdited();
                glm::vec4 newLightDirColor = Renderer::lightDirColor;
                ImGui::ColorEdit3("Dir light color:", glm::value_ptr(newLightDirColor));
                dirLightColorChanged = ImGui::IsItemEdited();
                if (dirLightAngleChanged || dirLightColorChanged) renderer->updateDirectionalLight(&newLightDir, &newLightDirColor);
            }
            if (ImGui::CollapsingHeader("Scene")) {
                static int modelNum = 0;
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Model index:");
                ImGui::SameLine();

                // Arrow buttons with Repeater
                float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
                ImGui::PushButtonRepeat(true);
                if (ImGui::ArrowButton("##left", ImGuiDir_Left)) { modelNum--; }
                ImGui::SameLine(0.0f, spacing);
                if (ImGui::ArrowButton("##right", ImGuiDir_Right)) { modelNum++; }
                ImGui::PopButtonRepeat();
                if (modelNum >= Renderer::modelAssets.size()) modelNum = Renderer::modelAssets.size() - 1;
                if (modelNum < 0) modelNum = 0;

                ImGui::Text("%d", modelNum);

                if (!Renderer::modelAssets.empty()) {
                    size_t numIndices = 0;
                    for (int i = 0; i < Renderer::modelAssets.at(modelNum)->meshes.size(); i++) {
                        numIndices += Renderer::modelAssets.at(modelNum)->meshes.at(i).numIndices;
                    }
                    ImGui::SameLine();
                    ImGui::Text(" %s Indices: %u", Renderer::modelAssets.at(modelNum)->directory.c_str(), numIndices);
                    //ImGui::DragFloat3("Light pos:", glm::value_ptr(inputLightPos), 0.01f, -100.0f, 100.0f);
                    //ImGui::InputFloat3("input float3", vec4f);
                    bool modelAngleChanged = false;
                    bool modelScaleChanged = false;
                    bool modelPositionChanged = false;
                    Model* curModel = Renderer::modelAssets.at(modelNum);
                    glm::vec3 ypr(curModel->yaw, curModel->pitch, curModel->roll);
                    ImGui::DragFloat3("Pitch, roll, yaw", glm::value_ptr(ypr), 0.01f, 0.0f, 2*pi);
                    modelAngleChanged = ImGui::IsItemEdited();
                    curModel->pitch = ypr.y;
                    curModel->yaw = ypr.x;
                    curModel->roll = ypr.z;
                    glm::vec3 inputPosition = curModel->position;
                    //ImGui::DragFloat3("Position XYZ", glm::value_ptr(inputPosition), 0.1f, -100.0f, 100.0f);
                    ImGui::InputFloat3("Position XYZ", glm::value_ptr(inputPosition));
                    modelPositionChanged = ImGui::IsItemEdited();
                    curModel->position = glm::vec4(inputPosition, 1.0f);
                    glm::vec3 inputScale = curModel->scale;
                    ImGui::InputFloat3("Scale XYZ", glm::value_ptr(inputScale));
                    modelScaleChanged = ImGui::IsItemEdited();
                    curModel->scale = inputScale;
                    if (modelAngleChanged || modelScaleChanged || modelPositionChanged) renderer->updateStaticPointShadowMap();
                    if (ImGui::Button("Delete")) {
                        renderer->removeModel(modelNum);
                    }
                }
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                static bool importAsSingleMesh = false;
                static bool flipUVs = false;
                static std::string fileName;
                ImGui::InputText("Path to load model", &fileName);
                ImGui::Checkbox("Import as single mesh", &importAsSingleMesh);
                ImGui::SameLine();
                ImGui::Checkbox("Flip UVs", &flipUVs);
                ImGui::SameLine();
                if (ImGui::Button("Load")) {
                    renderer->addModel(fileName, importAsSingleMesh, flipUVs);
                }
            }
            //Adjust point light params and position
            //static float f0 = 0.001f;
            //ImGui::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");
            //static float vec4a[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
            //ImGui::InputFloat3("input float3", vec4a);
            //static float f1 = 1.00f, f2 = 0.0067f;
            //ImGui::DragFloat("drag float", &f1, 0.005f);
            //static float col1[3] = { 1.0f, 0.0f, 0.2f };
            //static float col2[4] = { 0.4f, 0.7f, 0.0f, 0.5f };
            //ImGui::ColorEdit3("color 1", col1);
            //ImGui::SameLine(); HelpMarker(
            //    "Click on the color square to open a color picker.\n"
             //   "Click and hold to use drag and drop.\n"
            //    "Right-click on the color square to show options.\n"
              //  "CTRL+click on individual component to input value.\n");

            //ImGui::ColorEdit4("color 2", col2);

            //ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0)
            //ImGui::ColorEdit3("MyColor##1", (float*)&color, misc_flags);

            //static float vec4f[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
            //static int vec4i[4] = { 1, 5, 100, 255 };

            //ImGui::InputFloat2("input float2", vec4f);
            //ImGui::DragFloat2("drag float2", vec4f, 0.01f, 0.0f, 1.0f);
            //ImGui::SliderFloat2("slider float2", vec4f, 0.0f, 1.0f);
            //ImGui::InputInt2("input int2", vec4i);
            //ImGui::DragInt2("drag int2", vec4i, 1, 0, 255);
            //ImGui::SliderInt2("slider int2", vec4i, 0, 255);
            //ImGui::Spacing();

            //ImGui::InputFloat3("input float3", vec4f);
            //ImGui::DragFloat3("drag float3", vec4f, 0.01f, 0.0f, 1.0f);
            //ImGui::SliderFloat3("slider float3", vec4f, 0.0f, 1.0f);
            //ImGui::InputInt3("input int3", vec4i);
            //ImGui::DragInt3("drag int3", vec4i, 1, 0, 255);
            //ImGui::SliderInt3("slider int3", vec4i, 0, 255);
            //ImGui::Spacing();

            //ImGui::InputFloat4("input float4", vec4f);
            //ImGui::DragFloat4("drag float4", vec4f, 0.01f, 0.0f, 1.0f);
            //ImGui::SliderFloat4("slider float4", vec4f, 0.0f, 1.0f);
            //ImGui::InputInt4("input int4", vec4i);
            //ImGui::DragInt4("drag int4", vec4i, 1, 0, 255);
            //ImGui::SliderInt4("slider int4", vec4i, 0, 255);
            //also add config for every object in scene
            //save settings to a file
            //have options to add new objects
            
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
