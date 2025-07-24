#ifndef GAME_ECS_SYSTEM_MANAGER_HPP
#define GAME_ECS_SYSTEM_MANAGER_HPP

#include "ecs/entity.hpp"
#include "ecs/entity_manager.hpp"
#include "ecs/system.hpp"
#include <cassert>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <unordered_map>

namespace game {
namespace ecs {

/**
 * @brief Manages all systems and their entity associations.
 * 
 * Handles system registration, signature management, and
 * entity-system relationship updates. Maintains the list
 * of entities that each system should operate on.
 */
 class SystemManager {
    std::unordered_map<std::type_index, Signature> signatures_;
    std::unordered_map<std::type_index, std::unique_ptr<System>> systems_;

public:
    /**
     * @brief Called once per-frame.
     * Updates all of the underlying systems registered.
     *
     * @param delta The delta time
     */
    void tick(const float delta) noexcept {
        for (auto& [_, system] : systems_) {
            system->tick(delta);
        }
    }

    /**
     * @brief Called when an entity's signature changes.
     * Updates all systems' entity lists accordingly
     * 
     * @param entity The entity whose signature changed
     * @param entity_signature The entity's new signature
     */
    void entity_signature_changed(const Entity entity, const Signature entity_signature) noexcept {
        for (auto& [index, system] : systems_) {
            const auto& system_signature = signatures_[index];
            if ((entity_signature & system_signature) == system_signature) {
                // Entity signature matches system signature (add to set)
                system->entities_.insert(entity);
            } else {
                // Entity signature does not match system signature (remove from set)
                system->entities_.erase(entity);
            }
        }
    }

    /**
     * @brief Called when an entity is destroyed.
     * Removes the entity from all systems.
     *
     * @param entity The entity that was destroyed
     */
    void entity_destroyed(const Entity entity) noexcept {
        for (auto& [_, system] : systems_) {
            system->entities_.erase(entity);
        }
    }

    template<typename T, typename... Args>
    [[nodiscard]] T& register_system(Args&&... args) noexcept {
        static_assert(std::is_base_of_v<System, T>, "T must inherit System");

        const auto index = std::type_index(typeid(T));
        assert(systems_.find(index) == systems_.end() && "System is already registered");

        auto system = std::make_unique<T>(std::forward<Args>(args)...);
        auto& system_ref = *system;

        systems_.emplace(index, std::move(system));

        return system_ref;
    }

    template<typename T>
    void unregister_system() noexcept {
        static_assert(std::is_base_of_v<System, T>, "T must inherit System");

        const auto index = std::type_index(typeid(T));
        assert(systems_.find(index) != systems_.end() && "System is not registered");

        systems_.erase(index);
        signatures_.erase(index);
    }

    template<typename T>
    [[nodiscard]] T& get_system() noexcept {
        static_assert(std::is_base_of_v<System, T>, "T must inherit System");

        const auto index = std::type_index(typeid(T));
        const auto it = systems_.find(index);
        assert(it != systems_.end() && "System is not registered");

        return static_cast<T&>(*it->second);
    }

    template<typename T>
    [[nodiscard]] const T& get_system() const noexcept {
        static_assert(std::is_base_of_v<System, T>, "T must inherit System");

        const auto index = std::type_index(typeid(T));
        const auto it = systems_.find(index);
        assert(it != systems_.end() && "System is not registered");

        return static_cast<const T&>(*it->second);
    }

    template<typename T>
    [[nodiscard]] T* find_system() noexcept {
        static_assert(std::is_base_of_v<System, T>, "T must inherit System");

        const auto index = std::type_index(typeid(T));
        const auto it = systems_.find(index);
        
        return (it != systems_.end()) ? static_cast<T*>(it->second.get()) : nullptr;
    }

    template<typename T>
    [[nodiscard]] const T* find_system() const noexcept {
        static_assert(std::is_base_of_v<System, T>, "T must inherit System");

        const auto index = std::type_index(typeid(T));
        const auto it = systems_.find(index);
        
        return (it != systems_.end()) ? static_cast<const T*>(it->second.get()) : nullptr;
    }

    template<typename T>
    void set_signature(const Signature signature) noexcept {
        static_assert(std::is_base_of_v<System, T>, "T must inherit System");

        const auto index = std::type_index(typeid(T));
        assert(systems_.find(index) != systems_.end() && "System is not registered");

        signatures_[index] = signature;
    }
 };

}//ecs
}//game

#endif//GAME_ECS_SYSTEM_MANAGER_HPP