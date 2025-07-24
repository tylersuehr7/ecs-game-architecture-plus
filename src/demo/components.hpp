#ifndef GAME_EXAMPLE_COMPONENTS_HPP
#define GAME_EXAMPLE_COMPONENTS_HPP

#include <string>

namespace game {
namespace example {

/**
 * @brief Component representing 2D position in the game world.
 */
struct Position {
    float x{0.0f};
    float y{0.0f};
    
    Position() = default;
    Position(float x, float y) : x(x), y(y) {}
};

/**
 * @brief Component representing 2D velocity for movement.
 */
struct Velocity {
    float dx{0.0f};
    float dy{0.0f};
    
    Velocity() = default;
    Velocity(float dx, float dy) : dx(dx), dy(dy) {}
};

/**
 * @brief Component for visual representation of entities.
 */
struct Sprite {
    std::string texture_name;
    float width{32.0f};
    float height{32.0f};
    
    Sprite() = default;
    explicit Sprite(const std::string& texture, float w = 32.0f, float h = 32.0f) 
        : texture_name(texture), width(w), height(h) {}
};

/**
 * @brief Component for entity health/hit points.
 */
struct Health {
    int current{100};
    int maximum{100};
    
    Health() = default;
    Health(int current, int maximum) : current(current), maximum(maximum) {}
    
    [[nodiscard]] bool is_alive() const noexcept { return current > 0; }
    [[nodiscard]] float health_percentage() const noexcept { 
        return maximum > 0 ? static_cast<float>(current) / maximum : 0.0f; 
    }
};

/**
 * @brief Component for player-controlled entities.
 */
struct PlayerControlled {
    float move_speed{100.0f};
    
    PlayerControlled() = default;
    explicit PlayerControlled(float speed) : move_speed(speed) {}
};

/**
 * @brief Component for AI-controlled entities.
 */
struct AIControlled {
    float patrol_range{200.0f};
    float detection_radius{150.0f};
    Position home_position{0.0f, 0.0f};
    
    AIControlled() = default;
    AIControlled(float patrol, float detection, Position home) 
        : patrol_range(patrol), detection_radius(detection), home_position(home) {}
};

/**
 * @brief Component for entities that deal damage on collision.
 */
struct Damage {
    int amount{10};
    bool destroy_on_hit{true};
    
    Damage() = default;
    Damage(int dmg, bool destroy = true) : amount(dmg), destroy_on_hit(destroy) {}
};

/**
 * @brief Component for entities with limited lifetime.
 */
struct Lifetime {
    float remaining_time{5.0f};
    
    Lifetime() = default;
    explicit Lifetime(float time) : remaining_time(time) {}
    
    [[nodiscard]] bool is_expired() const noexcept { return remaining_time <= 0.0f; }
};

/**
 * @brief Component for collectible items.
 */
struct Collectible {
    int score_value{10};
    std::string pickup_sound{"coin"};
    
    Collectible() = default;
    Collectible(int value, const std::string& sound) : score_value(value), pickup_sound(sound) {}
};

/**
 * @brief Component for physics collision detection.
 */
struct Collider {
    float radius{16.0f};
    bool is_trigger{false};  // If true, doesn't block movement but still detects collisions
    
    Collider() = default;
    Collider(float r, bool trigger = false) : radius(r), is_trigger(trigger) {}
};

} // namespace example
} // namespace game

#endif // GAME_EXAMPLE_COMPONENTS_HPP 