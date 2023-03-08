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

#include "transform.hpp"

#include <cmath>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace nabla2d
{
    Transform::Transform(const glm::vec3 &aPosition,
                         const glm::vec3 &aRotation,
                         const glm::vec3 &aScale) : mPosition(aPosition),
                                                    mRotation(aRotation),
                                                    mScale(aScale),
                                                    mMatrix(1.0F),
                                                    mChanged(true)
    {
        UpdateMatrix();
    }

    const glm::vec3 &Transform::GetPosition() const
    {
        return mPosition;
    }

    const glm::vec3 &Transform::GetRotation() const
    {
        return mRotation;
    }

    const glm::vec3 &Transform::GetScale() const
    {
        return mScale;
    }

    void Transform::SetPosition(const glm::vec3 &aPosition)
    {
        mPosition = aPosition;
        mChanged = true;
    }

    void Transform::SetRotation(const glm::vec3 &aRotation)
    {
        mRotation = {std::fmod(aRotation.x, 360.0F),
                     std::fmod(aRotation.y, 360.0F),
                     std::fmod(aRotation.z, 360.0F)};
        mChanged = true;
    }

    void Transform::SetScale(const glm::vec3 &aScale)
    {
        mScale = aScale;
        mChanged = true;
    }

    void Transform::Translate(const glm::vec3 &aTranslation)
    {
        mPosition += aTranslation;
        mChanged = true;
    }

    void Transform::Rotate(float aAngle, const glm::vec3 &aAxis)
    {
        mRotation += aAxis * aAngle;
        mChanged = true;
    }

    void Transform::Scale(const glm::vec3 &aScale)
    {
        mScale *= aScale;
        mChanged = true;
    }

    void Transform::LookAt(const glm::vec3 &aTarget)
    {
        if (aTarget == mPosition)
        {
            return;
        }

        glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::mat4 view = glm::lookAt(mPosition, aTarget, up);
        glm::quat rotation = glm::quat_cast(view);
        mRotation = -glm::degrees(glm::eulerAngles(rotation));
        mChanged = true;
    }

    void Transform::Lerp(const Transform &aTarget, float aInterpolation)
    {
        mPosition = glm::lerp(mPosition, aTarget.GetPosition(), aInterpolation);
        mRotation = glm::lerp(mRotation, aTarget.GetRotation(), aInterpolation);
        mScale = glm::lerp(mScale, aTarget.GetScale(), aInterpolation);
        mChanged = true;
    }

    Transform Transform::Lerp(const Transform &aT1, const Transform &aT2, float aInterpolation)
    {
        Transform result = aT1;
        result.Lerp(aT2, aInterpolation);
        return result;
    }

    const glm::mat4 &Transform::GetMatrix()
    {
        if (mChanged)
        {
            UpdateMatrix();
        }
        return mMatrix;
    }

    const glm::vec3 &Transform::GetForward()
    {
        if (mChanged)
        {
            UpdateMatrix();
        }
        return mForward;
    }

    const glm::vec3 &Transform::GetUp()
    {
        if (mChanged)
        {
            UpdateMatrix();
        }
        return mUp;
    }

    const glm::vec3 &Transform::GetRight()
    {
        if (mChanged)
        {
            UpdateMatrix();
        }
        return mRight;
    }

    void Transform::UpdateMatrix()
    {
        mMatrix = glm::translate(glm::mat4(1.0F), mPosition);
        mMatrix = glm::rotate(mMatrix, glm::radians(mRotation.x), glm::vec3(1.0F, 0.0F, 0.0F));
        mMatrix = glm::rotate(mMatrix, glm::radians(mRotation.y), glm::vec3(0.0F, 1.0F, 0.0F));
        mMatrix = glm::rotate(mMatrix, glm::radians(mRotation.z), glm::vec3(0.0F, 0.0F, 1.0F));
        mMatrix = glm::scale(mMatrix, mScale);

        mForward = -glm::normalize(glm::vec3(mMatrix[2][0], mMatrix[2][1], mMatrix[2][2]));
        mUp = glm::normalize(glm::vec3(mMatrix[1][0], mMatrix[1][1], mMatrix[1][2]));
        mRight = glm::normalize(glm::vec3(mMatrix[0][0], mMatrix[0][1], mMatrix[0][2]));

        mChanged = false;
    }

} // namespace nabla2d

// くコ:彡