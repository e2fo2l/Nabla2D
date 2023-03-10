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

#ifndef NABLA2D_EDITOR_HPP
#define NABLA2D_EDITOR_HPP

#include <array>
#include <vector>
#include <glm/glm.hpp>
#include "camera.hpp"
#include "scene.hpp"
#include "transform.hpp"
#include "renderer/renderer.hpp"

namespace nabla2d
{
    class Editor
    {
    public:
        Editor() = default;
        ~Editor() = default;

        void Init(Renderer *aRenderer);
        void Destroy(Renderer *aRenderer);

        void Update(float aDeltaTime, float aTime, Renderer *aRenderer, Camera &aCamera);

        void DrawGrid(Renderer *aRenderer, Camera &aCamera);
        void DrawGUI(Renderer *aRenderer, Camera &aCamera, Scene &aScene);

    private:
        Renderer::ShaderHandle mGridShader;
        Renderer::DataHandle mGridData;
        Renderer::DataHandle mSubgridData;
        Renderer::DataHandle mAxisData;
        Transform mGridTransform;
        Transform mSubgridTransform;
        Transform mAxisTransform;

        bool mIs3Dmode{false};

        bool mIsTransitioningMode{false};
        float mTransitionStartTime{0.0F};
        float mTransitionDuration{0.0F};
        Transform mTransitionStart;
        Transform mTransitionEnd;

        std::array<float, 256> mFPSs;
        float mAverageFPS{0.0F};
        float mDeltaTime{0.0F};
        float mTime{0.0F};

        glm::vec3 mCameraTarget{0.0F, 0.0F, 0.0F};

        entt::entity mSelectedEntity{entt::null};

        void UpdateInput(Renderer *aRenderer, Camera &aCamera);
        void UpdateInput2D(Renderer *aRenderer, Camera &aCamera, float aMovementSpeed);
        void UpdateInput3D(Renderer *aRenderer, Camera &aCamera, float aMovementSpeed);

        void GUIVec3Widget(const std::string &aTitle, glm::vec3 &aVec, float aSpeed = 0.02F, const std::string &aXLabel = "x", const std::string &aYLabel = "y", const std::string &aZLabel = "z");

        void GUIBeginCornerWindow(int aCorner = 0);
        void GUIDrawFPSWindow(Renderer *aRenderer);
        void GUIDrawCameraWindow(Camera &aCamera);
        void GUIDraw2D32Window(Camera &aCamera);
        void GUIDrawEntitiesWindow(Scene &aScene);
        void GUIDrawEntityNode(Scene &aScene, Scene::EntityNode &aNode, std::vector<std::pair<entt::entity, entt::entity>> &aNewParents);

        static std::vector<glm::vec3> GetGridVertices(float aSize, int aSlices);
        static std::vector<unsigned int> GetGridIndices(int aSlices);
    };
} // namespace nabla2d

#endif // NABLA2D_EDITOR_HPP

// くコ:彡