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

#include "sprite.hpp"

#include "logger.hpp"

namespace nabla2d
{
    const std::vector<std::pair<glm::vec3, glm::vec2>> kDefaultSquare = {
        {{-0.5F, -0.5F, 0.0F}, {0.0, 0.0}},
        {{-0.5F, 0.5F, 0.0F}, {0.0, 1.0}},
        {{0.5F, -0.5F, 0.0F}, {1.0, 0.0}},
        {{-0.5F, 0.5F, 0.0F}, {0.0, 1.0}},
        {{0.5F, 0.5F, 0.0F}, {1.0, 1.0}},
        {{0.5F, -0.5F, 0.0F}, {1.0, 0.0}}};

    Sprite::Sprite(std::shared_ptr<Renderer> aRenderer,
                   const std::string &aPath,
                   const glm::vec2 &aSize,
                   Renderer::TextureFilter aFilter) : mRenderer(aRenderer),
                                                      mPath(aPath),
                                                      mSize(aSize),
                                                      mFilter(aFilter)
    {
        mTexture = mRenderer->LoadTexture(mPath, mFilter);
        mTextureInfo = mRenderer->GetTextureInfo(mTexture);

        const float ratio = (float)mTextureInfo.width / (float)mTextureInfo.height;
        auto square = kDefaultSquare;

        if (ratio > 1.0F)
        {
            for (auto &v : square)
            {
                v.first.y /= ratio;
            }
        }
        else
        {
            for (auto &v : square)
            {
                v.first.x *= ratio;
            }
        }

        mSpriteData = mRenderer->LoadData(square);
    }

    Sprite::~Sprite()
    {
        mRenderer->DeleteData(mSpriteData);
        mRenderer->DeleteTexture(mTexture);
    }

    void Sprite::Draw(Camera &aCamera, const glm::mat4 &aParentTransform)
    {
        mRenderer->UseTexture(mTexture);
        auto drawParameters = Renderer::DrawParameters();
        drawParameters.atlasInfo = mAtlasInfo;
        mRenderer->DrawData(mSpriteData, aCamera, glm::scale(aParentTransform, {mSize.x, mSize.y, 1.0F}), drawParameters);
    }

    const std::string &Sprite::GetPath() const
    {
        return mPath;
    }

    const glm::vec2 &Sprite::GetSize() const
    {
        return mSize;
    }

    void Sprite::SetSize(const glm::vec2 &aSize)
    {
        mSize = aSize;
    }

    const glm::vec4 &Sprite::GetAtlasInfo() const
    {
        return mAtlasInfo;
    }

    const Renderer::TextureFilter &Sprite::GetFilter() const
    {
        return mFilter;
    }

    const Renderer::TextureInfo &Sprite::GetTextureInfo() const
    {
        return mTextureInfo;
    }

    void Sprite::SetAtlasInfo(const glm::vec4 &aAtlasInfo)
    {
        mAtlasInfo = aAtlasInfo;
    }

} // namespace nabla2d

// くコ:彡