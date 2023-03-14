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

#include <cmath>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "logger.hpp"

using json = nlohmann::json;

namespace nabla2d
{
    const std::vector<std::pair<glm::vec3, glm::vec2>> Sprite::kDefaultSquare = {
        /* Position             UV      */
        {{-0.5F, -0.5F, 0.0F}, {0.0, 1.0}},
        {{-0.5F, 0.5F, 0.0F}, {0.0, 0.0}},
        {{0.5F, -0.5F, 0.0F}, {1.0, 1.0}},
        {{-0.5F, 0.5F, 0.0F}, {0.0, 0.0}},
        {{0.5F, 0.5F, 0.0F}, {1.0, 0.0}},
        {{0.5F, -0.5F, 0.0F}, {1.0, 1.0}}};

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
                            const Animation &aAnimation,
                            const Renderer::TextureFilter &aFilter)
    {
        Sprite *sprite = new Sprite();
        sprite->mRenderer = aRenderer;
        sprite->mPath = aPath;
        sprite->mSize = aSize;

        sprite->mTexture = sprite->mRenderer->LoadTexture(sprite->mPath, aFilter);
        sprite->mTextureInfo = sprite->mRenderer->GetTextureInfo(sprite->mTexture);
        sprite->mSpriteData = sprite->mRenderer->LoadData(GetSquare({sprite->mTextureInfo.width, sprite->mTextureInfo.height}));

        sprite->mAnimations[""] = aAnimation;
        sprite->mFrames.emplace_back();
        sprite->SetAnimation("");

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

        try
        {
            for (auto &frame : spriteJson["frames"])
            {
                Frame newFrame;
                newFrame.duration = frame["duration"].get<float>() / 1000.0F;
                newFrame.atlasInfo = {
                    frame["frame"]["x"].get<float>() / sprite->mTextureInfo.width,
                    frame["frame"]["y"].get<float>() / sprite->mTextureInfo.height,
                    frame["frame"]["w"].get<float>() / sprite->mTextureInfo.width,
                    frame["frame"]["h"].get<float>() / sprite->mTextureInfo.height};
                sprite->mFrames.push_back(newFrame);
            }
        }
        catch (json::type_error &e)
        {
            Logger::error("Sprite::FromJSON: Failed to get frames from file '{}': {}", aPath, e.what());
            return nullptr;
        }

        try
        {
            for (auto &animation : spriteJson["meta"]["frameTags"])
            {
                std::string animName = animation["name"].get<std::string>();
                sprite->mAnimations[animName] = {};
                auto &newAnimation = sprite->mAnimations[animName];

                std::string typeString = animation["direction"].get<std::string>();
                if (typeString == "forward")
                {
                    newAnimation.type = FORWARD;
                }
                else if (typeString == "backward")
                {
                    newAnimation.type = BACKWARD;
                }
                else if (typeString == "pingpong")
                {
                    newAnimation.type = PINGPONG;
                }
                else if (typeString == "static")
                {
                    newAnimation.type = STATIC;
                }

                newAnimation.startIndex = animation["from"].get<int>();
                newAnimation.endIndex = animation["to"].get<int>();
            }
        }
        catch (json::type_error &e)
        {
            Logger::error("Sprite::FromJSON: Failed to get animations from file '{}': {}", aPath, e.what());
            return nullptr;
        }

        int framesSize = static_cast<int>(sprite->mFrames.size());
        sprite->mAnimations[""] = {STATIC, framesSize, framesSize};
        sprite->mFrames.emplace_back();
        sprite->SetAnimation(aDefaultAnimation);

        return sprite;
    }

    void Sprite::UpdateAnimation(float aDeltaTime)
    {
        if (mCurrentAnimation->type == STATIC)
        {
            return;
        }

        mTimeElapsed += aDeltaTime;

        if (mTimeElapsed >= mFrames[mCurrentFrameIndex].duration)
        {
            mTimeElapsed = std::fmod(mTimeElapsed, mFrames[mCurrentFrameIndex].duration);

            int nextFrameIndex = mCurrentFrameIndex + mAnimationDirection;
            if (nextFrameIndex < mCurrentAnimation->startIndex || nextFrameIndex > mCurrentAnimation->endIndex)
            {
                switch (mCurrentAnimation->type)
                {
                case FORWARD:
                    nextFrameIndex = mCurrentAnimation->startIndex;
                    break;
                case BACKWARD:
                    nextFrameIndex = mCurrentAnimation->endIndex;
                    break;
                case PINGPONG:
                    mAnimationDirection *= -1;
                    nextFrameIndex += mAnimationDirection * 2;
                    break;
                default:
                    nextFrameIndex = 0;
                    break;
                }
            }

            mCurrentFrameIndex = nextFrameIndex;
        }
    }

    void Sprite::Draw(Camera &aCamera, const glm::mat4 &aParentTransform)
    {
        mRenderer->UseTexture(mTexture);
        auto drawParameters = Renderer::DrawParameters();
        drawParameters.atlasInfo = mFrames.at(mCurrentFrameIndex).atlasInfo;
        mRenderer->DrawData(mSpriteData, aCamera, glm::scale(aParentTransform, {mSize.x, mSize.y, 1.0F}), drawParameters);
    }

    const std::string &Sprite::GetPath() const
    {
        return mPath;
    }

    const Renderer::TextureFilter &Sprite::GetFilter() const
    {
        return mFilter;
    }

    const Renderer::TextureInfo &Sprite::GetTextureInfo() const
    {
        return mTextureInfo;
    }

    const glm::vec2 &Sprite::GetSize() const
    {
        return mSize;
    }

    void Sprite::SetSize(const glm::vec2 &aSize)
    {
        mSize = aSize;
    }

    const std::string &Sprite::GetAnimation() const
    {
        return mAnimationTag;
    }

    void Sprite::SetAnimation(const std::string &aAnimation)
    {
        try
        {
            mCurrentAnimation = &mAnimations.at(aAnimation);
            mAnimationTag = aAnimation;
            mCurrentFrameIndex = mCurrentAnimation->startIndex;
            mTimeElapsed = 0.0F;
            mAnimationDirection = mCurrentAnimation->type == AnimationType::BACKWARD ? -1 : 1;
        }
        catch (std::out_of_range &e)
        {
            Logger::error("Sprite::SetAnimation: Animation '{}' not found", aAnimation);
        }
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