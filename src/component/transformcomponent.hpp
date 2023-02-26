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

#ifndef NABLA2D_TRANSFORMCOMPONENT_HPP
#define NABLA2D_TRANSFORMCOMPONENT_HPP

#include <glm/glm.hpp>
#include <gtx/transform.hpp>

namespace nabla2d
{
    struct TransformComponent
    {
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;

        TransformComponent() : position(0.0f), rotation(0.0f), scale(1.0f) {}

        glm::mat4 GetModelMatrix() const
        {
            glm::mat4 model = glm::mat4(1.0F);
            model = glm::translate(model, position);
            model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0F, 0.0F, 0.0F));
            model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0F, 1.0F, 0.0F));
            model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0F, 0.0F, 1.0F));
            model = glm::scale(model, scale);
            return model;
        }
    };
} // namespace nabla2d

#endif // NABLA2D_TRANSFORMCOMPONENT_HPP

// くコ:彡