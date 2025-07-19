#ifndef GAME_ECS_COMPONENT_ARRAY_HPP
#define GAME_ECS_COMPONENT_ARRAY_HPP

#include "entity.hpp"

namespace game {
namespace ecs {

/**
 * @brief Interface for type-erased component arrays.
 * 
 * Provides a common interface for different component types
 * while maintaining type safety through templates.
 */
class IComponentArray {
public:
    virtual ~IComponentArray() = default;
    virtual void entity_destroyed(const Entity entity) = 0;
};

}//ecs
}//game

#endif//GAME_ECS_COMPONENT_ARRAY_HPP