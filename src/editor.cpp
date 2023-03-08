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
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "input.hpp"
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

    void Editor::Update(float aDeltaTime, float aTime, Renderer *aRenderer, Camera &aCamera)
    {
        mDeltaTime = aDeltaTime;
        mTime = aTime;

        std::copy(mFPSs.begin() + 1, mFPSs.end(), mFPSs.begin());
        mFPSs.back() = 1.0F / mDeltaTime;
        mAverageFPS = std::accumulate(mFPSs.begin(), mFPSs.end(), 0.0F) / static_cast<float>(mFPSs.size());

        if (mIsTransitioningMode)
        {
            if (mTime - mTransitionStartTime >= mTransitionDuration)
            {
                mIsTransitioningMode = false;
                aCamera.SetPosition(mTransitionEnd.GetPosition());
                aCamera.SetRotation(mTransitionEnd.GetRotation());
            }
            else
            {
                const float t = (mTime - mTransitionStartTime) / mTransitionDuration;
                Transform lerp = Transform::Lerp(mTransitionStart, mTransitionEnd, t);
                aCamera.SetPosition(lerp.GetPosition());
                aCamera.SetRotation(lerp.GetRotation());
            }
        }
        else
        {
            float movementSpeed = 3.0F;
            if (Input::KeyHeld(Input::KEY_LSHIFT))
            {
                movementSpeed *= 2.0F;
            }

            if (!mIs3Dmode) // 2D MODE
            {
                aCamera.SetRotation({0.0F, 0.0F, aCamera.GetRotation().z});

                // --------- CONTROLS ---------
                auto cameraPos = aCamera.GetPosition();
                float scroll = Input::GetMouseScroll();
                if (scroll != 0.0F)
                {
                    if ((cameraPos.z > 2.0F || scroll < 0.0F) &&
                        (cameraPos.z < 80.0F || scroll > 0.0F))
                    {
                        aCamera.Translate({0.0F, 0.0F, -scroll * 0.6F});
                    }
                }

                if (Input::KeyDown(Input::KEY_MOUSE1))
                {
                    aRenderer->SetMouseCapture(true);
                }
                if (Input::KeyHeld(Input::KEY_MOUSE1))
                {
                    glm::vec2 deltaPos = Input::GetMouseDelta();
                    aCamera.Translate({deltaPos.x * 8.0F, -deltaPos.y * 8.0F / (16.0F / 9.0F), 0.0F});
                }
                if (Input::KeyUp(Input::KEY_MOUSE1))
                {
                    aRenderer->SetMouseCapture(false);
                }

                auto axis = Input::GetAxis(Input::AXIS_LEFT);
                if (axis.x != 0.0F || axis.y != 0.0F)
                {
                    glm::vec3 cameraTranslation = {axis.x, axis.y, 0.0F};
                    aCamera.Translate(cameraTranslation * mDeltaTime * -movementSpeed);
                }
            }
            else // 3D MODE
            {
                // --------- CONTROLS ---------

                // auto cameraPos = aCamera.GetPosition();
                // auto cameraRot = aCamera.GetRotation();

                if (Input::KeyDown(Input::KEY_MOUSE1))
                {
                    aRenderer->SetMouseCapture(true);
                }
                if (Input::KeyHeld(Input::KEY_MOUSE1))
                {
                    glm::vec2 deltaPos = Input::GetMouseDelta();
                    glm::vec3 cameraRot = aCamera.GetRotation();

                    cameraRot.x -= deltaPos.y * 100.0F;
                    cameraRot.y = 0.0F;
                    cameraRot.z -= deltaPos.x * 100.0F;

                    cameraRot.x = glm::clamp(cameraRot.x, 0.1F, 179.9F);

                    aCamera.SetRotation(cameraRot);
                }
                if (Input::KeyUp(Input::KEY_MOUSE1))
                {
                    aRenderer->SetMouseCapture(false);
                }

                auto axis = Input::GetAxis(Input::AXIS_LEFT);
                if (axis.x != 0.0F || axis.y != 0.0F)
                {
                    auto cameraForward = aCamera.GetForward();
                    auto cameraRight = aCamera.GetRight();

                    glm::vec3 cameraTranslation = cameraForward * axis.y + cameraRight * axis.x;
                    aCamera.Translate(cameraTranslation * mDeltaTime * -movementSpeed);
                }
            }
        }
    }

    void Editor::DrawGrid(Renderer *aRenderer, Camera &aCamera)
    {
        auto cameraPosition = aCamera.GetPosition();
        auto cameraRotation = aCamera.GetRotation();

        aRenderer->UseShader(mGridShader);
        auto drawParameters = Renderer::DrawParameters();
        drawParameters.lineWidth = 1.0F;

        const float scaleIndex = std::max(std::floor(std::log2(std::abs(cameraPosition.z) / 10.0F) + 1.0F), 1.0F);
        const float scaleFactor = std::pow(2.0F, scaleIndex - 1.0F);
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
        mAxisTransform.Rotate(90.0F, {0.0F, 0.0F, 1.0F});
        drawParameters.color = {0.0F, 1.0F, 0.0F, 1.0F};
        aRenderer->DrawData(mAxisData, aCamera, mAxisTransform.GetMatrix(), drawParameters);
        mAxisTransform.SetRotation({0.0F, -90.0F, 0.0F});

        if (cameraRotation.x != 0.0F || cameraRotation.y != 0.0F)
        {
            float alpha = 1.0F;

            if (mIsTransitioningMode) // Fade in/out for Z axis
            {
                float timer = std::min((mTime - mTransitionStartTime) / mTransitionDuration, 1.0F);
                if (mIs3Dmode)
                {
                    alpha = timer;
                }
                else
                {
                    alpha = 1.0F - timer;
                }
            }

            drawParameters.color = {0.0F, 0.0F, 1.0F, alpha};
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
        GUIDraw2D32Window(aCamera);
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
            if (!mIs3Dmode && cameraPos.z < 1.0F)
            {
                cameraPos.z = 1.0F;
            }
            aCamera.SetPosition(cameraPos);
        }
        if (mIs3Dmode)
        {
            glm::vec3 cameraRot = aCamera.GetRotation();
            GUIVec3Widget("Rotation", cameraRot, 0.1F);
            if (cameraRot != aCamera.GetRotation())
            {
                aCamera.SetRotation(cameraRot);
            }
            GUIVec3Widget("Target", mCameraTarget, 0.1F);
            if (ImGui::Button("Set target", ImVec2(-1, 0)))
            {
                aCamera.LookAt(mCameraTarget);
            }
        }
        else
        {
            ImGui::Text("Rotation");
            float rotation = aCamera.GetRotation().z;
            ImGui::DragFloat("z##RotationZ", &rotation, 0.1F);
            if (rotation != aCamera.GetRotation().z)
            {
                aCamera.SetRotation({0.0F, 0.0F, rotation});
            }
        }
        ImGui::End();
    }

    void Editor::GUIDraw2D32Window(Camera &aCamera)
    {
        GUIBeginCornerWindow(1);
        ImGui::Text("Editor Mode");
        float windowWidth = 150.0F;
        float windowHeight = 30.0F;
        if (ImGui::Button("2D", ImVec2(windowWidth * 0.45F, windowHeight)) && mIs3Dmode && !mIsTransitioningMode)
        {
            mIs3Dmode = false;
            mIsTransitioningMode = true;
            mTransitionStartTime = mTime;
            mTransitionDuration = 1.0F;

            auto cameraPos = aCamera.GetPosition();
            auto cameraRot = aCamera.GetRotation();

            mTransitionStart = Transform(cameraPos, cameraRot);
            auto transitionEndPos = cameraPos + glm::vec3{-2.0F, 2.0F, 0.0F};
            transitionEndPos.z = 5.0F;
            mTransitionEnd = Transform(transitionEndPos, {0.0F, 0.0F, 0.0F});
        }
        ImGui::SameLine();
        if (ImGui::Button("3D", ImVec2(windowWidth * 0.45F, windowHeight)) && !mIs3Dmode && !mIsTransitioningMode)
        {
            mIs3Dmode = true;
            mIsTransitioningMode = true;
            mTransitionStartTime = mTime;
            mTransitionDuration = 1.0F;

            auto cameraPos = aCamera.GetPosition();
            auto cameraRot = aCamera.GetRotation();

            mTransitionStart = Transform(cameraPos, cameraRot);
            mTransitionEnd = Transform(cameraPos - glm::vec3{-2.0F, 2.0F, 2.0F});
            mTransitionEnd.LookAt({cameraPos.x, cameraPos.y, 0.0F}, {0.0F, 0.0F, 1.0F});
        }
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