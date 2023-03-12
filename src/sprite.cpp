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

#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "logger.hpp"

using json = nlohmann::json;

namespace nabla2d
{
    const std::vector<std::pair<glm::vec3, glm::vec2>> Sprite::kDefaultSquare = {
        /* Position             UV      */
        {{-0.5F, -0.5F, 0.0F}, {0.0, 0.0}},
        {{-0.5F, 0.5F, 0.0F}, {0.0, 1.0}},
        {{0.5F, -0.5F, 0.0F}, {1.0, 0.0}},
        {{-0.5F, 0.5F, 0.0F}, {0.0, 1.0}},
        {{0.5F, 0.5F, 0.0F}, {1.0, 1.0}},
        {{0.5F, -0.5F, 0.0F}, {1.0, 0.0}}};

    Sprite::~Sprite()
    {
        if (mSpriteData != 0)
        {
            mRenderer->DeleteData(mSpriteData);
        }
        if (mTexture != 0)
        {
            mRenderer->DeleteTexture(mTexture);
        }
    }

    Sprite *Sprite::FromPNG(std::shared_ptr<Renderer> aRenderer,
                            const std::string &aPath,
                            const glm::vec2 &aSize,
                            const Renderer::TextureFilter &aFilter)
    {
        Sprite *sprite = new Sprite();
        sprite->mRenderer = aRenderer;
        sprite->mPath = aPath;
        sprite->mSize = aSize;

        sprite->mTexture = sprite->mRenderer->LoadTexture(sprite->mPath, aFilter);
        sprite->mTextureInfo = sprite->mRenderer->GetTextureInfo(sprite->mTexture);

        sprite->mSpriteData = sprite->mRenderer->LoadData(GetSquare({sprite->mTextureInfo.width, sprite->mTextureInfo.height}));

        return sprite;
    }

    Sprite *Sprite::FromJSON(std::shared_ptr<Renderer> aRenderer,
                             const std::string &aPath,
                             const glm::vec2 &aSize,
                             const std::string &aDefaultAnimation,
                             const Renderer::TextureFilter &aFilter)
    {
        Sprite *sprite = new Sprite();
        sprite->mRenderer = aRenderer;
        sprite->mPath = aPath;
        sprite->mSize = aSize;

        std::ifstream jsonFile(aPath);
        if (!jsonFile.is_open())
        {
            Logger::error("Sprite::FromJSON: Failed to open file '{}'", aPath);
            return nullptr;
        }

        json spriteJson;
        try
        {
            spriteJson = json::parse(jsonFile);
        }
        catch (json::parse_error &e)
        {
            Logger::error("Sprite::FromJSON: Failed to parse file '{}': {}", aPath, e.what());
            return nullptr;
        }

        std::string imageName;
        try
        {
            imageName = spriteJson["meta"]["image"].get<std::string>();
        }
        catch (json::type_error &e)
        {
            Logger::error("Sprite::FromJSON: Failed to get image name from file '{}': {}", aPath, e.what());
            return nullptr;
        }

        auto baseDir = std::filesystem::path(aPath).parent_path();
        const std::string texturePath = baseDir / imageName;

        sprite->mTexture = sprite->mRenderer->LoadTexture(texturePath, aFilter);
        sprite->mTextureInfo = sprite->mRenderer->GetTextureInfo(sprite->mTexture);
        sprite->mSpriteData = sprite->mRenderer->LoadData(GetSquare({sprite->mTextureInfo.width, sprite->mTextureInfo.height}));

        (void)aDefaultAnimation;

        return sprite;
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

    std::vector<std::pair<glm::vec3, glm::vec2>> Sprite::GetSquare(const glm::vec2 &aSize)
    {
        auto square = kDefaultSquare;
        const float ratio = (float)aSize.x / (float)aSize.y;

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

        return square;
    }

} // namespace nabla2d

// くコ:彡