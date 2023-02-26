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

#ifndef NABLA2D_GLDATA_HPP
#define NABLA2D_GLDATA_HPP

#include <vector>
#include <GL/glew.h>

namespace nabla2d
{
    class GLData
    {
    public:
        explicit GLData(const std::vector<float> &aVertices);
        ~GLData();

        std::size_t GetSize() const;
        GLuint GetVAO() const;
        GLuint GetVBO() const;

    private:
        std::size_t mSize{0};
        GLuint mVAO{0};
        GLuint mVBO{0};
    };

} // namespace nabla2d

#endif // NABLA2D_GLDATA_HPP

// くコ:彡