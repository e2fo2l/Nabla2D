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

#ifndef NABLA2D_GLVERTEX_HPP
#define NABLA2D_GLVERTEX_HPP

#include <array>
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace nabla2d
{
    class GLVertex
    {
    public:
        GLVertex(glm::vec3 aPosition, glm::vec2 aUV);
        ~GLVertex() = default;

        glm::vec3 GetPosition() const;
        glm::vec2 GetUV() const;
        const std::array<float, 5> &GetData() const;

    private:
        std::array<float, 5> mData{};
    };
} // namespace nabla2d

#endif // NABLA2D_GLVERTEX_HPP

// くコ:彡