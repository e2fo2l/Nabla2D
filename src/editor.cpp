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

#include "editor.hpp"

#include <cmath>
#include <string>
#include "logger.hpp"

constexpr const char *kGridVertexShader{R"(
#version 330 core

uniform mat4 u_ModelViewProjectionMatrix;
layout (location = 0) in vec3 a_Position;

void main()
{
    gl_Position = u_ModelViewProjectionMatrix * vec4(a_Position, 1.0f);
}

)"};

constexpr const char *kGridFragmentShader{R"(
#version 330 core

uniform vec4 u_Color;
out vec4 FragColor;

void main()
{
    FragColor = u_Color;
}

)"};

namespace nabla2d
{
    void Editor::Init(Renderer *aRenderer)
    {
        mGridShader = aRenderer->LoadShader(kGridVertexShader, kGridFragmentShader);

        mGridData = aRenderer->LoadDataLines(GetGridVertices(1.0F, 50), GetGridIndices(50));
        mSubgridData = aRenderer->LoadDataLines(GetGridVertices(1.0F, 500), GetGridIndices(500));
        mAxisData = aRenderer->LoadDataLines({{0.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F}}, {0, 1});

        mGridTransform = Transform({0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F}, {50.0F, 50.0F, 50.0F});
        mSubgridTransform = Transform({0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F}, {50.0F, 50.0F, 50.0F});
        mAxisTransform = Transform({0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F}, {10000.0F, 10000.0F, 10000.0F});
    }

    void Editor::Update(float aDeltaTime, float aTime)
    {
        (void)aDeltaTime;
        (void)aTime;
    }

    void Editor::Render(Renderer *aRenderer, Camera &aCamera)
    {
        auto cameraPosition = aCamera.GetPosition();
        auto cameraRotation = aCamera.GetRotation();

        aRenderer->UseShader(mGridShader);
        auto drawParameters = Renderer::DrawParameters();
        drawParameters.lineWidth = 1.0F;

        const float scaleIndex = std::max(std::floor(std::log2(std::abs(cameraPosition.z) / 10.0F) + 1.0F), 1.0F);
        const float scaleFactor = std::pow(2.0F, scaleIndex);
        mSubgridTransform.SetScale({scaleFactor * 50.0F, scaleFactor * 50.0F, 1.0F});
        mGridTransform.SetScale({scaleFactor * 50.0F, scaleFactor * 50.0F, 1.0F});

        mSubgridTransform.SetPosition({std::floor(cameraPosition.x / scaleFactor) * scaleFactor, std::floor(cameraPosition.y / scaleFactor) * scaleFactor, 0.0F});
        mGridTransform.SetPosition({std::floor(cameraPosition.x / scaleFactor) * scaleFactor, std::floor(cameraPosition.y / scaleFactor) * scaleFactor, 0.0F});

        const float gridOffset = (cameraPosition.z > 0.0F) ? -0.005F / scaleFactor : 0.005F / scaleFactor;
        mSubgridTransform.Translate({0.0F, 0.0F, gridOffset});
        mGridTransform.Translate({0.0F, 0.0F, gridOffset});
        mAxisTransform.Translate({0.0F, 0.0F, gridOffset * 0.75F});

        // Subgrid
        drawParameters.color = {0.45F, 0.45F, 0.45F, 1.0F};
        aRenderer->DrawData(mSubgridData, aCamera, mSubgridTransform.GetMatrix(), drawParameters);

        // Grid
        drawParameters.color = {0.125F, 0.125F, 0.125F, 1.0F};
        aRenderer->DrawData(mGridData, aCamera, mGridTransform.GetMatrix(), drawParameters);

        // Axes
        drawParameters.color = {1.0F, 0.0F, 0.0F, 1.0F};
        aRenderer->DrawData(mAxisData, aCamera, mAxisTransform.GetMatrix(), drawParameters);
        mAxisTransform.Rotate(1.0F, {0.0F, 0.0F, 90.0F});
        drawParameters.color = {0.0F, 1.0F, 0.0F, 1.0F};
        aRenderer->DrawData(mAxisData, aCamera, mAxisTransform.GetMatrix(), drawParameters);
        mAxisTransform.Rotate(1.0F, {90.0F, 0.0F, 0.0F});

        if (cameraRotation.x != 0.0F || cameraRotation.y != 0.0F)
        {
            drawParameters.color = {0.0F, 0.0F, 1.0F, 1.0F};
            aRenderer->DrawData(mAxisData, aCamera, mAxisTransform.GetMatrix(), drawParameters);
        }

        mAxisTransform.SetRotation({0.0F, 0.0F, 0.0F});
        mSubgridTransform.Translate({0.0F, 0.0F, -gridOffset});
        mGridTransform.Translate({0.0F, 0.0F, -gridOffset});
        mAxisTransform.Translate({0.0F, 0.0F, -gridOffset * 0.75});
    }

    void Editor::Destroy(Renderer *aRenderer)
    {
        aRenderer->DeleteData(mGridData);
        aRenderer->DeleteData(mSubgridData);
        aRenderer->DeleteData(mAxisData);
        aRenderer->DeleteShader(mGridShader);
    }

    std::vector<glm::vec3> Editor::GetGridVertices(float aSize, int aSlices)
    {
        std::vector<glm::vec3> vertices;
        vertices.reserve((aSlices + 1) * 4);

        for (int i = 0; i <= aSlices; i++)
        {
            const float x = (static_cast<float>(i) * aSize / static_cast<float>(aSlices)) - aSize / 2.0F;
            vertices.emplace_back(x, -aSize / 2.0F, 0.0F);
            vertices.emplace_back(x, aSize / 2.0F, 0.0F);
        }

        for (int i = 0; i <= aSlices; i++)
        {
            const float y = (static_cast<float>(i) * aSize / static_cast<float>(aSlices)) - aSize / 2.0F;
            vertices.emplace_back(-aSize / 2.0F, y, 0.0F);
            vertices.emplace_back(aSize / 2.0F, y, 0.0F);
        }

        return vertices;
    }

    std::vector<unsigned int> Editor::GetGridIndices(int aSlices)
    {
        std::vector<unsigned int> indices;
        indices.reserve((aSlices + 1) * 4);

        for (int i = 0; i <= aSlices; ++i)
        {
            indices.push_back(i * 2);
            indices.push_back(i * 2 + 1);
        }

        for (int i = 0; i <= aSlices; ++i)
        {
            indices.push_back((aSlices + 1) * 2 + i * 2);
            indices.push_back((aSlices + 1) * 2 + i * 2 + 1);
        }

        return indices;
    }

} // namespace nabla2d

// くコ:彡