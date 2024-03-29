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
#include <glm/gtx/quaternion.hpp>

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
        const glm::quat &GetRotationQuat() const;
        const glm::vec3 &GetScale() const;

        void SetPosition(const glm::vec3 &aPosition);
        void SetRotation(const glm::vec3 &aRotation);
        void SetRotation(const glm::quat &aRotation);
        void SetScale(const glm::vec3 &aScale);

        void Translate(const glm::vec3 &aTranslation);
        void Rotate(float aAngle, const glm::vec3 &aAxis);
        void Scale(const glm::vec3 &aScale);
        void LookAt(const glm::vec3 &aTarget, const glm::vec3 &aUp = {0.0F, 0.0F, 1.0F});

        void Lerp(const Transform &aTarget, float aInterpolation);
        static Transform Lerp(const Transform &aT1, const Transform &aT2, float aInterpolation);

        const glm::mat4 &GetMatrix();
        const glm::vec3 &GetForward();
        const glm::vec3 &GetUp();
        const glm::vec3 &GetRight();

    private:
        glm::vec3 mPosition;
        glm::quat mRotation;
        glm::vec3 mEulerRotation;
        glm::vec3 mScale;

        glm::mat4 mMatrix;
        glm::vec3 mForward;
        glm::vec3 mUp;
        glm::vec3 mRight;

        bool mChanged{true};

        void UpdateMatrix();
        void RecalculateEuler();
    };
} // namespace nabla2d

#endif // NABLA2D_TRANSFORM_HPP

// くコ:彡