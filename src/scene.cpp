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

#include "scene.hpp"

#include "logger.hpp"

namespace nabla2d
{
    Scene::Scene()
    {
        mParents[entt::null] = entt::null;
    }

    entt::entity Scene::CreateEntity(const std::string &aTag, entt::entity aParent)
    {
        if (aTag == mErrorTag)
        {
            Logger::error("Cannot create entity with reserved tag '{}'!", aTag);
            return entt::null;
        }
        if (mTags.find(aTag) != mTags.end())
        {
            Logger::error("Cannot create entity with the tag '{}', one with the same tag alread exists!", aTag);
            return entt::null;
        }

        auto entity = mRegistry.create();
        mTags[aTag] = entity;
        mReverseTags[entity] = aTag;
        mParents[entity] = aParent;

        BuildEntityTree();

        return entity;
    }

    void Scene::DestroyEntity(entt::entity aEntity)
    {
        if (aEntity == entt::null)
        {
            Logger::error("Cannot delete null entity!");
            return;
        }

        if (!mRegistry.valid(aEntity))
        {
            Logger::error("Cannot delete entity, it doesn't appear to be valid!");
        }

        mRegistry.destroy(aEntity);
        std::string tag = mReverseTags.at(aEntity);
        mTags.erase(tag);
        mReverseTags.erase(aEntity);
        mParents.erase(aEntity);

        BuildEntityTree();
    }

    const entt::registry &Scene::GetRegistry() const
    {
        return mRegistry;
    }

    entt::entity Scene::GetEntity(const std::string &aTag) const
    {
        auto entity = mTags.find(aTag);
        if (entity == mTags.end())
        {
            Logger::error("No entity with the tag '{}' exists!", aTag);
            return entt::null;
        }
        return entity->second;
    }

    const std::string &Scene::GetTag(entt::entity aEntity) const
    {
        auto tag = mReverseTags.find(aEntity);
        if (tag == mReverseTags.end())
        {
            Logger::error("No entity with the specified id exists!");
            return mErrorTag;
        }
        return tag->second;
    }

    entt::entity Scene::GetParent(entt::entity aEntity) const
    {
        auto parent = mParents.find(aEntity);
        if (parent == mParents.end())
        {
            Logger::error("No entity with the specified id exists!");
            return entt::null;
        }
        return parent->second;
    }

    void Scene::SetParent(entt::entity aEntity, entt::entity aParent)
    {
        auto entity = mParents.find(aEntity);
        if (entity == mParents.end())
        {
            Logger::error("No entity with the specified id exists!");
            return;
        }
        entity->second = aParent;

        BuildEntityTree();
    }

    const Scene::EntityTree &Scene::GetEntityTree() const
    {
        return mEntityTree;
    }

    void Scene::BuildEntityTree()
    {
        // TODO
    }

} // namespace nabla2d

// くコ:彡