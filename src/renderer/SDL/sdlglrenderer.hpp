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

#ifndef NABLA2D_SDLGLRENDERER_HPP
#define NABLA2D_SDLGLRENDERER_HPP

#include <string>
#include <memory>
#include <cstdint>
#include <unordered_map>
#include <SDL2/SDL.h>

#define GLEW_STATIC 1
#include <GL/glew.h>

#include "../renderer.hpp"
#include "../OpenGL/gldata.hpp"
#include "../OpenGL/glshader.hpp"
#include "../OpenGL/gltexture.hpp"

namespace nabla2d
{
    class SDLGLRenderer : public Renderer
    {
    public:
        SDLGLRenderer(const std::string &aTitle, const std::pair<int, int> &aSize);
        ~SDLGLRenderer() override;

        int GetWidth() const override;
        int GetHeight() const override;
        float GetAspectRatio() const override;
        const std::string &GetRendererInfo() const override;

        bool PollWindowEvents() override;
        void SetMouseCapture(bool aCapture) override;
        bool HasBeenResized() const override;

        void Clear() override;
        void Render() override;

        DataHandle LoadData(const std::vector<std::pair<glm::vec3, glm::vec2>> &aData) override;
        DataHandle LoadData(const std::vector<std::pair<glm::vec3, glm::vec2>> &aData, const std::vector<unsigned int> &aIndices) override;
        DataHandle LoadDataDynamic(const std::vector<std::pair<glm::vec3, glm::vec2>> &aData) override;
        DataHandle LoadDataDynamic(const std::vector<std::pair<glm::vec3, glm::vec2>> &aData, const std::vector<unsigned int> &aIndices) override;
        DataHandle LoadDataLines(const std::vector<glm::vec3> &aPoints, const std::vector<unsigned int> &aIndices) override;
        void UpdateData(DataHandle aHandle, const std::vector<float> &aVertices, const std::vector<unsigned int> &aIndices) override;
        void DeleteData(DataHandle aHandle) override;
        void DrawData(DataHandle aHandle, const Camera &aCamera, const glm::mat4 &aTransform, const DrawParameters &aDrawParameters) override;

        ShaderHandle LoadShader(const std::string &aVertexPath, const std::string &aFragmentPath) override;
        void DeleteShader(ShaderHandle aHandle) override;
        void UseShader(ShaderHandle aHandle) override;

        TextureHandle LoadTexture(const std::string &aPath, Renderer::TextureFilter aFilter) override;
        void DeleteTexture(TextureHandle aHandle) override;
        void UseTexture(TextureHandle aHandle) override;
        TextureInfo GetTextureInfo(TextureHandle aHandle) override;

    private:
        DataHandle LoadDataInternal(const std::vector<float> &aVertices, const std::vector<unsigned int> &aIndices, GLenum aDrawMode, GLenum aDrawUsage);

        int mWidth;
        int mHeight;
        std::string mRendererInfo;
        bool mResized{false};

        std::shared_ptr<GLShader> mCurrentShader;
        std::shared_ptr<GLTexture> mCurrentTexture;

        std::unordered_map<DataHandle, std::shared_ptr<GLData>> mData{};
        std::unordered_map<ShaderHandle, std::shared_ptr<GLShader>> mShaders{};
        std::unordered_map<TextureHandle, std::shared_ptr<GLTexture>> mTextures{};

        SDL_Window *mWindow;
        SDL_GLContext mGLContext;

        float mLineWidthMin{0.0f};
        float mLineWidthMax{0.0f};

        void UpdateInput(const uint8_t *aKeys, float aMouseScroll);
        bool mMouseButtons[3]{false, false, false};
        bool mMouseCapured{false};
    };
} // namespace nabla2d

#endif // NABLA2D_SDLGLRENDERER_HPP

// くコ:彡