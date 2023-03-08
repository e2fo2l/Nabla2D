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

#include "camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace nabla2d
{
    Camera::Camera(const glm::vec3 &aPosition,
                   const glm::vec3 &aRotation,
                   const ProjectionSettings &aSettings) : mTransform(aPosition, aRotation, {1.0F, 1.0F, 1.0F}),
                                                          mNextTransform(mTransform),
                                                          mProjectionSettings(aSettings),
                                                          mNextProjectionSettings(aSettings),
                                                          mViewMatrix(1.0F),
                                                          mProjectionMatrix(1.0F),
                                                          mProjectionViewMatrix(1.0F)
    {
        Update();
    }

    const glm::vec3 &Camera::GetPosition() const
    {
        return mTransform.GetPosition();
    }

    const glm::vec3 &Camera::GetRotation() const
    {
        return mTransform.GetRotation();
    }

    const glm::quat &Camera::GetRotationQuat() const
    {
        return mTransform.GetRotationQuat();
    }

    void Camera::SetPosition(const glm::vec3 &aPosition)
    {
        mNextTransform.SetPosition(aPosition);
        mTransformChanged = true;
    }

    void Camera::SetRotation(const glm::vec3 &aRotation)
    {
        mNextTransform.SetRotation(aRotation);
        mTransformChanged = true;
    }

    void Camera::SetRotation(const glm::quat &aRotation)
    {
        mNextTransform.SetRotation(aRotation);
        mTransformChanged = true;
    }

    void Camera::Translate(const glm::vec3 &aTranslation)
    {
        mNextTransform.Translate(aTranslation);
        mTransformChanged = true;
    }

    void Camera::Rotate(float aAngle, const glm::vec3 &aAxis)
    {
        mNextTransform.Rotate(aAngle, aAxis);
        mTransformChanged = true;
    }

    void Camera::LookAt(const glm::vec3 &aPosition)
    {
        mNextTransform.LookAt(aPosition);
        mTransformChanged = true;
    }

    const Camera::ProjectionSettings &Camera::GetProjectionSettings() const
    {
        return mProjectionSettings;
    }

    void Camera::SetProjectionSettings(const ProjectionSettings &aSettings)
    {
        mNextProjectionSettings = aSettings;
        mProjectionSettingsChanged = true;
    }

    const glm::vec3 &Camera::GetForward() const
    {
        return mForward;
    }

    const glm::vec3 &Camera::GetUp() const
    {
        return mUp;
    }

    const glm::vec3 &Camera::GetRight() const
    {
        return mRight;
    }

    const glm::mat4 &Camera::GetViewMatrix() const
    {
        return mViewMatrix;
    }

    const glm::mat4 &Camera::GetProjectionMatrix() const
    {
        return mProjectionMatrix;
    }

    const glm::mat4 &Camera::GetProjectionViewMatrix() const
    {
        return mProjectionViewMatrix;
    }

    void Camera::Update()
    {
        if (mTransformChanged)
        {
            mTransform = mNextTransform;
            mViewMatrix = glm::inverse(mTransform.GetMatrix());
            mForward = mTransform.GetForward();
            mUp = mTransform.GetUp();
            mRight = mTransform.GetRight();
        }

        if (mProjectionSettingsChanged)
        {
            mProjectionSettings = mNextProjectionSettings;
            mProjectionMatrix = glm::perspective(mProjectionSettings.fov, mProjectionSettings.aspectRatio, mProjectionSettings.near, mProjectionSettings.far);
        }

        if (mTransformChanged || mProjectionSettingsChanged)
        {
            mProjectionViewMatrix = mProjectionMatrix * mViewMatrix;
        }

        mTransformChanged = false;
        mProjectionSettingsChanged = false;
    }

} // namespace nabla2d

// くコ:彡