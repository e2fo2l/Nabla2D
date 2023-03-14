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

#include <algorithm>
#include "glvertex.hpp"

namespace nabla2d
{
    GLData::GLData(const std::vector<float> &aVertices,
                   const std::vector<unsigned int> &aIndices,
                   GLenum aMode,
                   GLenum aDrawUsage) : mMode(aMode),
                                        mDrawUsage(aDrawUsage)
    {
        glGenVertexArrays(1, &mVAO);

        mSize = std::max(aVertices.size(), aIndices.size());
        mLargestSize = aDrawUsage != GL_STATIC_DRAW ? mSize * 2 : mSize;

        glBindVertexArray(mVAO);
        glGenBuffers(1, &mVBO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        if (mDrawUsage != GL_STATIC_DRAW)
        {
            glBufferData(GL_ARRAY_BUFFER, mLargestSize * sizeof(float), nullptr, mDrawUsage);
            glBufferSubData(GL_ARRAY_BUFFER, 0, mSize * sizeof(float), aVertices.data());
        }
        else
        {
            glBufferData(GL_ARRAY_BUFFER, aVertices.size() * sizeof(float), aVertices.data(), mDrawUsage);
        }

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

        if (!aIndices.empty())
        {
            glGenBuffers(1, &mEBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
            if (mDrawUsage != GL_STATIC_DRAW)
            {
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, mLargestSize * sizeof(unsigned int), nullptr, mDrawUsage);
                glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, mSize * sizeof(unsigned int), aIndices.data());
            }
            else
            {
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, aIndices.size() * sizeof(unsigned int), aIndices.data(), mDrawUsage);
            }
        }
        else
        {
            mEBO = 0;
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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

    void GLData::ChangeData(const std::vector<float> &aVertices, const std::vector<unsigned int> &aIndices)
    {
        if (mVAO == 0 || mVBO == 0)
        {
            return;
        }

        glBindVertexArray(mVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);

        size_t nextSize = mSize;
        size_t nextLargestSize = mLargestSize;

        if (aVertices.size() > mLargestSize)
        {
            nextSize = std::max(aVertices.size(), aIndices.size());
            nextLargestSize = nextSize * 2;
            glBufferData(GL_ARRAY_BUFFER, nextLargestSize * sizeof(float), nullptr, mDrawUsage);
            glBufferSubData(GL_ARRAY_BUFFER, 0, aVertices.size() * sizeof(float), aVertices.data());
        }
        else
        {
            glBufferSubData(GL_ARRAY_BUFFER, 0, aVertices.size() * sizeof(float), aVertices.data());
        }

        if (!aIndices.empty())
        {
            bool hadNoEBO = mEBO == 0;
            if (hadNoEBO)
            {
                glGenBuffers(1, &mEBO);
            }
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);

            if (hadNoEBO || aIndices.size() > mLargestSize)
            {
                nextSize = std::max(aVertices.size(), aIndices.size());
                nextLargestSize = nextSize * 2;
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, nextLargestSize * sizeof(unsigned int), nullptr, mDrawUsage);
                glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, aIndices.size() * sizeof(unsigned int), aIndices.data());
            }
            else
            {
                glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, aIndices.size() * sizeof(unsigned int), aIndices.data());
            }
        }
        else
        {
            if (mEBO != 0)
            {
                glDeleteBuffers(1, &mEBO);
                mEBO = 0;
            }
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        mSize = nextSize;
        mLargestSize = nextLargestSize;

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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

    GLenum GLData::GetDrawUsage() const
    {
        return mDrawUsage;
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