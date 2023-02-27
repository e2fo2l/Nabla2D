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
#include <glm/glm.hpp>
#include "logger.hpp"

namespace nabla2d
{
    Game::Game()
    {
        mCamera = Camera({0.0F, 0.0F, 5.0F});
        mRenderer = std::unique_ptr<Renderer>(Renderer::Create("Nabla2D", {1600, 900}));

        mTestTriangle = mRenderer->LoadData({{{0.5F, 0.5F, 0.0F}, {0.0, 0.0}},
                                             {{-0.5F, 0.5F, 0.0F}, {0.0, 0.0}},
                                             {{0.0F, -0.5F, 0.0F}, {0.0, 0.0}}});

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
        in vec2 v_TexCoord;
        out vec4 FragColor;
        void main()
        {
            FragColor = texture(u_Texture, v_TexCoord);
        }
        )");

        Logger::info("Game created");
    }

    Game::~Game()
    {
        mRenderer->DeleteData(mTestTriangle);
        Logger::info("Game destroyed");
    }

    float Game::GetDeltaTime() const
    {
        return mDeltaTime;
    }

    void Game::Run()
    {
        Logger::info("Game started");
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
            mCamera.SetPosition({std::cos(totalDelta), std::sin(totalDelta), 5.0F});

            mRenderer->UseShader(mTestShader);
            mRenderer->DrawData(mTestTriangle, mCamera, glm::mat4(1.0F));

            mRenderer->Render();
        }
        Logger::info("Game ended");
    }
}

// くコ:彡