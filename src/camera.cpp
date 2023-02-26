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
                   const ProjectionSettings &aSettings) : mPosition(aPosition),
                                                          mRotation(aRotation),
                                                          mProjectionSettings(aSettings),
                                                          mNextPosition(aPosition),
                                                          mNextRotation(aRotation),
                                                          mNextProjectionSettings(aSettings)
    {
    }

    const glm::vec3 &Camera::GetPosition() const
    {
        return mPosition;
    }

    const glm::vec3 &Camera::GetRotation() const
    {
        return mRotation;
    }

    void Camera::SetPosition(const glm::vec3 &aPosition)
    {
        mNextPosition = aPosition;
    }

    void Camera::SetRotation(const glm::vec3 &aRotation)
    {
        mNextRotation = aRotation;
    }

    const Camera::ProjectionSettings &Camera::GetProjectionSettings() const
    {
        return mProjectionSettings;
    }

    void Camera::SetProjectionSettings(const Camera::ProjectionSettings &aSettings)
    {
        mNextProjectionSettings = aSettings;
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

    void Camera::Translate(const glm::vec3 &aTranslation)
    {
        mNextPosition += aTranslation;
    }

    void Camera::Rotate(const glm::vec3 &aRotation)
    {
        mNextRotation += aRotation;
    }

    void Camera::LookAt(const glm::vec3 &aPosition)
    {
        const glm::vec3 direction = glm::normalize(aPosition - mPosition);
        mNextRotation.x = glm::degrees(glm::asin(-direction.y));
        mNextRotation.y = glm::degrees(glm::atan(direction.x, direction.z));
    }

    void Camera::Update()
    {
        mPosition = mNextPosition;
        mRotation = mNextRotation;
        mProjectionSettings = mNextProjectionSettings;

        mViewMatrix = glm::mat4(1.0F);
        mViewMatrix = glm::translate(mViewMatrix, mPosition);
        mViewMatrix = glm::rotate(mViewMatrix, glm::radians(mRotation.x), {1.0F, 0.0F, 0.0F});
        mViewMatrix = glm::rotate(mViewMatrix, glm::radians(mRotation.y), {0.0F, 1.0F, 0.0F});
        mViewMatrix = glm::rotate(mViewMatrix, glm::radians(mRotation.z), {0.0F, 0.0F, 1.0F});
        mViewMatrix = glm::inverse(mViewMatrix);

        mProjectionMatrix = glm::perspective(glm::radians(mProjectionSettings.fov),
                                             mProjectionSettings.aspectRatio,
                                             mProjectionSettings.near,
                                             mProjectionSettings.far);

        mProjectionViewMatrix = mProjectionMatrix * mViewMatrix;
    }

} // namespace nabla2d

// くコ:彡