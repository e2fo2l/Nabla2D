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

#include <chrono>
#include "logger.hpp"

namespace nabla2d
{
    Game::Game() : mDeltaTime(0.0F)
    {
        mRenderer = std::unique_ptr<Renderer>(Renderer::Create("Nabla2D", {1600, 900}));
        Logger::info("Game created");
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
            Logger::debug("DeltaTime: {}", mDeltaTime);

            mRenderer->Clear();
            mRenderer->Render();
        }
        Logger::info("Game ended");
    }
}

// くコ:彡