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

#include "input.hpp"

#include <glm/glm.hpp>
#include "logger.hpp"

namespace nabla2d
{
    std::array<bool, Input::KEY_COUNT> Input::sKeys;
    std::array<bool, Input::KEY_COUNT> Input::sPrevKeys;

    glm::vec2 Input::sMousePos;
    glm::vec2 Input::sPrevMousePos;

    float Input::sMouseScroll;

    void Input::Init()
    {
        sKeys.fill(false);
        sPrevKeys.fill(false);

        sMousePos = {0.0F, 0.0F};
        sPrevMousePos = {0.0F, 0.0F};

        sMouseScroll = 0.0F;
    }

    void Input::Update()
    {
        sPrevKeys = sKeys;
        sPrevMousePos = sMousePos;

        sMouseScroll = 0.0F;
    }

    void Input::FeedKeys(const std::array<bool, KEY_COUNT> &aKeys)
    {
        sKeys = aKeys;
    }

    void Input::FeedMousePos(const glm::vec2 &aMousePos)
    {
        sMousePos = aMousePos;
    }

    void Input::FeedMouseScroll(float aMouseScroll)
    {
        sMouseScroll = aMouseScroll;
    }

    bool Input::KeyDown(Key aKey)
    {
        if (aKey < 0 || aKey >= KEY_COUNT)
        {
            Logger::warn("Invalid key code: {}", static_cast<int>(aKey));
            return false;
        }
        return sKeys[aKey] && !sPrevKeys[aKey];
    }

    bool Input::KeyUp(Key aKey)
    {
        if (aKey < 0 || aKey >= KEY_COUNT)
        {
            Logger::warn("Invalid key code: {}", static_cast<int>(aKey));
            return false;
        }
        return !sKeys[aKey] && sPrevKeys[aKey];
    }

    bool Input::KeyHeld(Key aKey)
    {
        if (aKey < 0 || aKey >= KEY_COUNT)
        {
            Logger::warn("Invalid key code: {}", static_cast<int>(aKey));
            return false;
        }
        return sKeys[aKey];
    }

    glm::vec2 Input::GetMousePos()
    {
        return sMousePos;
    }

    glm::vec2 Input::GetMouseDelta()
    {
        return sMousePos - sPrevMousePos;
    }

    float Input::GetMouseScroll()
    {
        return sMouseScroll;
    }

} // namespace nabla2d

// くコ:彡