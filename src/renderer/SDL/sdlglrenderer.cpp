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

#include <array>
#include <algorithm>
#include <stdexcept>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include "imgui/imgui_impl_sdl2.h"
#include "../OpenGL/imgui/imgui_impl_opengl3.h"

#include "../../logger.hpp"
#include "../../input.hpp"
#include "../renderer.hpp"

namespace nabla2d
{

    SDLGLRenderer::SDLGLRenderer(const std::string &aTitle, const std::pair<int, int> &aSize) : mWidth(aSize.first),
                                                                                                mHeight(aSize.second),
                                                                                                mRendererInfo("SDL2 w/ OpenGL"),
                                                                                                mCurrentShader(nullptr),
                                                                                                mCurrentTexture(nullptr)
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

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.01f);

        glViewport(0, 0, mWidth, mHeight);

        float lineWidthRange[2];
        glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, &lineWidthRange[0]);
        mLineWidthMin = lineWidthRange[0];
        mLineWidthMax = lineWidthRange[1];

        Logger::info("SDL Renderer with OpenGL initialized");
        Logger::info("OpenGL version: {}", (char *)glGetString(GL_VERSION));
        Logger::info("OpenGL vendor: {}", (char *)glGetString(GL_VENDOR));
        Logger::info("OpenGL renderer: {}", (char *)glGetString(GL_RENDERER));
        Logger::info("OpenGL GLSL version: {}", (char *)glGetString(GL_SHADING_LANGUAGE_VERSION));

        // ImGui

        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui::StyleColorsDark();

        ImGui_ImplSDL2_InitForOpenGL(mWindow, mGLContext);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    SDLGLRenderer::~SDLGLRenderer()
    {
        mCurrentShader = nullptr;
        mCurrentTexture = nullptr;

        mData.clear();
        mShaders.clear();
        mTextures.clear();

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        SDL_GL_DeleteContext(mGLContext);
        SDL_DestroyWindow(mWindow);
        SDL_Quit();
    }

    int SDLGLRenderer::GetWidth() const
    {
        return mWidth;
    }

    int SDLGLRenderer::GetHeight() const
    {
        return mHeight;
    }

    float SDLGLRenderer::GetAspectRatio() const
    {
        return static_cast<float>(mWidth) / static_cast<float>(mHeight);
    }

    const std::string &SDLGLRenderer::GetRendererInfo() const
    {
        return mRendererInfo;
    }

    void SDLGLRenderer::UpdateInput(const uint8_t *aKeys, float aMouseScroll)
    {
        // Keys
        std::array<bool, Input::Key::KEY_COUNT> keys;
        keys.fill(false);

        keys[Input::Key::KEY_UP] = aKeys[SDL_SCANCODE_UP];
        keys[Input::Key::KEY_DOWN] = aKeys[SDL_SCANCODE_DOWN];
        keys[Input::Key::KEY_LEFT] = aKeys[SDL_SCANCODE_LEFT];
        keys[Input::Key::KEY_RIGHT] = aKeys[SDL_SCANCODE_RIGHT];
        keys[Input::Key::KEY_SPACE] = aKeys[SDL_SCANCODE_SPACE];
        keys[Input::Key::KEY_ENTER] = aKeys[SDL_SCANCODE_RETURN];
        keys[Input::Key::KEY_ESCAPE] = aKeys[SDL_SCANCODE_ESCAPE];
        keys[Input::Key::KEY_BACKSPACE] = aKeys[SDL_SCANCODE_BACKSPACE];
        keys[Input::Key::KEY_TAB] = aKeys[SDL_SCANCODE_TAB];
        keys[Input::Key::KEY_LCTRL] = aKeys[SDL_SCANCODE_LCTRL];
        keys[Input::Key::KEY_LSHIFT] = aKeys[SDL_SCANCODE_LSHIFT];
        keys[Input::Key::KEY_LALT] = aKeys[SDL_SCANCODE_LALT];

        // Mouse input
        keys[Input::Key::KEY_MOUSE0] = mMouseButtons[0];
        keys[Input::Key::KEY_MOUSE1] = mMouseButtons[1];
        keys[Input::Key::KEY_MOUSE2] = mMouseButtons[2];

        Input::FeedKeys(keys);

        // Axes
        std::array<glm::vec2, Input::Axis::AXIS_COUNT> axes;
        axes.fill({0.0F, 0.0F});

        axes[Input::Axis::AXIS_LEFT] = {aKeys[SDL_SCANCODE_A] - aKeys[SDL_SCANCODE_D], aKeys[SDL_SCANCODE_S] - aKeys[SDL_SCANCODE_W]};

        Input::FeedAxes(axes);

        // Mouse (pos + scroll)
        Input::FeedMouseScroll(aMouseScroll);
        int x, y;
        SDL_GetMouseState(&x, &y);
        float localX = static_cast<float>(x) / static_cast<float>(mWidth) - 0.5F;
        float localY = static_cast<float>(y) / static_cast<float>(mHeight) - 0.5F;
        auto prevMousePos = Input::GetMousePos();
        Input::FeedMousePos({localX, localY});

        if (mMouseCapured)
        {
            Input::FeedMouseDelta({localX, localY});
            SDL_WarpMouseInWindow(mWindow, mWidth / 2, mHeight / 2);
        }
        else
        {
            Input::FeedMouseDelta({localX - prevMousePos.x, localY - prevMousePos.y});
        }
    }

    bool SDLGLRenderer::PollWindowEvents()
    {
        float mouseScroll = 0.0F;

        SDL_Event event;
        while (SDL_PollEvent(&event) != 0)
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
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
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    mMouseButtons[0] = true;
                }
                if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    mMouseButtons[1] = true;
                }
                if (event.button.button == SDL_BUTTON_MIDDLE)
                {
                    mMouseButtons[2] = true;
                }
            }
            if (event.type == SDL_MOUSEBUTTONUP)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    mMouseButtons[0] = false;
                }
                if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    mMouseButtons[1] = false;
                }
                if (event.button.button == SDL_BUTTON_MIDDLE)
                {
                    mMouseButtons[2] = false;
                }
            }
            if (event.type == SDL_MOUSEWHEEL)
            {
                mouseScroll = event.wheel.preciseY;
            }
        }

        const uint8_t *keys = SDL_GetKeyboardState(nullptr);
        UpdateInput(keys, mouseScroll);

        return true;
    }

    void SDLGLRenderer::SetMouseCapture(bool aCapture)
    {
        if (aCapture)
        {
            SDL_SetRelativeMouseMode(SDL_TRUE);
            SDL_WarpMouseInWindow(mWindow, mWidth / 2, mHeight / 2);
        }
        else
        {
            SDL_SetRelativeMouseMode(SDL_FALSE);
        }
        mMouseCapured = aCapture;
    }

    void SDLGLRenderer::Clear()
    {
        glViewport(0, 0, mWidth, mHeight);
        glClearColor(0.5F, 0.5F, 0.5F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(mWindow);
        ImGui::NewFrame();
    }

    void SDLGLRenderer::Render()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(mWindow);
    }

    Renderer::DataHandle SDLGLRenderer::LoadData(const std::vector<std::pair<glm::vec3, glm::vec2>> &aData)
    {
        try
        {
            std::vector<float> vertices{};
            vertices.reserve(aData.size() * 5);
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

    Renderer::DataHandle SDLGLRenderer::LoadDataLines(const std::vector<glm::vec3> &aPoints, const std::vector<unsigned int> &aIndices)
    {
        try
        {
            std::vector<float> vertices{};
            vertices.reserve(aPoints.size() * 3);
            for (auto &point : aPoints)
            {
                // Position
                vertices.push_back(point.x);
                vertices.push_back(point.y);
                vertices.push_back(point.z);
            }
            auto data = std::make_shared<GLData>(vertices, aIndices, GL_LINES);
            mData[data->GetVAO()] = data;
            return data->GetVAO();
        }
        catch (std::runtime_error &e)
        {
            Logger::error("Failed to load data: {}", e.what());
            return 0;
        }
        return 0;
    }

    void SDLGLRenderer::DeleteData(DataHandle aHandle)
    {
        if (mData.erase(aHandle) == 0)
        {
            Logger::warn("Tried to delete data #{}, which does not exist", aHandle);
        }
    }

    void SDLGLRenderer::DrawData(DataHandle aHandle, const Camera &aCamera, const glm::mat4 &aTransform, const DrawParameters &aDrawParameters)
    {
        auto data = mData.find(aHandle);
        if (data == mData.end())
        {
            Logger::warn("Tried to draw data #{}, which does not exist", aHandle);
            return;
        }

        if (mCurrentShader == nullptr)
        {
            Logger::warn("Tried to draw data #{}, but no shader is set", aHandle);
        }

        GLuint VAO = data->second->GetVAO();
        glBindVertexArray(VAO);

        if (mCurrentShader != nullptr)
        {
            glm::mat4 mvp = aCamera.GetProjectionViewMatrix() * aTransform;
            glUniformMatrix4fv(mCurrentShader->GetModelViewProjectionLocation(), 1, GL_FALSE, glm::value_ptr(mvp));

            if (mCurrentTexture != nullptr)
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, mCurrentTexture->GetTexture());
                glUniform1i(mCurrentShader->GetTextureLocation(), 0);

                if (aDrawParameters.atlasInfo != glm::vec4{0.0F, 0.0F, 0.0F, 0.0F})
                {
                    glUniform4fv(mCurrentShader->GetAtlasInfoLocation(), 1, glm::value_ptr(aDrawParameters.atlasInfo));
                }
            }
        }

        if (aDrawParameters.color != glm::vec4{0.0F, 0.0F, 0.0F, 0.0F})
        {
            glUniform4fv(mCurrentShader->GetColorLocation(), 1, glm::value_ptr(aDrawParameters.color));
        }

        if (aDrawParameters.lineWidth != 0.0F)
        {
            glLineWidth(std::clamp(aDrawParameters.lineWidth, mLineWidthMin, mLineWidthMax));
        }

        auto mode = data->second->GetMode();
        if (mode == GL_LINES)
        {
            glEnable(GL_LINE_SMOOTH);
        }

        if (data->second->GetEBO() != 0)
        {
            glDrawElements(mode, data->second->GetSize(), GL_UNSIGNED_INT, 0);
        }
        else
        {
            glDrawArrays(mode, 0, data->second->GetSize());
        }

        if (mode == GL_LINES)
        {
            glDisable(GL_LINE_SMOOTH);
        }

        glBindTexture(GL_TEXTURE_2D, 0);
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

    Renderer::TextureHandle SDLGLRenderer::LoadTexture(const std::string &aPath, Renderer::TextureFilter aFilter)
    {
        GLTexture::GLTextureFilter filter = GLTexture::GLTextureFilter::LINEAR;
        switch (aFilter)
        {
        case TextureFilter::NEAREST:
            filter = GLTexture::GLTextureFilter::NEAREST;
            break;
        case TextureFilter::LINEAR:
            filter = GLTexture::GLTextureFilter::LINEAR;
            break;
        case TextureFilter::NEAREST_MIPMAP_NEAREST:
            filter = GLTexture::GLTextureFilter::NEAREST_MIPMAP_NEAREST;
            break;
        case TextureFilter::LINEAR_MIPMAP_NEAREST:
            filter = GLTexture::GLTextureFilter::LINEAR_MIPMAP_NEAREST;
            break;
        case TextureFilter::NEAREST_MIPMAP_LINEAR:
            filter = GLTexture::GLTextureFilter::NEAREST_MIPMAP_LINEAR;
            break;
        case TextureFilter::LINEAR_MIPMAP_LINEAR:
            filter = GLTexture::GLTextureFilter::LINEAR_MIPMAP_LINEAR;
            break;
        default:
            Logger::error("Unknown texture filter: {}", static_cast<int>(aFilter));
            return 0;
        }

        try
        {
            auto texture = std::make_shared<GLTexture>(aPath, filter);
            mTextures[texture->GetTexture()] = texture;
            return texture->GetTexture();
        }
        catch (std::runtime_error &e)
        {
            Logger::error("Failed to load texture: {}", e.what());
            return 0;
        }
    }

    void SDLGLRenderer::DeleteTexture(TextureHandle aHandle)
    {
        auto texture = mTextures.find(aHandle);
        if (texture == mTextures.end())
        {
            Logger::warn("Tried to delete texture #{}, which does not exist", aHandle);
            return;
        }

        if (mCurrentTexture == texture->second)
        {
            mCurrentTexture = nullptr;
        }

        mTextures.erase(texture);
    }

    void SDLGLRenderer::UseTexture(TextureHandle aHandle)
    {
        auto texture = mTextures.find(aHandle);
        if (texture == mTextures.end())
        {
            Logger::error("Texture #{} does not exist, it can not be used", aHandle);
            return;
        }

        mCurrentTexture = texture->second;
        glBindTexture(GL_TEXTURE_2D, mCurrentTexture->GetTexture());
    }

    Renderer::TextureInfo SDLGLRenderer::GetTextureInfo(TextureHandle aHandle)
    {
        auto texture = mTextures.find(aHandle);
        if (texture == mTextures.end())
        {
            Logger::error("Texture #{} does not exist, can't get info", aHandle);
            return {0, 0, 0};
        }

        return {texture->second->GetWidth(), texture->second->GetHeight(), texture->second->GetChannels()};
    }

} // namespace nabla2d

// くコ:彡