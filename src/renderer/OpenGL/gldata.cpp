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

#include "gldata.hpp"

#include "glvertex.hpp"

namespace nabla2d
{
    GLData::GLData(const std::vector<float> &aVertices, GLenum aMode) : mSize(aVertices.size()),
                                                                        mEBO(0),
                                                                        mMode(aMode)
    {
        glGenVertexArrays(1, &mVAO);
        glGenBuffers(1, &mVBO);

        glBindVertexArray(mVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, mSize * sizeof(float), aVertices.data(), GL_STATIC_DRAW);

        GLsizei stride = GetStride(mMode);

        // Position (x, y, z)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * static_cast<GLsizei>(sizeof(float)), (void *)0);
        glEnableVertexAttribArray(0);

        if (mMode == GL_TRIANGLES)
        {
            // Texture coordinates (u, v)
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * static_cast<GLsizei>(sizeof(float)), (void *)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    GLData::GLData(const std::vector<float> &aVertices, const std::vector<unsigned int> &aIndices, GLenum aMode) : mSize(aVertices.size()),
                                                                                                                   mMode(aMode)
    {
        glGenVertexArrays(1, &mVAO);
        glGenBuffers(1, &mVBO);
        glGenBuffers(1, &mEBO);

        glBindVertexArray(mVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, mSize * sizeof(float), aVertices.data(), GL_STATIC_DRAW);

        GLsizei stride = GetStride(mMode);

        // Position (x, y, z)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * static_cast<GLsizei>(sizeof(float)), (void *)0);
        glEnableVertexAttribArray(0);

        if (mMode == GL_TRIANGLES)
        {
            // Texture coordinates (u, v)
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * static_cast<GLsizei>(sizeof(float)), (void *)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, aIndices.size() * sizeof(unsigned int), aIndices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    GLData::~GLData()
    {
        if (mVAO != 0)
        {
            glDeleteVertexArrays(1, &mVAO);
        }
        if (mVBO != 0)
        {
            glDeleteBuffers(1, &mVBO);
        }
        if (mEBO != 0)
        {
            glDeleteBuffers(1, &mEBO);
        }
    }

    std::size_t GLData::GetSize() const
    {
        return mSize;
    }

    GLuint GLData::GetVAO() const
    {
        return mVAO;
    }

    GLuint GLData::GetVBO() const
    {
        return mVBO;
    }

    GLuint GLData::GetEBO() const
    {
        return mEBO;
    }

    GLenum GLData::GetMode() const
    {
        return mMode;
    }

    GLsizei GLData::GetStride(GLenum aMode)
    {
        switch (aMode)
        {
        case GL_TRIANGLES:
            return 5;
        case GL_LINES:
        default:
            return 3;
        }
    }
} // namespace nabla2d

// くコ:彡