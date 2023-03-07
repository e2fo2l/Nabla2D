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

#ifndef NABLA2D_TRANSFORM_HPP
#define NABLA2D_TRANSFORM_HPP

#include <glm/glm.hpp>

namespace nabla2d
{
    class Transform
    {
    public:
        explicit Transform(const glm::vec3 &aPosition = {1.0F, 1.0F, 1.0F},
                           const glm::vec3 &aRotation = {0.0F, 0.0F, 0.0F},
                           const glm::vec3 &aScale = {1.0F, 1.0F, 1.0F});
        Transform(const Transform &aTransform) = default;
        Transform(Transform &&aTransform) = default;
        Transform &operator=(const Transform &aTransform) = default;
        ~Transform() = default;

        const glm::vec3 &GetPosition() const;
        const glm::vec3 &GetRotation() const;
        const glm::vec3 &GetScale() const;

        void SetPosition(const glm::vec3 &aPosition);
        void SetRotation(const glm::vec3 &aRotation);
        void SetScale(const glm::vec3 &aScale);

        void Translate(const glm::vec3 &aTranslation);
        void Rotate(float aAngle, const glm::vec3 &aAxis);
        void Scale(const glm::vec3 &aScale);
        void LookAt(const glm::vec3 &aTarget);

        const glm::mat4 &GetMatrix();

    private:
        glm::vec3 mPosition;
        glm::vec3 mRotation;
        glm::vec3 mScale;

        glm::mat4 mMatrix;

        bool mChanged{true};

        void UpdateMatrix();
    };
} // namespace nabla2d

#endif // NABLA2D_TRANSFORM_HPP

// くコ:彡