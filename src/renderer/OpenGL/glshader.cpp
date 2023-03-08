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

#include "glshader.hpp"

#include <array>
#include <cstdint>
#include <stdexcept>
#include <GL/glew.h>

#include "../../logger.hpp"

constexpr uint32_t MAX_LOG_LENGTH = 1024U;

namespace nabla2d
{

    GLShader::GLShader(const std::string &aVertexShader, const std::string &aFragmentShader)
    {
        const GLuint mVertexShader = glCreateShader(GL_VERTEX_SHADER);
        const GLuint mFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        const char *vertexShaderSource = aVertexShader.c_str();
        const char *fragmentShaderSource = aFragmentShader.c_str();

        glShaderSource(mVertexShader, 1, &vertexShaderSource, nullptr);
        glShaderSource(mFragmentShader, 1, &fragmentShaderSource, nullptr);

        // Compile the shaders and check for errors
        glCompileShader(mVertexShader);
        glCompileShader(mFragmentShader);

        GLint success = 0;
        std::array<GLchar, MAX_LOG_LENGTH> infoLog{};

        glGetShaderiv(mVertexShader, GL_COMPILE_STATUS, &success);
        glGetShaderInfoLog(mVertexShader, infoLog.size(), nullptr, infoLog.data());
        if (success == 0)
        {
            Logger::error("Vertex shader compilation failed: {}", infoLog.data());
            throw std::runtime_error("Vertex shader compilation failed");
        }
        if (infoLog[0] != '\0')
        {
            Logger::warn("Vertex shader compilation warning: {}", infoLog.data());
        }

        glGetShaderiv(mFragmentShader, GL_COMPILE_STATUS, &success);
        glGetShaderInfoLog(mFragmentShader, infoLog.size(), nullptr, infoLog.data());
        if (success == 0)
        {
            Logger::error("Fragment shader compilation failed: {}", infoLog.data());
            throw std::runtime_error("Fragment shader compilation failed");
        }
        if (infoLog[0] != '\0')
        {
            Logger::warn("Fragment shader compilation warning: {}", infoLog.data());
        }

        mProgram = glCreateProgram();
        glAttachShader(mProgram, mVertexShader);
        glAttachShader(mProgram, mFragmentShader);
        glLinkProgram(mProgram);

        glGetProgramiv(mProgram, GL_LINK_STATUS, &success);
        glGetProgramInfoLog(mProgram, infoLog.size(), nullptr, infoLog.data());
        if (success == 0)
        {
            Logger::error("Shader program linking failed: {}", infoLog.data());
            throw std::runtime_error("Shader program linking failed");
        }
        if (infoLog[0] != '\0')
        {
            Logger::warn("Shader program linking warning: {}", infoLog.data());
        }

        glDeleteShader(mVertexShader);
        glDeleteShader(mFragmentShader);

        mModelViewProjectionLocation = glGetUniformLocation(mProgram, "u_ModelViewProjectionMatrix");
        mTextureLocation = glGetUniformLocation(mProgram, "u_Texture");
        mAtlasInfoLocation = glGetUniformLocation(mProgram, "u_AtlasInfo");
        mColorLocation = glGetUniformLocation(mProgram, "u_Color");
    }

    GLShader::~GLShader()
    {
        glDeleteProgram(mProgram);
    }

    GLuint GLShader::GetProgram() const
    {
        return mProgram;
    }

    GLint GLShader::GetModelViewProjectionLocation() const
    {
        return mModelViewProjectionLocation;
    }

    GLint GLShader::GetTextureLocation() const
    {
        return mTextureLocation;
    }

    GLint GLShader::GetAtlasInfoLocation() const
    {
        return mAtlasInfoLocation;
    }

    GLint GLShader::GetColorLocation() const
    {
        return mColorLocation;
    }

} // namespace nabla2d

// くコ:彡