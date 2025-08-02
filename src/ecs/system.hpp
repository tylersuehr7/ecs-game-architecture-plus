#ifndef GAME_ECS_SYSTEM_HPP
#define GAME_ECS_SYSTEM_HPP

#include "entity.hpp"
#include <set>

namespace game::ecs {

/**
 * @brief Base class for all systems in the ECS.
 * 
 * Systems contain the logic that operates on entities with
 * specific component combinations. They are notified when
 * entities are added or removed from their interest set.
 */
struct System {
    /**
     * @brief Set of entities that match this system's signature.
     */
    std::set<Entity> entities_;

    virtual ~System() = default;

    /**
     * @brief Called once per frame to update the system.
     * @param delta Time elapsed since last frame
     */
    virtual void tick(const float delta) = 0;
};

}

#endif//GAME_ECS_SYSTEM_HPP