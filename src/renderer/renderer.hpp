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

#ifndef NABLA2D_RENDERER_HPP
#define NABLA2D_RENDERER_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <glm/glm.hpp>

#include "../camera.hpp"
namespace nabla2d
{
    // Abstract class for rendering
    class Renderer
    {
    public:
        typedef uint64_t DataHandle;
        typedef uint64_t ShaderHandle;
        typedef uint64_t TextureHandle;

        typedef enum
        {
            NEAREST,
            LINEAR,
            NEAREST_MIPMAP_NEAREST,
            LINEAR_MIPMAP_NEAREST,
            NEAREST_MIPMAP_LINEAR,
            LINEAR_MIPMAP_LINEAR
        } TextureFilter;

        typedef struct
        {
            int width;
            int height;
            int channels;
        } TextureInfo;

        typedef struct
        {
            glm::vec4 atlasInfo = glm::vec4(0.0F, 0.0F, 0.0F, 0.0F);
            glm::vec4 color = glm::vec4(0.0F, 0.0f, 0.0f, 0.0f);
            float lineWidth = 0.0F;
        } DrawParameters;

        virtual ~Renderer() = default;

        static Renderer *Create(const std::string &aTitle, const std::pair<int, int> &aSize);

        virtual int GetWidth() const = 0;
        virtual int GetHeight() const = 0;
        virtual float GetAspectRatio() const = 0;
        virtual const std::string &GetRendererInfo() const = 0;

        virtual bool PollWindowEvents() = 0;
        virtual void SetMouseCapture(bool aCapture) = 0;
        virtual bool HasBeenResized() const = 0;

        virtual void Clear() = 0;
        virtual void Render() = 0;

        virtual DataHandle LoadData(const std::vector<std::pair<glm::vec3, glm::vec2>> &aData) = 0;
        virtual DataHandle LoadDataLines(const std::vector<glm::vec3> &aPoints, const std::vector<unsigned int> &aIndices) = 0;
        virtual void DeleteData(DataHandle aHandle) = 0;
        virtual void DrawData(DataHandle aHandle, const Camera &aCamera, const glm::mat4 &aTransform, const DrawParameters &aDrawParameters) = 0;

        virtual ShaderHandle LoadShader(const std::string &aVertexPath, const std::string &aFragmentPath) = 0;
        virtual void DeleteShader(ShaderHandle aHandle) = 0;
        virtual void UseShader(ShaderHandle aHandle) = 0;

        virtual TextureHandle LoadTexture(const std::string &aPath, TextureFilter aFilter) = 0;
        virtual void DeleteTexture(TextureHandle aHandle) = 0;
        virtual void UseTexture(TextureHandle aHandle) = 0;
        virtual TextureInfo GetTextureInfo(TextureHandle aHandle) = 0;

        // TODO : Define a way to get the "error" handle, don't hardcode it to 0 everywhere
    };
} // namespace nabla2d

#endif // NABLA2D_RENDERER_HPP

// くコ:彡