#ifndef GAME_ECS_ENTITY_HPP
#define GAME_ECS_ENTITY_HPP

#include <cstdint>
#include <limits>

namespace game::ecs {

/**
 * @brief Entity represents a unique identifier in the ECS system.
 * 
 * Entities are simple integer IDs that serve as keys to associate
 * components together. This design keeps entities lightweight and
 * enables efficient storage and lookup.
 */
using Entity = std::uint64_t;

/**
 * @brief Invalid entity constant representing no entity.
 */
constexpr Entity INVALID_ENTITY = std::numeric_limits<Entity>::max();

/**
 * @brief Maximum number of entities that can exist simultaneously.
 */
 constexpr std::uint64_t MAX_ENTITIES = 5000;

}

#endif//GAME_ECS_ENTITY_HPP