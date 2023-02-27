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

#ifndef NABLA2D_RENDERER_HPP
#define NABLA2D_RENDERER_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <glm/glm.hpp>

#include "../camera.hpp"
namespace nabla2d
{
    // Abstract class for rendering
    class Renderer
    {
    public:
        typedef uint64_t DataHandle;
        typedef uint64_t ShaderHandle;
        typedef uint64_t TextureHandle;

        virtual ~Renderer() = default;

        static Renderer *Create(const std::string &aTitle, const std::pair<int, int> &aSize);

        virtual bool PollWindowEvents() = 0;
        virtual void Clear() = 0;
        virtual void Render() = 0;

        virtual DataHandle LoadData(const std::vector<std::pair<glm::vec3, glm::vec2>> &aData) = 0;
        virtual void DeleteData(DataHandle aHandle) = 0;
        virtual void DrawData(DataHandle aHandle, const Camera &aCamera, const glm::mat4 &aTransform) = 0;

        virtual ShaderHandle LoadShader(const std::string &aVertexPath, const std::string &aFragmentPath) = 0;
        virtual void DeleteShader(ShaderHandle aHandle) = 0;
        virtual void UseShader(ShaderHandle aHandle) = 0;

        // virtual TextureHandle LoadTexture(const std::string &aPath) = 0;
        // virtual void DeleteTexture(TextureHandle aHandle) = 0;
    };
} // namespace nabla2d

#endif // NABLA2D_RENDERER_HPP

// くコ:彡