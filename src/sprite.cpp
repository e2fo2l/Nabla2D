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

    Sprite::Sprite(Renderer *mRenderer,
                   const std::string &aPath,
                   const glm::ivec2 &aSubspriteCount,
                   const glm::vec2 &aSize,
                   Renderer::TextureFilter aFilter) : mPath(aPath),
                                                      mSubspriteCount(aSubspriteCount),
                                                      mSize(aSize),
                                                      mFilter(aFilter),
                                                      mTexture(mRenderer->LoadTexture(mPath, mFilter))
    {
        if (mSubspriteCount.x <= 0 || mSubspriteCount.y <= 0)
        {
            Logger::error("Sprite::Sprite: Subsprite count must be greater than 0 ({},{})", mSubspriteCount.x, mSubspriteCount.y);
            throw std::runtime_error("Sprite::Sprite: Subsprite count must be greater than 0");
        }

        const float ratio = (float)mSubspriteCount.x / (float)mSubspriteCount.y;
        auto baseSquare = kDefaultSquare;

        if (ratio > 1.0F)
        {
            for (auto &v : baseSquare)
            {
                v.first.x *= ratio;
            }
        }
        else
        {
            for (auto &v : baseSquare)
            {
                v.first.y /= ratio;
            }
        }
        for (auto &v : baseSquare)
        {
            v.first.x *= mSize.x;
            v.first.y *= mSize.y;
        }

        for (int j = mSubspriteCount.y - 1; j >= 0; j--)
        {
            for (int i = 0; i < mSubspriteCount.x; i++)
            {
                const glm::vec4 minMaxUV = {
                    (float)i / (float)mSubspriteCount.x,
                    (float)(i + 1) / (float)mSubspriteCount.x,
                    (float)j / (float)mSubspriteCount.y,
                    (float)(j + 1) / (float)mSubspriteCount.y,
                };

                auto square = baseSquare;

                square[0].second = {minMaxUV.x, minMaxUV.z};
                square[1].second = {minMaxUV.x, minMaxUV.w};
                square[2].second = {minMaxUV.y, minMaxUV.z};
                square[3].second = {minMaxUV.x, minMaxUV.w};
                square[4].second = {minMaxUV.y, minMaxUV.w};
                square[5].second = {minMaxUV.y, minMaxUV.z};

                mSubsprites.push_back(mRenderer->LoadData(square));
            }
        }
    }

    Sprite::~Sprite()
    {
        mSubsprites.clear();
    }

    void Sprite::Clear(Renderer *mRenderer)
    {
        for (auto &e : mSubsprites)
        {
            mRenderer->DeleteData(e);
        }
        mRenderer->DeleteTexture(mTexture);
    }

    void Sprite::Draw(Renderer *mRenderer, Camera &mCamera, const glm::mat4 &aParentTransform)
    {
        mRenderer->UseTexture(mTexture);
        mRenderer->DrawData(mSubsprites[mCurrentSubsprite], mCamera, aParentTransform);
    }

    const std::string &Sprite::GetPath() const
    {
        return mPath;
    }

    const glm::ivec2 &Sprite::GetSubspriteCount() const
    {
        return mSubspriteCount;
    }

    const glm::vec2 &Sprite::GetSize() const
    {
        return mSize;
    }

    const Renderer::TextureFilter &Sprite::GetFilter() const
    {
        return mFilter;
    }

    int Sprite::GetSubsprite() const
    {
        return mCurrentSubsprite;
    }

    glm::ivec2 Sprite::GetSubsprite2D() const
    {
        return {mCurrentSubsprite % mSubspriteCount.x, mCurrentSubsprite / mSubspriteCount.x};
    }

    void Sprite::SetSubsprite(const glm::ivec2 &aSubsprite)
    {
        SetSubsprite(aSubsprite.x + aSubsprite.y * mSubspriteCount.x);
    }

    void Sprite::SetSubsprite(int aSubsprite)
    {
        if (aSubsprite < 0 || aSubsprite >= mSubspriteCount.x * mSubspriteCount.y)
        {
            Logger::warn("Sprite::SetSubsprite: Subsprite out of range ({})", aSubsprite);
            return;
        }
        mCurrentSubsprite = aSubsprite;
    }

} // namespace nabla2d

// くコ:彡