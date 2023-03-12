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

#ifndef NABLA2D_SPRITE_HPP
#define NABLA2D_SPRITE_HPP

#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include "renderer/renderer.hpp"

namespace nabla2d
{
    class Sprite
    {
    public:
        ~Sprite();

        static Sprite *FromPNG(std::shared_ptr<Renderer> aRenderer,
                               const std::string &aPath,
                               const glm::vec2 &aSize = {1.0F, 1.0F},
                               const Renderer::TextureFilter &aFilter = Renderer::TextureFilter::NEAREST);
        static Sprite *FromJSON(std::shared_ptr<Renderer> aRenderer,
                                const std::string &aPath,
                                const glm::vec2 &aSize = {1.0F, 1.0F},
                                const std::string &aDefaultAnimation = "",
                                const Renderer::TextureFilter &aFilter = Renderer::TextureFilter::NEAREST);

        void Draw(Camera &aCamera, const glm::mat4 &aParentTransform);

        const std::string &GetPath() const;
        const Renderer::TextureFilter &GetFilter() const;
        const Renderer::TextureInfo &GetTextureInfo() const;

        const glm::vec2 &GetSize() const;
        void SetSize(const glm::vec2 &aSize);

        const glm::vec4 &GetAtlasInfo() const;
        void SetAtlasInfo(const glm::vec4 &aAtlasInfo);

    private:
        Sprite() = default;
        static std::vector<std::pair<glm::vec3, glm::vec2>> GetSquare(const glm::vec2 &aSize);

        static const std::vector<std::pair<glm::vec3, glm::vec2>> kDefaultSquare;

        std::shared_ptr<Renderer> mRenderer;
        std::string mPath{""};
        glm::vec2 mSize{1.0F, 1.0F};
        Renderer::TextureFilter mFilter{Renderer::TextureFilter::NEAREST};
        Renderer::TextureInfo mTextureInfo{0, 0, 0};

        Renderer::TextureHandle mTexture{0};
        Renderer::DataHandle mSpriteData{0};
        glm::vec4 mAtlasInfo{0.0F, 0.0F, 1.0F, 1.0F};
    };
} // namespace nabla2d

#endif // NABLA2D_SPRITE_HPP

// くコ:彡