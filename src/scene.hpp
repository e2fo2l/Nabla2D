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

#ifndef NABLA2D_SCENE_HPP
#define NABLA2D_SCENE_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <entt/entt.hpp>

namespace nabla2d
{
    class Scene
    {
    public:
        struct EntityNode
        {
            entt::entity entity;
            entt::entity parent;
            std::vector<EntityNode> children;
        };
        typedef EntityNode EntityTree;

        Scene();
        ~Scene() = default;

        entt::entity CreateEntity(const std::string &aTag, entt::entity aParent = entt::null);
        void DestroyEntity(entt::entity aEntity);

        const entt::registry &GetRegistry() const;
        entt::entity GetEntity(const std::string &aTag) const;
        const std::string &GetTag(entt::entity aEntity) const;

        entt::entity GetParent(entt::entity aEntity) const;
        void SetParent(entt::entity aEntity, entt::entity aParent);

        const EntityTree &GetEntityTree() const;

    private:
        const std::string mErrorTag{""};
        const std::string mRootTag{"Root"};

        entt::registry mRegistry;
        std::unordered_map<std::string, entt::entity> mTags;
        std::unordered_map<entt::entity, std::string> mReverseTags;
        std::unordered_map<entt::entity, entt::entity> mParents;
        std::unordered_map<entt::entity, std::vector<entt::entity>> mChildren;

        EntityTree mEntityTree;

        void BuildEntityTree();
        void BuildEntityTreeRecursive(EntityNode &aNode, entt::entity aEntity);
    };

} // namespace nabla2d

#endif // NABLA2D_SCENE_HPP

// くコ:彡