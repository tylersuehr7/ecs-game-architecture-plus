#ifndef GAME_ECS_WORLD_HPP
#define GAME_ECS_WORLD_HPP

#include "ecs/component_manager.hpp"
#include "ecs/entity.hpp"
#include "ecs/entity_manager.hpp"
#include "ecs/system_manager.hpp"

namespace game::ecs {

/**
 * @brief Main ECS coordinator that manages entities, components, and systems.
 * 
 * Provides a unified interface for ECS operations and ensures
 * all managers are kept in sync. Acts as the main entry point
 * for all ECS functionality in the game.
 */
class World {
    ComponentManager component_manager_;
    EntityManager entity_manager_;
    SystemManager system_manager_;

public:
    /**
     * @brief Called once per-frame.
     * @param delta Time elapsed since last frame
     */
    void tick(const float delta) noexcept {
        system_manager_.tick(delta);
    }

    /**
     * @brief Creates a new entity.
     * @return The new entity ID
     */
    Entity add_entity() noexcept {
        return entity_manager_.add_entity();
    }

    /**
     * @brief Removes an entity and all its components.
     * @param entity The entity to remove
     */
    void remove_entity(const Entity entity) noexcept {
        entity_manager_.remove_entity(entity);
        component_manager_.entity_destroyed(entity);
        system_manager_.entity_destroyed(entity);
    }

    /**
     * @brief Registers a component type with the ECS.
     * @tparam T The component type to register
     */
    template<typename T>
    void register_component() noexcept {
        component_manager_.register_component_array<T>();
    }

    /**
     * @brief Adds a component to an entity.
     * @tparam T The component type
     * @param entity The entity to add the component to
     * @param component The component data
     */
    template<typename T>
    void add_component(const Entity entity, T component) noexcept {
        component_manager_.add_component<T>(entity, std::move(component));

        auto signature = entity_manager_.get_signature(entity);
        signature.set(component_manager_.get_component_type<T>(), true);

        entity_manager_.set_signature(entity, signature);
        system_manager_.entity_signature_changed(entity, signature);
    }

    /**
     * @brief Removes a component from an entity.
     * @tparam T The component type
     * @param entity The entity to remove the component from
     */
    template<typename T>
    void remove_component(const Entity entity) noexcept {
        component_manager_.remove_component<T>(entity);

        auto signature = entity_manager_.get_signature(entity);
        signature.set(component_manager_.get_component_type<T>(), false);

        entity_manager_.set_signature(entity, signature);
        system_manager_.entity_signature_changed(entity, signature);
    }

    /**
     * @brief Gets a component for an entity.
     * @tparam T The component type
     * @param entity The entity to get the component for
     * @return Reference to the component
     */
    template<typename T>
    [[nodiscard]] T& get_component(const Entity entity) noexcept {
        return component_manager_.get_component<T>(entity);
    }

    /**
     * @brief Gets a component for an entity (const version).
     * @tparam T The component type
     * @param entity The entity to get the component for
     * @return Const reference to the component
     */
    template<typename T>
    [[nodiscard]] const T& get_component(const Entity entity) const noexcept {
        return component_manager_.get_component<T>(entity);
    }

    /**
     * @brief Checks if an entity has a component.
     * @tparam T The component type
     * @param entity The entity to check
     * @return True if the entity has the component
     */
    template<typename T>
    [[nodiscard]] bool has_component(const Entity entity) const noexcept {
        return component_manager_.has_component<T>(entity);
    }

    /**
     * @brief Registers a system with the ECS.
     * @tparam T The system type to register
     * @return Reference to the registered system
     */
    template<typename T, typename... Args>
    [[nodiscard]] T& register_system(Args&&... args) noexcept {
        return system_manager_.register_system<T>(std::forward<Args>(args)...);
    }

    /**
     * @brief Sets the signature for a system using component types.
     * 
     * This is a convenience method that automatically creates a signature
     * based on the provided component types and sets it for the specified system.
     * 
     * @tparam SystemT The system type
     * @tparam ComponentTypes The component types required by the system
     */
    template<typename SystemT, typename... ComponentTypes>
    void set_system_signature() noexcept {
        Signature signature;
        ((signature.set(component_manager_.get_component_type<ComponentTypes>(), true)), ...);
        system_manager_.set_signature<SystemT>(signature);
    }

    /**
     * @brief Creates a component signature from the specified component types.
     * 
     * Utility method that generates a signature with bits set for each
     * component type provided. Used for creating signatures for queries
     * or system requirements.
     * 
     * @tparam ComponentTypes The component types to include in the signature
     * @return A signature with bits set for the specified component types
     */
    template<typename... ComponentTypes>
    [[nodiscard]] Signature make_signature() const noexcept {
        Signature signature;
        ((signature.set(component_manager_.get_component_type<ComponentTypes>(), true)), ...);
        return signature;
    }

    /**
      * @brief Gets the current number of active entities.
      * @return Number of active entities
      */
    [[nodiscard]] std::uint64_t get_entity_count() const noexcept {
        return entity_manager_.get_living_entity_count();
    }
};

}

#endif//GAME_ECS_WORLD_HPP