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

        mTestSprite = std::shared_ptr<Sprite>(new Sprite(mRenderer.get(),
                                                         "assets/logo.png",
                                                         {1.0F, 1.0F},
                                                         Renderer::NEAREST));
        mTestTransform = Transform({0.5F, 0.5F, 0.0F}, {0.0F, 0.0F, 0.0F}, {1.0F, 1.0F, 1.0F});

        mEditor.Init(mRenderer.get());

        Logger::info("Game created");
    }

    Game::~Game()
    {
        mTestSprite->Clear(mRenderer.get());
        mRenderer->DeleteShader(mTestShader);

        mEditor.Destroy(mRenderer.get());

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

            mCamera.Update();

            mRenderer->Clear();

            static float totalDelta = 0.0F;
            totalDelta += mDeltaTime;

            // --------------- EDITOR ---------------
            mEditor.Update(mDeltaTime, totalDelta, mRenderer.get(), mCamera);
            mEditor.DrawGrid(mRenderer.get(), mCamera);

            // --------------- TEST SPRITE ---------------

            mRenderer->UseShader(mTestShader);
            mTestSprite->Draw(mRenderer.get(), mCamera, mTestTransform.GetMatrix());

            // --------------- EDITOR ---------------
            mEditor.DrawGUI(mRenderer.get(), mCamera);

            mRenderer->Render();
            Input::Update();
        }
        Logger::info("Game ended");
    }
} // namespace nabla2d

// くコ:彡