#ifndef GAME_ECS_COMPONENT_ARRAY_HPP
#define GAME_ECS_COMPONENT_ARRAY_HPP

#include "entity.hpp"
#include <array>
#include <cassert>
#include <cstddef>
#include <unordered_map>
#include <utility>

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

/**
 * @brief Dense component storage for a specific component type.
 * 
 * Uses dense array storage for cache efficiency. Maps entities
 * to array indices for O(1) component access while maintaining
 * contiguous memory layout for optimal iteration performance.
 *
 * Includes iterator support.
 */
template<typename T>
class ComponentArray : public IComponentArray {
    std::array<T, MAX_ENTITIES> components_{};
    std::unordered_map<Entity, std::size_t> entity_to_index_{};
    std::unordered_map<std::size_t, Entity> index_to_entity_{};
    std::size_t size_{0};

public:
    // Type aliases for iterator support
    using iterator = typename std::array<T, MAX_ENTITIES>::iterator;
    using const_iterator = typename std::array<T, MAX_ENTITIES>::const_iterator;

    void insert(const Entity entity, T component) noexcept {
        assert(entity_to_index_.find(entity) == entity_to_index_.end() && "Component already exists for entity");
        assert(size_ < MAX_ENTITIES && "Component array is full");

        // Put new entry at end and update all mappings
        const std::size_t new_index = size_++;
        entity_to_index_[entity] = new_index;
        index_to_entity_[new_index] = entity;
        components_[new_index] = std::move(component);
    }

    void remove(const Entity entity) noexcept {
        assert(entity_to_index_.find(entity) != entity_to_index_.end() && "Component does not exist for entity");

        const std::size_t index_of_removed_entity = entity_to_index_[entity];
        const std::size_t index_of_last_element = size_ - 1;
        
        // Only move if we're not removing the last element
        if (index_of_removed_entity != index_of_last_element) {
            // Move element at end into deleted element's place to maintain density
            components_[index_of_removed_entity] = std::move(components_[index_of_last_element]);
            
            // Update map to point to moved spot
            const Entity entity_of_last_element = index_to_entity_[index_of_last_element];
            entity_to_index_[entity_of_last_element] = index_of_removed_entity;
            index_to_entity_[index_of_removed_entity] = entity_of_last_element;
        }

        entity_to_index_.erase(entity);
        index_to_entity_.erase(index_of_last_element);

        --size_;
    }

    const T& get(const Entity entity) const noexcept {
        assert(entity_to_index_.find(entity) != entity_to_index_.end() && "Component does not exist for entity");
        return components_[entity_to_index_.at(entity)];
    }

    T& get(const Entity entity) noexcept {
        assert(entity_to_index_.find(entity) != entity_to_index_.end() && "Component does not exist for entity");
        return components_[entity_to_index_[entity]];
    }

    bool has(const Entity entity) const noexcept {
        return entity_to_index_.find(entity) != entity_to_index_.end();
    }

    // Iterator support for range-based for loops
    iterator begin() noexcept {
        return components_.begin();
    }

    const_iterator begin() const noexcept {
        return components_.begin();
    }

    const_iterator cbegin() const noexcept {
        return components_.cbegin();
    }

    iterator end() noexcept {
        return components_.begin() + size_;
    }

    const_iterator end() const noexcept {
        return components_.begin() + size_;
    }

    const_iterator cend() const noexcept {
        return components_.cbegin() + size_;
    }

    // Raw data access (for compatibility)
    const T* data() const noexcept {
        return components_.data();
    }

    T* data() noexcept {
        return components_.data();
    }

    std::size_t size() const noexcept {
        return size_;
    }

    void entity_destroyed(const Entity entity) override {
        if (entity_to_index_.find(entity) != entity_to_index_.end()) {
            remove(entity);
        }
    }
};

}//ecs
}//game

#endif//GAME_ECS_COMPONENT_ARRAY_HPP