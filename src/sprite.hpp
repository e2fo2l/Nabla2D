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
#include <glm/glm.hpp>

#include "renderer/renderer.hpp"

namespace nabla2d
{
    class Sprite
    {
    public:
        Sprite(Renderer *mRenderer, const std::string &aPath, const glm::ivec2 &aSubspriteCount = {1, 1}, const glm::vec2 &aSize = {1.0F, 1.0F}, Renderer::TextureFilter aFilter = Renderer::TextureFilter::LINEAR);
        ~Sprite();
        void Clear(Renderer *mRenderer);

        void Draw(Renderer *mRenderer, Camera &mCamera, const glm::mat4 &aParentTransform);

        const std::string &GetPath() const;
        const glm::ivec2 &GetSubspriteCount() const;
        const glm::vec2 &GetSize() const;
        const Renderer::TextureFilter &GetFilter() const;

        int GetSubsprite() const;
        glm::ivec2 GetSubsprite2D() const;

        void SetSubsprite(const glm::ivec2 &aSubsprite);
        void SetSubsprite(int aSubsprite);

    private:
        std::string mPath;
        glm::ivec2 mSubspriteCount;
        glm::vec2 mSize;
        Renderer::TextureFilter mFilter;

        int mCurrentSubsprite{0};
        Renderer::TextureHandle mTexture;
        std::vector<Renderer::DataHandle> mSubsprites;
    };
} // namespace nabla2d

#endif // NABLA2D_SPRITE_HPP

// くコ:彡