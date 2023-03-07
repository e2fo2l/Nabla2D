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

#include "gltexture.hpp"

#include <cstdint>
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "../../logger.hpp"

namespace nabla2d
{
    GLTexture::GLTexture(const std::string &aPath, GLTextureFilter aFilter)
    {
        stbi_set_flip_vertically_on_load(1);
        uint8_t *data = stbi_load(aPath.c_str(), &mWidth, &mHeight, &mChannels, 0);

        if (data == nullptr)
        {
            const char *error = stbi_failure_reason();
            Logger::error("Failed to load texture '{}': {}", aPath, error);
            throw std::runtime_error("Failed to load texture '" + aPath + "': " + error);
        }

        glGenTextures(1, &mTexture);
        glBindTexture(GL_TEXTURE_2D, mTexture);

        auto filter = static_cast<GLTextureFilter>(aFilter);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

        if (mChannels == 3)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else if (mChannels == 4)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            Logger::error("Failed to load texture '{}': Unsupported number of channels: {}", aPath, mChannels);
            throw std::runtime_error("Failed to load texture '" + aPath + "': Unsupported number of channels: " + std::to_string(mChannels));
        }

        if (filter == GLTextureFilter::LINEAR_MIPMAP_NEAREST ||
            filter == GLTextureFilter::LINEAR_MIPMAP_LINEAR ||
            filter == GLTextureFilter::NEAREST_MIPMAP_NEAREST ||
            filter == GLTextureFilter::NEAREST_MIPMAP_LINEAR)
        {
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    GLTexture::~GLTexture()
    {
        glDeleteTextures(1, &mTexture);
    }

    int GLTexture::GetWidth() const
    {
        return mWidth;
    }

    int GLTexture::GetHeight() const
    {
        return mHeight;
    }

    int GLTexture::GetChannels() const
    {
        return mChannels;
    }

    GLuint GLTexture::GetTexture() const
    {
        return mTexture;
    }
} // namespace nabla2d

// くコ:彡