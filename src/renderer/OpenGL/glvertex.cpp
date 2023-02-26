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

#include "glvertex.hpp"

namespace nabla2d
{
    GLVertex::GLVertex(glm::vec3 aPosition, glm::vec2 aUV) : mData({aPosition.x,
                                                                    aPosition.y,
                                                                    aPosition.z,
                                                                    aUV.x,
                                                                    aUV.y})
    {
    }

    glm::vec3 GLVertex::GetPosition() const
    {
        return {mData[0], mData[1], mData[2]};
    }

    glm::vec2 GLVertex::GetUV() const
    {
        return {mData[3], mData[4]};
    }

    const std::array<float, GLVertex::ElementCount> &GLVertex::GetData() const
    {
        return mData;
    }
} // namespace nabla2d

// くコ:彡