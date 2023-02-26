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

#ifndef NABLA2D_SDLGLRENDERER_HPP
#define NABLA2D_SDLGLRENDERER_HPP

#include <string>
#include <memory>
#include <unordered_map>
#include <SDL2/SDL.h>

#include "../renderer.hpp"
#include "../OpenGL/glshader.hpp"

namespace nabla2d
{
    class SDLGLRenderer : public Renderer
    {
    public:
        SDLGLRenderer(const std::string &aTitle, const std::pair<int, int> &aSize);
        ~SDLGLRenderer() override;

        bool PollWindowEvents() override;
        void Clear() override;
        void Render() override;

        ShaderHandle LoadShader(const std::string &aVertexPath, const std::string &aFragmentPath) override;
        void DeleteShader(ShaderHandle aHandle) override;
        void UseShader(ShaderHandle aHandle) override;

    private:
        int mWidth;
        int mHeight;

        std::unordered_map<ShaderHandle, std::shared_ptr<GLShader>> mShaders;

        SDL_Window *mWindow;
        SDL_GLContext mGLContext;
    };
} // namespace nabla2d

#endif // NABLA2D_SDLGLRENDERER_HPP

// くコ:彡