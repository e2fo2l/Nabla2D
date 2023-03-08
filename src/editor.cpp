//    _  __     __   __     ___  ___
//   / |/ /__ _/ /  / /__ _|_  |/ _ |
//  /    / _ `/ _ \/ / _ `/ __// // /
// /_/|_/\_,_/_.__/_/\_,_/____/____/
//
// Copyright (C) 2023 - Efflam
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "editor.hpp"

#include <cmath>
#include <string>
#include <numeric>
#include <imgui.h>
#include "logger.hpp"

constexpr const char *kGridVertexShader{R"(
#version 330 core

uniform mat4 u_ModelViewProjectionMatrix;
layout (location = 0) in vec3 a_Position;

void main()
{
    gl_Position = u_ModelViewProjectionMatrix * vec4(a_Position, 1.0f);
}

)"};

constexpr const char *kGridFragmentShader{R"(
#version 330 core

uniform vec4 u_Color;
out vec4 FragColor;

void main()
{
    FragColor = u_Color;
}

)"};

namespace nabla2d
{
    void Editor::Init(Renderer *aRenderer)
    {
        mGridShader = aRenderer->LoadShader(kGridVertexShader, kGridFragmentShader);

        mGridData = aRenderer->LoadDataLines(GetGridVertices(1.0F, 50), GetGridIndices(50));
        mSubgridData = aRenderer->LoadDataLines(GetGridVertices(1.0F, 500), GetGridIndices(500));
        mAxisData = aRenderer->LoadDataLines({{0.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F}}, {0, 1});

        mGridTransform = Transform({0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F}, {50.0F, 50.0F, 50.0F});
        mSubgridTransform = Transform({0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F}, {50.0F, 50.0F, 50.0F});
        mAxisTransform = Transform({0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F}, {10000.0F, 10000.0F, 10000.0F});

        mFPSs.fill(0.0F);
    }

    void Editor::Destroy(Renderer *aRenderer)
    {
        aRenderer->DeleteData(mGridData);
        aRenderer->DeleteData(mSubgridData);
        aRenderer->DeleteData(mAxisData);
        aRenderer->DeleteShader(mGridShader);
    }

    void Editor::Update(float aDeltaTime, float aTime)
    {
        mDeltaTime = aDeltaTime;
        mTime = aTime;

        std::copy(mFPSs.begin() + 1, mFPSs.end(), mFPSs.begin());
        mFPSs.back() = 1.0F / mDeltaTime;
        mAverageFPS = std::accumulate(mFPSs.begin(), mFPSs.end(), 0.0F) / static_cast<float>(mFPSs.size());
    }

    void Editor::DrawGrid(Renderer *aRenderer, Camera &aCamera)
    {
        auto cameraPosition = aCamera.GetPosition();
        auto cameraRotation = aCamera.GetRotation();

        aRenderer->UseShader(mGridShader);
        auto drawParameters = Renderer::DrawParameters();
        drawParameters.lineWidth = 1.0F;

        const float scaleIndex = std::max(std::floor(std::log2(std::abs(cameraPosition.z) / 10.0F) + 1.0F), 1.0F);
        const float scaleFactor = std::pow(2.0F, scaleIndex);
        mSubgridTransform.SetScale({scaleFactor * 50.0F, scaleFactor * 50.0F, 1.0F});
        mGridTransform.SetScale({scaleFactor * 50.0F, scaleFactor * 50.0F, 1.0F});

        mSubgridTransform.SetPosition({std::floor(cameraPosition.x / scaleFactor) * scaleFactor, std::floor(cameraPosition.y / scaleFactor) * scaleFactor, 0.0F});
        mGridTransform.SetPosition({std::floor(cameraPosition.x / scaleFactor) * scaleFactor, std::floor(cameraPosition.y / scaleFactor) * scaleFactor, 0.0F});

        const float gridOffset = (cameraPosition.z > 0.0F) ? -0.005F / scaleFactor : 0.005F / scaleFactor;
        mSubgridTransform.Translate({0.0F, 0.0F, gridOffset});
        mGridTransform.Translate({0.0F, 0.0F, gridOffset});
        mAxisTransform.Translate({0.0F, 0.0F, gridOffset * 0.75F});

        // Subgrid
        drawParameters.color = {0.45F, 0.45F, 0.45F, 1.0F};
        aRenderer->DrawData(mSubgridData, aCamera, mSubgridTransform.GetMatrix(), drawParameters);

        // Grid
        drawParameters.color = {0.125F, 0.125F, 0.125F, 1.0F};
        aRenderer->DrawData(mGridData, aCamera, mGridTransform.GetMatrix(), drawParameters);

        // Axes
        drawParameters.color = {1.0F, 0.0F, 0.0F, 1.0F};
        aRenderer->DrawData(mAxisData, aCamera, mAxisTransform.GetMatrix(), drawParameters);
        mAxisTransform.Rotate(1.0F, {0.0F, 0.0F, 90.0F});
        drawParameters.color = {0.0F, 1.0F, 0.0F, 1.0F};
        aRenderer->DrawData(mAxisData, aCamera, mAxisTransform.GetMatrix(), drawParameters);
        mAxisTransform.Rotate(1.0F, {90.0F, 0.0F, 0.0F});

        if (cameraRotation.x != 0.0F || cameraRotation.y != 0.0F)
        {
            drawParameters.color = {0.0F, 0.0F, 1.0F, 1.0F};
            aRenderer->DrawData(mAxisData, aCamera, mAxisTransform.GetMatrix(), drawParameters);
        }

        mAxisTransform.SetRotation({0.0F, 0.0F, 0.0F});
        mSubgridTransform.Translate({0.0F, 0.0F, -gridOffset});
        mGridTransform.Translate({0.0F, 0.0F, -gridOffset});
        mAxisTransform.Translate({0.0F, 0.0F, -gridOffset * 0.75});
    }

    void Editor::DrawGUI(Renderer *aRenderer, Camera &aCamera)
    {
        GUIDrawFPSWindow(aRenderer);
        GUIDrawCameraWindow(aCamera);
    }

    void Editor::GUIVec3Widget(const std::string &aTitle, glm::vec3 &aVec, float aSpeed, const std::string &aXLabel, const std::string &aYLabel, const std::string &aZLabel)
    {
        ImGui::Text("%s", aTitle.c_str());
        ImGui::PushItemWidth((0.65f * ImGui::GetWindowSize().x) / 3.0f);
        ImGui::DragFloat(fmt::format("{}##{}X", aXLabel, aTitle).c_str(), &aVec.x, aSpeed);
        ImGui::SameLine();
        ImGui::DragFloat(fmt::format("{}##{}Y", aYLabel, aTitle).c_str(), &aVec.y, aSpeed);
        ImGui::SameLine();
        ImGui::DragFloat(fmt::format("{}##{}Z", aZLabel, aTitle).c_str(), &aVec.z, aSpeed);
        ImGui::PopItemWidth();
    }

    void Editor::GUIBeginCornerWindow(int aCorner)
    {
        // https://github.com/ocornut/imgui/blob/master/imgui_demo.cpp#L7310=
        const float PAD = 10.0f;
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImVec2 work_pos = viewport->WorkPos;
        ImVec2 work_size = viewport->WorkSize;
        ImVec2 window_pos, window_pos_pivot;
        window_pos.x = (aCorner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
        window_pos.y = (aCorner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
        window_pos_pivot.x = (aCorner & 1) ? 1.0f : 0.0f;
        window_pos_pivot.y = (aCorner & 2) ? 1.0f : 0.0f;
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                                        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                                        ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
        ImGui::SetNextWindowBgAlpha(0.6f);
        ImGui::Begin(fmt::format("##CornerWindow{}", aCorner).c_str(), nullptr, window_flags);
    }

    void Editor::GUIDrawFPSWindow(Renderer *aRenderer)
    {
        GUIBeginCornerWindow(0);
        ImGui::Text("Nabla2D");
        ImGui::Text("Efflam - 2023");
        ImGui::Text("Built on %s %s", __DATE__, __TIME__);
        ImGui::Text("Renderer: %s", aRenderer->GetRendererInfo().c_str());
        ImGui::Text("Resolution: %dx%d", aRenderer->GetWidth(), aRenderer->GetHeight());
        ImGui::Text("FPS: %d", static_cast<int>(std::round(mAverageFPS)));
        ImGui::PlotLines("", mFPSs.data(), mFPSs.size(), 0, nullptr, 0);
        ImGui::End();
    }

    void Editor::GUIDrawCameraWindow(Camera &aCamera)
    {
        ImGui::Begin("Camera");
        glm::vec3 cameraPos = aCamera.GetPosition();
        GUIVec3Widget("Position", cameraPos, 0.1F);
        if (cameraPos != aCamera.GetPosition())
        {
            aCamera.SetPosition(cameraPos);
        }
        glm::vec3 cameraRot = aCamera.GetRotation();
        GUIVec3Widget("Rotation", cameraRot, 0.1F);
        if (cameraRot != aCamera.GetRotation())
        {
            aCamera.SetRotation(cameraRot);
        }
        static glm::vec3 cameraTarget = {0.0F, 0.0F, 0.0F};
        GUIVec3Widget("Target", cameraTarget, 0.1F);
        if (ImGui::Button("Set target", ImVec2(-1, 0)))
        {
            aCamera.LookAt(cameraTarget);
        }
        ImGui::End();
    }

    void Editor::GUIDraw2D32Window()
    {
        ImGui::Begin("Editor Mode");

        ImGui::End();
    }

    std::vector<glm::vec3> Editor::GetGridVertices(float aSize, int aSlices)
    {
        std::vector<glm::vec3> vertices;
        vertices.reserve((aSlices + 1) * 4);

        for (int i = 0; i <= aSlices; i++)
        {
            const float x = (static_cast<float>(i) * aSize / static_cast<float>(aSlices)) - aSize / 2.0F;
            vertices.emplace_back(x, -aSize / 2.0F, 0.0F);
            vertices.emplace_back(x, aSize / 2.0F, 0.0F);
        }

        for (int i = 0; i <= aSlices; i++)
        {
            const float y = (static_cast<float>(i) * aSize / static_cast<float>(aSlices)) - aSize / 2.0F;
            vertices.emplace_back(-aSize / 2.0F, y, 0.0F);
            vertices.emplace_back(aSize / 2.0F, y, 0.0F);
        }

        return vertices;
    }

    std::vector<unsigned int> Editor::GetGridIndices(int aSlices)
    {
        std::vector<unsigned int> indices;
        indices.reserve((aSlices + 1) * 4);

        for (int i = 0; i <= aSlices; ++i)
        {
            indices.push_back(i * 2);
            indices.push_back(i * 2 + 1);
        }

        for (int i = 0; i <= aSlices; ++i)
        {
            indices.push_back((aSlices + 1) * 2 + i * 2);
            indices.push_back((aSlices + 1) * 2 + i * 2 + 1);
        }

        return indices;
    }

} // namespace nabla2d

// くコ:彡