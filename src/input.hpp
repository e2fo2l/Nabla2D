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

#ifndef NABLA2D_INPUT_HPP
#define NABLA2D_INPUT_HPP

#include <array>
#include <utility>
#include <glm/glm.hpp>

namespace nabla2d
{
    class Input
    {
    public:
        typedef enum
        {
            KEY_UP = 0,
            KEY_DOWN,
            KEY_LEFT,
            KEY_RIGHT,
            KEY_SPACE,
            KEY_ENTER,
            KEY_ESCAPE,
            KEY_BACKSPACE,
            KEY_TAB,
            KEY_LCTRL,
            KEY_LSHIFT,
            KEY_LALT,
            KEY_MOUSE0, // Left mouse button
            KEY_MOUSE1, // Right mouse button
            KEY_MOUSE2, // Middle mouse button
            KEY_COUNT,
            KEY_UNKNOWN
        } Key;

        typedef enum
        {
            AXIS_LEFT = 0,
            AXIS_RIGHT,
            AXIS_COUNT
        } Axis;

        static void Init();
        static void Update();

        static void FeedKeys(const std::array<bool, KEY_COUNT> &aKeys);
        static void FeedAxes(const std::array<glm::vec2, AXIS_COUNT> &aAxes);
        static void FeedMousePos(const glm::vec2 &aMousePos);
        static void FeedMouseScroll(float aMouseScroll);

        static bool KeyDown(Key aKey);
        static bool KeyUp(Key aKey);
        static bool KeyHeld(Key aKey);

        static const glm::vec2 &GetAxis(Axis aAxis);

        static glm::vec2 GetMousePos();
        static glm::vec2 GetMouseDelta();
        static float GetMouseScroll();

    private:
        static std::array<bool, KEY_COUNT> sKeys;
        static std::array<bool, KEY_COUNT> sPrevKeys;

        static std::array<glm::vec2, AXIS_COUNT> sAxes;

        static glm::vec2 sMousePos;
        static glm::vec2 sPrevMousePos;

        static float sMouseScroll;
    };
} // namespace nabla2d

#endif // NABLA2D_INPUT_HPP

// くコ:彡