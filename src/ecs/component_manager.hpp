#ifndef GAME_ECS_COMPONENT_MANAGER_HPP
#define GAME_ECS_COMPONENT_MANAGER_HPP

#include "ecs/component_array.hpp"
#include "ecs/entity.hpp"
#include <cassert>
#include <cstddef>
#include <memory>
#include <typeindex>
#include <unordered_map>

namespace game {
namespace ecs {

/**
 * @brief Type alias for component type IDs.
 *
 * This is needed so that sequential IDs (that represent each Component)
 * can be stored in the signature bitset appropriately.
 */
 using ComponentType = std::size_t;

/**
 * @brief Manages all component types and their storage.
 * 
 * Provides type-safe registration and access to component arrays.
 * Uses template metaprogramming to maintain type safety while
 * allowing runtime component management.
 */
class ComponentManager {
    std::unordered_map<std::type_index, ComponentType> component_types_{};
    std::unordered_map<std::type_index, std::unique_ptr<IComponentArray>> component_arrays_{};
    ComponentType next_sequenced_component_type_{0};

public:
    template<typename T>
    void register_component_array() noexcept {
        const auto index = std::type_index(typeid(T));
        assert(component_types_.find(index) == component_types_.end() && "Component type already registered");
        component_types_[index] = next_sequenced_component_type_++;
        component_arrays_[index] = std::make_unique<ComponentArray<T>>();
    }

    template<typename T>
    const ComponentType get_component_type() const noexcept {
        const auto index = std::type_index(typeid(T));
        const auto it = component_types_.find(index);
        assert(it != component_types_.end() && "Component type not registered");
        return it->second;
    }

    template<typename T>
    void add_component(const Entity entity, T component) noexcept {
        get_component_array<T>()->insert(entity, std::move(component));
    }

    template<typename T>
    void remove_component(const Entity entity) noexcept {
        get_component_array<T>()->remove(entity);
    }

    template<typename T>
    [[nodiscard]] const T& get_component(const Entity entity) const noexcept {
        return get_component_array<T>()->get(entity);
    }

    template<typename T>
    [[nodiscard]] T& get_component(const Entity entity) noexcept {
        return get_component_array<T>()->get(entity);
    }

    template<typename T>
    bool has_component(const Entity entity) const noexcept {
        return get_component_array<T>()->has(entity);
    }

    void entity_destroyed(const Entity entity) noexcept {
        for (auto& [_, array] : component_arrays_) {
            array->entity_destroyed(entity);
        }
    }

private:
    template<typename T>
    ComponentArray<T>* get_component_array() noexcept {
        const auto index = std::type_index(typeid(T));
        assert(component_types_.find(index) != component_types_.end() && "Component type not registered");
        return static_cast<ComponentArray<T>*>(component_arrays_.at(index).get());
    }

    template<typename T>
    const ComponentArray<T>* get_component_array() const noexcept {
        const auto index = std::type_index(typeid(T));
        assert(component_types_.find(index) != component_types_.end() && "Component type not registered");
        return static_cast<const ComponentArray<T>*>(component_arrays_.at(index).get());
    }
};

}//ecs
}//game

#endif//GAME_ECS_COMPONENT_MANAGER_HPP