#ifndef GAME_ECS_ENTITY_MANAGER_HPP
#define GAME_ECS_ENTITY_MANAGER_HPP

#include "entity.hpp"
#include <bitset>
#include <cstdint>
#include <queue>
#include <array>
#include <cassert>

namespace game {
namespace ecs {

/**
 * @brief Maximum number of different component types.
 */
constexpr std::size_t MAX_COMPONENTS = 32;

/**
 * @brief Signature represents which components an entity has.
 * 
 * Uses a bitset for efficient storage and operations.
 * Each bit represents whether an entity has a specific component type.
 */
using Signature = std::bitset<MAX_COMPONENTS>;

/**
 * @brief Manages entity creation, destruction, and signatures.
 * 
 * Maintains entity lifecycles and tracks which components
 * each entity has through signatures. Uses entity recycling
 * for efficient memory usage.
 */
class EntityManager {
    std::queue<Entity> available_entities_{};
    std::array<Signature, MAX_ENTITIES> signatures_{};
    std::uint64_t living_entity_count_{0};

public:
    EntityManager() {
        initialize_queue_with_all_possible_entities();
    }

    [[nodiscard]] Entity add_entity() noexcept {
        assert(living_entity_count_ < MAX_ENTITIES && "Too many entities exist");

        // Take an ID from the front of the queue
        const Entity new_id = available_entities_.front();
        available_entities_.pop();
        ++living_entity_count_;

        return new_id;
    }

    void remove_entity(const Entity entity) noexcept {
        assert(entity < MAX_ENTITIES && "Entity out-of-range");

        // Invalidate the destroyed entity's signature
        signatures_[entity].reset();

        // Put the destroyed ID at the back of the queue for reuse
        available_entities_.push(entity);
        --living_entity_count_;
    }

    void set_signature(const Entity entity, Signature signature) noexcept {
        assert(entity < MAX_ENTITIES && "Entity out-of-range");
        signatures_[entity] = signature;
    }

    const Signature& get_signature(const Entity entity) const noexcept {
        assert(entity < MAX_ENTITIES && "Entity out-of-range");
        return signatures_[entity]; 
    }

    Signature& get_signature(const Entity entity) noexcept {
        assert(entity < MAX_ENTITIES && "Entity out-of-range");
        return signatures_[entity];
    }

    std::uint64_t get_living_entity_count() const noexcept {
        return living_entity_count_;
    }

private:
    inline void initialize_queue_with_all_possible_entities() noexcept {
        for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
            available_entities_.push(entity);
        }
    }
};

}//ecs
}//game

#endif//GAME_ECS_ENTITY_MANAGER_HPP