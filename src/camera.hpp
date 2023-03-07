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

#ifndef NABLA2D_CAMERA_HPP
#define NABLA2D_CAMERA_HPP

#include <glm/glm.hpp>
#include "transform.hpp"

namespace nabla2d
{
    class Camera
    {
    public:
        typedef struct
        {
            float fov;
            float aspectRatio;
            float near;
            float far;
        } ProjectionSettings;

        explicit Camera(const glm::vec3 &aPosition = {0.0F, 0.0F, 0.0F},
                        const glm::vec3 &aRotation = {0.0F, 0.0F, 0.0F},
                        const ProjectionSettings &aSettings = {
                            45.0F, 16.0F / 9.0F, 0.1F, 100.0F});
        ~Camera() = default;

        const glm::vec3 &GetPosition() const;
        const glm::vec3 &GetRotation() const;

        void SetPosition(const glm::vec3 &aPosition);
        void SetRotation(const glm::vec3 &aRotation);

        void Translate(const glm::vec3 &aTranslation);
        void Rotate(float aAngle, const glm::vec3 &aAxis);
        void LookAt(const glm::vec3 &aPosition);

        const ProjectionSettings &GetProjectionSettings() const;
        void SetProjectionSettings(const ProjectionSettings &aSettings);

        const glm::mat4 &GetViewMatrix() const;
        const glm::mat4 &GetProjectionMatrix() const;
        const glm::mat4 &GetProjectionViewMatrix() const;

        void Update();

    private:
        Transform mTransform;
        Transform mNextTransform;
        bool mTransformChanged{true};

        ProjectionSettings mProjectionSettings;
        ProjectionSettings mNextProjectionSettings;
        bool mProjectionSettingsChanged{true};

        glm::mat4 mViewMatrix;
        glm::mat4 mProjectionMatrix;
        glm::mat4 mProjectionViewMatrix;
    };
} // namespace nabla2d

#endif // NABLA2D_CAMERA_HPP

// くコ:彡