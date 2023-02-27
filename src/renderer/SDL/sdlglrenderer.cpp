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

#include "sdlglrenderer.hpp"

#include <stdexcept>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../logger.hpp"
#include "../renderer.hpp"

namespace nabla2d
{

    SDLGLRenderer::SDLGLRenderer(const std::string &aTitle, const std::pair<int, int> &aSize) : mWidth(aSize.first), mHeight(aSize.second), mCurrentShader(nullptr)
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            Logger::error("SDL could not initialize! SDL_Error: %s", SDL_GetError());
            throw std::runtime_error("SDL could not initialize!");
            return;
        }

        mWindow = SDL_CreateWindow(aTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, mWidth, mHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
        if (mWindow == nullptr)
        {
            Logger::error("Window could not be created! SDL_Error: %s", SDL_GetError());
            throw std::runtime_error("Window could not be created!");
            return;
        }

        mGLContext = SDL_GL_CreateContext(mWindow);
        if (mGLContext == nullptr)
        {
            Logger::error("OpenGL context could not be created! SDL_Error: %s", SDL_GetError());
            throw std::runtime_error("OpenGL context could not be created!");
            return;
        }

        if (glewInit() != GLEW_OK)
        {
            Logger::error("GLEW could not be initialized!");
            throw std::runtime_error("GLEW could not be initialized!");
            return;
        }

        SDL_GL_SetSwapInterval(1); // VSync

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glViewport(0, 0, mWidth, mHeight);

        Logger::info("SDL Renderer with OpenGL initialized");
        Logger::info("OpenGL version: {}", (char *)glGetString(GL_VERSION));
        Logger::info("OpenGL vendor: {}", (char *)glGetString(GL_VENDOR));
        Logger::info("OpenGL renderer: {}", (char *)glGetString(GL_RENDERER));
        Logger::info("OpenGL GLSL version: {}", (char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
    }

    SDLGLRenderer::~SDLGLRenderer()
    {
        SDL_GL_DeleteContext(mGLContext);
        SDL_DestroyWindow(mWindow);
        SDL_Quit();
    }

    bool SDLGLRenderer::PollWindowEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                return false;
            }
            if (event.type == SDL_WINDOWEVENT && event.window.windowID == SDL_GetWindowID(mWindow))
            {
                if (event.window.event == SDL_WINDOWEVENT_CLOSE)
                {
                    return false;
                }
            }
        }
        return true;
    }

    void SDLGLRenderer::Clear()
    {
        glViewport(0, 0, mWidth, mHeight);
        glClearColor(1.0F, 0.0F, 0.0F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void SDLGLRenderer::Render()
    {
        SDL_GL_SwapWindow(mWindow);
    }

    Renderer::DataHandle SDLGLRenderer::LoadData(const std::vector<std::pair<glm::vec3, glm::vec2>> &aData)
    {
        try
        {
            std::vector<float> vertices;
            for (auto &vertex : aData)
            {
                // Position
                vertices.push_back(vertex.first.x);
                vertices.push_back(vertex.first.y);
                vertices.push_back(vertex.first.z);
                // Texture coordinates
                vertices.push_back(vertex.second.x);
                vertices.push_back(vertex.second.y);
            }
            auto data = std::make_shared<GLData>(vertices);
            mData[data->GetVAO()] = data;
            return data->GetVAO();
        }
        catch (std::runtime_error &e)
        {
            Logger::error("Failed to load data: {}", e.what());
            return 0;
        }
    }

    void SDLGLRenderer::DeleteData(DataHandle aHandle)
    {
        if (mData.erase(aHandle) == 0)
        {
            Logger::warn("Tried to delete data #{}, which does not exist", aHandle);
        }
    }

    void SDLGLRenderer::DrawData(DataHandle aHandle, const Camera &aCamera, const glm::mat4 &aTransform)
    {
        auto data = mData.find(aHandle);
        if (data == mData.end())
        {
            Logger::warn("Tried to draw data #{}, which does not exist", aHandle);
            return;
        }

        GLuint VAO = data->second->GetVAO();
        GLuint VBO = data->second->GetVBO();

        if (mCurrentShader == nullptr)
        {
            Logger::warn("Tried to draw data #{}, but no shader is set", aHandle);
        }

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glm::mat4 mvp = aCamera.GetProjectionViewMatrix() * aTransform;
        glUniformMatrix4fv(mCurrentShader->GetModelViewProjectionLocation(), 1, GL_FALSE, glm::value_ptr(mvp));

        glDrawArrays(GL_TRIANGLES, 0, data->second->GetSize());

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    Renderer::ShaderHandle SDLGLRenderer::LoadShader(const std::string &aVertexPath, const std::string &aFragmentPath)
    {
        try
        {
            auto shader = std::make_shared<GLShader>(aVertexPath, aFragmentPath);
            mShaders[shader->GetProgram()] = shader;
            return shader->GetProgram();
        }
        catch (std::runtime_error &e)
        {
            Logger::error("Failed to load shader: {}", e.what());
            return 0;
        }
    }

    void SDLGLRenderer::DeleteShader(ShaderHandle aHandle)
    {
        auto shader = mShaders.find(aHandle);
        if (shader == mShaders.end())
        {
            Logger::warn("Tried to delete shader #{}, which does not exist", aHandle);
            return;
        }

        if (mCurrentShader == shader->second)
        {
            mCurrentShader = nullptr;
        }

        mShaders.erase(shader);
    }

    void SDLGLRenderer::UseShader(ShaderHandle aHandle)
    {
        auto shader = mShaders.find(aHandle);
        if (shader == mShaders.end())
        {
            Logger::error("Shader #{} does not exist, it can not be used", aHandle);
            return;
        }

        mCurrentShader = shader->second;
        glUseProgram(mCurrentShader->GetProgram());
    }

} // namespace nabla2d

// くコ:彡