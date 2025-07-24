#ifndef GAME_EXAMPLE_SYSTEMS_HPP
#define GAME_EXAMPLE_SYSTEMS_HPP

#include "ecs/system.hpp"
#include "ecs/world.hpp"
#include "components.hpp"
#include <iostream>
#include <cmath>
#include <vector>

namespace game {
namespace example {

/**
 * @brief System that handles movement by applying velocity to position.
 * Operates on entities with Position and Velocity components.
 */
class MovementSystem : public ecs::System {
    ecs::World* world_;

public:
    explicit MovementSystem(ecs::World* world) : world_(world) {}

    void tick(const float delta) override {
        for (const auto entity : entities_) {
            auto& position = world_->get_component<Position>(entity);
            const auto& velocity = world_->get_component<Velocity>(entity);
            
            // Apply velocity to position
            position.x += velocity.dx * delta;
            position.y += velocity.dy * delta;
        }
    }
};

/**
 * @brief System for rendering entities with sprites.
 * Operates on entities with Position and Sprite components.
 */
class RenderSystem : public ecs::System {
    ecs::World* world_;

public:
    explicit RenderSystem(ecs::World* world) : world_(world) {}

    void tick(const float delta) override {
        for (const auto entity : entities_) {
            const auto& position = world_->get_component<Position>(entity);
            const auto& sprite = world_->get_component<Sprite>(entity);
            
            // In a real game, this would render to the screen
            // For demo purposes, we'll just print entity info occasionally
            static float time_accumulator = 0.0f;
            time_accumulator += delta;
            
            if (time_accumulator >= 2.0f) {  // Print every 2 seconds
                std::cout << "Rendering " << sprite.texture_name 
                         << " at (" << position.x << ", " << position.y << ")\n";
                time_accumulator = 0.0f;
            }
        }
    }
};

/**
 * @brief System that handles player input and controls entities.
 * Operates on entities with Position, Velocity, and PlayerControlled components.
 */
class PlayerInputSystem : public ecs::System {
    ecs::World* world_;

public:
    explicit PlayerInputSystem(ecs::World* world) : world_(world) {}

    void tick(const float delta) override {
        for (const auto entity : entities_) {
            auto& velocity = world_->get_component<Velocity>(entity);
            const auto& player_ctrl = world_->get_component<PlayerControlled>(entity);
            
            // Simple input simulation - in a real game you'd read actual input
            // For demo: make player entities move in a figure-8 pattern
            static float time = 0.0f;
            time += delta;
            
            velocity.dx = std::sin(time) * player_ctrl.move_speed;
            velocity.dy = std::sin(time * 2) * player_ctrl.move_speed * 0.5f;
        }
    }
};

/**
 * @brief System that handles AI behavior for computer-controlled entities.
 * Operates on entities with Position, Velocity, and AIControlled components.
 */
class AISystem : public ecs::System {
    ecs::World* world_;

public:
    explicit AISystem(ecs::World* world) : world_(world) {}

    void tick(const float delta) override {
        for (const auto entity : entities_) {
            const auto& position = world_->get_component<Position>(entity);
            auto& velocity = world_->get_component<Velocity>(entity);
            const auto& ai = world_->get_component<AIControlled>(entity);
            
            // Simple AI: patrol around home position
            float dx = position.x - ai.home_position.x;
            float dy = position.y - ai.home_position.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            
            if (distance > ai.patrol_range) {
                // Return to home position
                velocity.dx = -dx / distance * 50.0f;
                velocity.dy = -dy / distance * 50.0f;
            } else {
                // Random patrol movement
                static float ai_time = 0.0f;
                ai_time += delta;
                velocity.dx = std::cos(ai_time + entity) * 30.0f;  // Use entity ID for variation
                velocity.dy = std::sin(ai_time * 0.7f + entity) * 30.0f;
            }
        }
    }
};

/**
 * @brief System that handles health management and death.
 * Operates on entities with Health component.
 */
class HealthSystem : public ecs::System {
    ecs::World* world_;

public:
    explicit HealthSystem(ecs::World* world) : world_(world) {}

    void tick(const float delta) override {
        for (const auto entity : entities_) {
            const auto& health = world_->get_component<Health>(entity);
            
            if (!health.is_alive()) {
                std::cout << "Entity " << entity << " died and will be removed!\n";
                // In a real game, you might trigger death effects, drop items, etc.
                world_->remove_entity(entity);
                // Note: entity is now invalid, but the loop is safe as long as
                // we don't process more entities in this frame
                break;
            }
        }
    }
};

/**
 * @brief System that manages entity lifetimes and removes expired entities.
 * Operates on entities with Lifetime component.
 */
class LifetimeSystem : public ecs::System {
    ecs::World* world_;

public:
    explicit LifetimeSystem(ecs::World* world) : world_(world) {}

    void tick(const float delta) override {
        for (const auto entity : entities_) {
            auto& lifetime = world_->get_component<Lifetime>(entity);
            
            lifetime.remaining_time -= delta;
            
            if (lifetime.is_expired()) {
                std::cout << "Entity " << entity << " lifetime expired, removing...\n";
                world_->remove_entity(entity);
                break;  // Safe iteration break after removal
            }
        }
    }
};

/**
 * @brief System that handles collision detection between entities.
 * Operates on entities with Position and Collider components.
 */
class CollisionSystem : public ecs::System {
    ecs::World* world_;

public:
    explicit CollisionSystem(ecs::World* world) : world_(world) {}

    void tick(const float delta) override {
        // Simple O(n²) collision detection - in a real game you'd use spatial partitioning
        std::vector<ecs::Entity> colliding_entities(entities_.begin(), entities_.end());
        
        for (size_t i = 0; i < colliding_entities.size(); ++i) {
            for (size_t j = i + 1; j < colliding_entities.size(); ++j) {
                auto entity1 = colliding_entities[i];
                auto entity2 = colliding_entities[j];
                
                const auto& pos1 = world_->get_component<Position>(entity1);
                const auto& pos2 = world_->get_component<Position>(entity2);
                const auto& col1 = world_->get_component<Collider>(entity1);
                const auto& col2 = world_->get_component<Collider>(entity2);
                
                // Check collision (circle-circle)
                float dx = pos1.x - pos2.x;
                float dy = pos1.y - pos2.y;
                float distance = std::sqrt(dx * dx + dy * dy);
                float min_distance = col1.radius + col2.radius;
                
                if (distance < min_distance) {
                    // Collision detected!
                    handleCollision(entity1, entity2);
                }
            }
        }
    }

private:
    void handleCollision(ecs::Entity entity1, ecs::Entity entity2) {
        // Simple collision response - in a real game this would be more sophisticated
        static float last_collision_time = 0.0f;
        static float current_time = 0.0f;
        current_time += 0.016f;  // Assume ~60 FPS for demo
        
        if (current_time - last_collision_time > 1.0f) {  // Avoid spam
            std::cout << "Collision detected between entity " << entity1 
                     << " and entity " << entity2 << "!\n";
            last_collision_time = current_time;
        }
        
        // Example: Handle damage if one entity has Damage component
        if (world_->has_component<Damage>(entity1) && world_->has_component<Health>(entity2)) {
            const auto& damage = world_->get_component<Damage>(entity1);
            auto& health = world_->get_component<Health>(entity2);
            
            health.current -= damage.amount;
            std::cout << "Entity " << entity2 << " took " << damage.amount 
                     << " damage, health: " << health.current << "/" << health.maximum << "\n";
                     
            if (damage.destroy_on_hit) {
                world_->remove_entity(entity1);
            }
        }
        
        // Handle collectibles
        if (world_->has_component<Collectible>(entity1) && world_->has_component<PlayerControlled>(entity2)) {
            const auto& collectible = world_->get_component<Collectible>(entity1);
            std::cout << "Player collected item worth " << collectible.score_value << " points!\n";
            world_->remove_entity(entity1);
        }
    }
};

} // namespace example
} // namespace game

#endif // GAME_EXAMPLE_SYSTEMS_HPP 