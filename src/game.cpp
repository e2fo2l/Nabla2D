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

#include "game.hpp"

#include <cmath>
#include <chrono>
#include <numeric>
#include <imgui.h>
#include <fmt/format.h>
#include <glm/glm.hpp>

#include "logger.hpp"
#include "input.hpp"

namespace nabla2d
{
    Game::Game()
    {
        mCamera = Camera({0.0F, 0.0F, 5.0F}, {0.0F, 0.0F, 0.0F}, {45.0F, 16.0F / 9.0F, 0.1F, 100.0F});
        mRenderer = std::shared_ptr<Renderer>(Renderer::Create("Nabla2D", {1600, 900}));

        mFPSs.fill(0.0F);

        mTestShader = mRenderer->LoadShader(R"(
        #version 330 core
        uniform mat4 u_ModelViewProjectionMatrix;
        layout (location = 0) in vec3 a_Position;
        layout (location = 1) in vec2 a_TexCoord;
        out vec2 v_TexCoord;
        void main()
        {
            gl_Position = u_ModelViewProjectionMatrix * vec4(a_Position, 1.0);
            v_TexCoord = a_TexCoord;
        }
        )",
                                            R"(
        #version 330 core
        uniform sampler2D u_Texture;
        uniform vec4 u_AtlasInfo;
        in vec2 v_TexCoord;
        out vec4 FragColor;
        void main()
        {
            vec2 texCoord = v_TexCoord * u_AtlasInfo.zw + u_AtlasInfo.xy;
            texCoord.y = 1.0 - texCoord.y;
            FragColor = texture(u_Texture, texCoord);
        }
        )");

        mGridTexture = mRenderer->LoadTexture("assets/grid.png", Renderer::LINEAR_MIPMAP_LINEAR);
        mGridData = mRenderer->LoadData({{{-0.5F, -0.5F, 0.0F}, {0.0, 40.0}},
                                         {{-0.5F, 0.5F, 0.0F}, {0.0, 0.0}},
                                         {{0.5F, -0.5F, 0.0F}, {40.0, 40.0}},
                                         {{-0.5F, 0.5F, 0.0F}, {0.0, 0.0}},
                                         {{0.5F, 0.5F, 0.0F}, {40.0, 0.0}},
                                         {{0.5F, -0.5F, 0.0F}, {40.0, 40.0}}});
        mGridTransform = Transform({0.0F, 0.0F, -0.01F}, {0.0F, 0.0F, 0.0F}, {40.0F, 40.0F, 40.0F});

        mTestSprite = std::shared_ptr<Sprite>(new Sprite(mRenderer.get(),
                                                         "assets/logo.png",
                                                         {1.0F, 1.0F},
                                                         Renderer::NEAREST));
        mTestTransform = Transform({0.5F, 0.5F, 0.0F}, {0.0F, 0.0F, 0.0F}, {1.0F, 1.0F, 1.0F});

        Logger::info("Game created");
    }

    Game::~Game()
    {
        mRenderer->DeleteData(mGridData);
        mRenderer->DeleteTexture(mGridTexture);
        mTestSprite->Clear(mRenderer.get());
        mRenderer->DeleteShader(mTestShader);
        Logger::info("Game destroyed");
    }

    float Game::GetDeltaTime() const
    {
        return mDeltaTime;
    }

    void Game::Run()
    {
        Logger::info("Game started");
        Input::Init();
        auto lastTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        while (mRenderer->PollWindowEvents())
        {
            currentTime = std::chrono::high_resolution_clock::now();
            mDeltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(currentTime - lastTime).count();
            lastTime = currentTime;

            std::copy(mFPSs.begin() + 1, mFPSs.end(), mFPSs.begin());
            mFPSs.back() = 1.0F / mDeltaTime;
            mAverageFPS = std::accumulate(mFPSs.begin(), mFPSs.end(), 0.0F) / static_cast<float>(mFPSs.size());

            mCamera.Update();

            mRenderer->Clear();

            static float totalDelta = 0.0F;
            totalDelta += mDeltaTime;

            float scroll = Input::GetMouseScroll();
            if (scroll != 0.0F)
            {
                if ((mCamera.GetPosition().z > 2.0F || scroll < 0.0F) &&
                    (mCamera.GetPosition().z < 80.0F || scroll > 0.0F))
                {
                    mCamera.Translate({0.0F, 0.0F, -scroll * 0.6F});
                }
            }

            if (Input::KeyDown(Input::KEY_MOUSE1))
            {
                mRenderer->SetMouseCapture(true);
            }
            if (Input::KeyHeld(Input::KEY_MOUSE1))
            {
                glm::vec2 deltaPos = Input::GetMouseDelta();
                mCamera.Translate({deltaPos.x * 8.0F, -deltaPos.y * 8.0F / (16.0F / 9.0F), 0.0F});
            }
            if (Input::KeyUp(Input::KEY_MOUSE1))
            {
                mRenderer->SetMouseCapture(false);
            }

            // --------------- GRID ---------------

            auto cameraPos = mCamera.GetPosition();
            float gridScaleIndex = std::floor(cameraPos.z / 20.0F) + 1;
            mGridTransform.SetScale({40.0F * gridScaleIndex, 40.0F * gridScaleIndex, 1.0F});
            mGridTransform.SetPosition({std::floor(cameraPos.x / gridScaleIndex) * gridScaleIndex,
                                        std::floor(cameraPos.y / gridScaleIndex) * gridScaleIndex, -0.01F});

            mRenderer->UseShader(mTestShader);
            mRenderer->UseTexture(mGridTexture);
            mRenderer->DrawData(mGridData, mCamera, mGridTransform.GetMatrix(), {0.0F, 0.0F, 1.0F, 1.0F});

            // --------------- TEST SPRITE ---------------

            static Transform srcTransform = mTestTransform;
            static Transform dstTransform({-0.5F, 2.0F, 2.0F}, {45.0F, 150.0F, 80.0F}, {2.0F, 0.75F, 1.0F});
            mTestTransform = Transform::Lerp(srcTransform, dstTransform, std::min(totalDelta / 10.0F, 1.0F));
            mRenderer->UseShader(mTestShader);
            mTestSprite->Draw(mRenderer.get(), mCamera, mTestTransform.GetMatrix());

            // --------------- EDITOR ---------------
            DrawEditorWindows();

            mRenderer->Render();
            Input::Update();
        }
        Logger::info("Game ended");
    }

    // TODO : Move these methods to a separate Editor class

    void ImGuiBeginCornerWindow(int aCorner = 0)
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
        ImGui::Begin(
            fmt::format("##CornerWindow{}", aCorner).c_str(),
            nullptr, window_flags);
    }

    void ImGuiVec3(const std::string &aTitle,
                   glm::vec3 &aVec,
                   float aSpeed = 0.02F,
                   const std::string &aXLabel = "x",
                   const std::string &aYLabel = "y",
                   const std::string &aZLabel = "z")
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

    void Game::DrawEditorWindows()
    {
        // --------------- General Info ---------------
        ImGuiBeginCornerWindow(0);
        ImGui::Text("Nabla2D");
        ImGui::Text("Efflam - 2023");
        ImGui::Text("Built on %s %s", __DATE__, __TIME__);
        ImGui::Text("Renderer: %s", mRenderer->GetRendererInfo().c_str());
        ImGui::Text("Resolution: %dx%d", mRenderer->GetWidth(), mRenderer->GetHeight());
        ImGui::Text("FPS: %d", static_cast<int>(std::round(mAverageFPS)));
        ImGui::PlotLines("", mFPSs.data(), mFPSs.size(), 0, nullptr, 0);
        ImGui::End();

        // --------------- Camera Info ---------------
        ImGui::Begin("Camera");
        glm::vec3 cameraPos = mCamera.GetPosition();
        ImGuiVec3("Position", cameraPos, 0.1F);
        if (cameraPos != mCamera.GetPosition())
        {
            mCamera.SetPosition(cameraPos);
        }
        glm::vec3 cameraRot = mCamera.GetRotation();
        ImGuiVec3("Rotation", cameraRot, 0.1F);
        if (cameraRot != mCamera.GetRotation())
        {
            mCamera.SetRotation(cameraRot);
        }
        static glm::vec3 cameraTarget = {0.0F, 0.0F, 0.0F};
        ImGuiVec3("Target", cameraTarget, 0.1F);
        if (ImGui::Button("Set target", ImVec2(-1, 0)))
        {
            mCamera.LookAt(cameraTarget);
        }
        ImGui::End();
    }
} // namespace nabla2d

// くコ:彡