# ECS Framework Usage Guide

## Overview

This ECS (Entity Component System) framework provides a high-performance, cache-friendly architecture for game development. It follows SOLID design principles and clean code practices to ensure maintainable and efficient code.

## Key Features

- **Performance-Optimized**: Dense component storage for cache efficiency with contiguous memory layout
- **Type-Safe**: Template-based component and system management with compile-time type checking
- **Clean Architecture**: Clear separation between data (components) and behavior (systems)
- **Memory Efficient**: Entity recycling and move semantics for optimal performance
- **Scalable**: Supports up to 5,000 entities and 32 component types
- **Modern C++**: Uses C++20 features including fold expressions and perfect forwarding

## Architecture Overview

The framework consists of four core components:

- **📦 Components** - Pure data structures that define entity properties
- **🎮 Entities** - Lightweight integer IDs that aggregate components
- **⚙️ Systems** - Logic processors that operate on entities with specific components
- **🌍 World** - Main coordinator that manages all entities, components, and systems

## Core Concepts

### 1. Entities

Entities are simple 64-bit integer IDs that serve as unique identifiers:

```cpp
#include "ecs/world.hpp"

using namespace game::ecs;

// Create an entity
Entity player = world.add_entity();

// Remove an entity (also removes all its components)
world.remove_entity(player);

// Check entity count
std::cout << "Active entities: " << world.get_entity_count() << std::endl;
```

**Entity Limits:**
- Maximum entities: 5,000 simultaneous
- Entity type: `std::uint64_t`
- Invalid entity constant: `INVALID_ENTITY`

### 2. Components

Components are pure data structures (POD) that store entity properties:

```cpp
// Define components as simple structs
struct Position {
    float x{0.0f};
    float y{0.0f};
    
    Position() = default;
    Position(float x, float y) : x(x), y(y) {}
};

struct Health {
    int current{100};
    int maximum{100};
    
    Health() = default;
    Health(int current, int maximum) : current(current), maximum(maximum) {}
    
    bool is_alive() const noexcept { return current > 0; }
};

struct Velocity {
    float dx{0.0f};
    float dy{0.0f};
    
    Velocity() = default;
    Velocity(float dx, float dy) : dx(dx), dy(dy) {}
};
```

**Component Operations:**

```cpp
// 1. Register component types (must be done before use)
world.register_component<Position>();
world.register_component<Health>();
world.register_component<Velocity>();

// 2. Add components to entities
auto entity = world.add_entity();
world.add_component(entity, Position{100.0f, 200.0f});
world.add_component(entity, Health{80, 100});
world.add_component(entity, Velocity{5.0f, -2.0f});

// 3. Get components from entities
auto& position = world.get_component<Position>(entity);
const auto& health = world.get_component<Health>(entity);

// 4. Check if entity has component
if (world.has_component<Health>(entity)) {
    // Entity has health component
}

// 5. Remove components from entities
world.remove_component<Velocity>(entity);
```

**Component Limits:**
- Maximum component types: 32
- Components use move semantics for efficiency
- Components are stored in dense arrays for cache performance

### 3. Systems

Systems contain game logic and operate on entities with specific component combinations:

```cpp
#include "ecs/system.hpp"
#include "ecs/world.hpp"

// Systems inherit from ecs::System
class MovementSystem : public ecs::System {
    ecs::World* world_;

public:
    // Constructor receives world pointer
    explicit MovementSystem(ecs::World* world) : world_(world) {}

    // Implement tick method for game logic
    void tick(const float delta) override {
        // Process all entities that match this system's signature
        for (const auto entity : entities_) {
            auto& position = world_->get_component<Position>(entity);
            const auto& velocity = world_->get_component<Velocity>(entity);
            
            // Apply velocity to position
            position.x += velocity.dx * delta;
            position.y += velocity.dy * delta;
        }
    }
};

class HealthSystem : public ecs::System {
    ecs::World* world_;

public:
    explicit HealthSystem(ecs::World* world) : world_(world) {}

    void tick(const float delta) override {
        for (const auto entity : entities_) {
            const auto& health = world_->get_component<Health>(entity);
            
            // Remove dead entities
            if (!health.is_alive()) {
                std::cout << "Entity " << entity << " died!\n";
                world_->remove_entity(entity);
                break; // Safe to break after entity removal
            }
        }
    }
};
```

**System Registration and Configuration:**

```cpp
// Register systems with the world
auto& movement_system = world.register_system<MovementSystem>(&world);
auto& health_system = world.register_system<HealthSystem>(&world);

// Set system signatures (which components each system requires)
world.set_system_signature<MovementSystem, Position, Velocity>();
world.set_system_signature<HealthSystem, Health>();
```

**System Features:**
- Systems automatically receive entities that match their signature
- `entities_` member contains all matching entities as `std::set<Entity>`
- Systems are updated every frame via `world.tick(delta)`
- Type-safe component access through world pointer

### 4. System Signatures

System signatures define which components a system requires using variadic templates:

```cpp
// MovementSystem processes entities with BOTH Position AND Velocity
world.set_system_signature<MovementSystem, Position, Velocity>();

// RenderSystem processes entities with BOTH Position AND Sprite
world.set_system_signature<RenderSystem, Position, Sprite>();

// PlayerInputSystem requires Position, Velocity, AND PlayerControlled
world.set_system_signature<PlayerInputSystem, Position, Velocity, PlayerControlled>();

// HealthSystem only requires Health component
world.set_system_signature<HealthSystem, Health>();
```

**Signature Matching:**
- Entities are automatically added/removed from systems when components change
- Systems only process entities that have ALL required components
- Adding/removing components triggers signature updates
- Uses efficient bitset operations for matching

## Complete Example: Building a Simple Game

```cpp
#include "ecs/world.hpp"
#include <iostream>

using namespace game::ecs;

// Define components
struct Position {
    float x{0.0f}, y{0.0f};
    Position() = default;
    Position(float x, float y) : x(x), y(y) {}
};

struct Velocity {
    float dx{0.0f}, dy{0.0f};
    Velocity() = default;
    Velocity(float dx, float dy) : dx(dx), dy(dy) {}
};

struct Health {
    int current{100}, maximum{100};
    Health() = default;
    Health(int current, int maximum) : current(current), maximum(maximum) {}
    bool is_alive() const { return current > 0; }
};

// Define systems
class MovementSystem : public ecs::System {
    ecs::World* world_;
public:
    explicit MovementSystem(ecs::World* world) : world_(world) {}
    
    void tick(const float delta) override {
        for (const auto entity : entities_) {
            auto& pos = world_->get_component<Position>(entity);
            const auto& vel = world_->get_component<Velocity>(entity);
            
            pos.x += vel.dx * delta;
            pos.y += vel.dy * delta;
        }
    }
};

class HealthSystem : public ecs::System {
    ecs::World* world_;
public:
    explicit HealthSystem(ecs::World* world) : world_(world) {}
    
    void tick(const float delta) override {
        for (const auto entity : entities_) {
            const auto& health = world_->get_component<Health>(entity);
            
            if (!health.is_alive()) {
                std::cout << "Entity " << entity << " died!\n";
                world_->remove_entity(entity);
                break;
            }
        }
    }
};

int main() {
    // Create ECS world
    World world;
    
    // Step 1: Register components
    world.register_component<Position>();
    world.register_component<Velocity>();
    world.register_component<Health>();
    
    // Step 2: Register systems
    auto& movement_system = world.register_system<MovementSystem>(&world);
    auto& health_system = world.register_system<HealthSystem>(&world);
    
    // Step 3: Set system signatures
    world.set_system_signature<MovementSystem, Position, Velocity>();
    world.set_system_signature<HealthSystem, Health>();
    
    // Step 4: Create entities
    auto player = world.add_entity();
    world.add_component(player, Position{100.0f, 100.0f});
    world.add_component(player, Velocity{10.0f, 5.0f});
    world.add_component(player, Health{100, 100});
    
    auto enemy = world.add_entity();
    world.add_component(enemy, Position{200.0f, 150.0f});
    world.add_component(enemy, Velocity{-5.0f, 0.0f});
    world.add_component(enemy, Health{50, 50});
    
    // Step 5: Game loop
    const float delta_time = 1.0f / 60.0f; // 60 FPS
    
    for (int frame = 0; frame < 300; ++frame) { // 5 seconds at 60 FPS
        // Update all systems
        world.tick(delta_time);
        
        // Print status every second
        if (frame % 60 == 0) {
            std::cout << "Frame " << frame << " - Entities: " 
                     << world.get_entity_count() << std::endl;
        }
    }
    
    return 0;
}
```

## Component Composition Patterns

### Basic Entity Types

**Moving Object** (Position + Velocity):
```cpp
auto moving_entity = world.add_entity();
world.add_component(moving_entity, Position{0, 0});
world.add_component(moving_entity, Velocity{10, 5});
// Processed by: MovementSystem
```

**Renderable Object** (Position + Sprite):
```cpp
auto visual_entity = world.add_entity();
world.add_component(visual_entity, Position{50, 100});
world.add_component(visual_entity, Sprite{"texture.png"});
// Processed by: RenderSystem
```

**Player Character** (Multiple Components):
```cpp
auto player = world.add_entity();
world.add_component(player, Position{100, 100});
world.add_component(player, Velocity{0, 0});
world.add_component(player, Health{100, 100});
world.add_component(player, PlayerControlled{80.0f});
world.add_component(player, Sprite{"player.png"});
// Processed by: MovementSystem, HealthSystem, PlayerInputSystem, RenderSystem
```

## Advanced System Patterns

### 1. Input System
```cpp
class PlayerInputSystem : public ecs::System {
    ecs::World* world_;
public:
    explicit PlayerInputSystem(ecs::World* world) : world_(world) {}
    
    void tick(const float delta) override {
        for (const auto entity : entities_) {
            auto& velocity = world_->get_component<Velocity>(entity);
            const auto& player_ctrl = world_->get_component<PlayerControlled>(entity);
            
            // Reset velocity
            velocity.dx = velocity.dy = 0.0f;
            
            // Apply input (this is simulated - use actual input in real games)
            // if (IsKeyDown(KEY_W)) velocity.dy = -player_ctrl.move_speed;
            // if (IsKeyDown(KEY_S)) velocity.dy = player_ctrl.move_speed;
            // if (IsKeyDown(KEY_A)) velocity.dx = -player_ctrl.move_speed;
            // if (IsKeyDown(KEY_D)) velocity.dx = player_ctrl.move_speed;
        }
    }
};
```

### 2. Collision System
```cpp
class CollisionSystem : public ecs::System {
    ecs::World* world_;
public:
    explicit CollisionSystem(ecs::World* world) : world_(world) {}
    
    void tick(const float delta) override {
        // Convert set to vector for nested iteration
        std::vector<Entity> entity_list(entities_.begin(), entities_.end());
        
        for (size_t i = 0; i < entity_list.size(); ++i) {
            for (size_t j = i + 1; j < entity_list.size(); ++j) {
                auto entity1 = entity_list[i];
                auto entity2 = entity_list[j];
                
                const auto& pos1 = world_->get_component<Position>(entity1);
                const auto& pos2 = world_->get_component<Position>(entity2);
                const auto& col1 = world_->get_component<Collider>(entity1);
                const auto& col2 = world_->get_component<Collider>(entity2);
                
                // Simple circle collision
                float dx = pos1.x - pos2.x;
                float dy = pos1.y - pos2.y;
                float distance = std::sqrt(dx * dx + dy * dy);
                
                if (distance < col1.radius + col2.radius) {
                    handleCollision(entity1, entity2);
                }
            }
        }
    }
    
private:
    void handleCollision(Entity entity1, Entity entity2) {
        // Handle collision response
        std::cout << "Collision between " << entity1 << " and " << entity2 << std::endl;
    }
};
```

### 3. Lifetime Management System
```cpp
class LifetimeSystem : public ecs::System {
    ecs::World* world_;
public:
    explicit LifetimeSystem(ecs::World* world) : world_(world) {}
    
    void tick(const float delta) override {
        for (const auto entity : entities_) {
            auto& lifetime = world_->get_component<Lifetime>(entity);
            
            lifetime.remaining_time -= delta;
            
            if (lifetime.remaining_time <= 0.0f) {
                std::cout << "Entity " << entity << " expired\n";
                world_->remove_entity(entity);
                break; // Safe to break after removal
            }
        }
    }
};
```

## Game Loop Integration

```cpp
void game_loop() {
    World world;
    
    // Setup phase
    setup_components(world);
    setup_systems(world);
    create_initial_entities(world);
    
    // Main loop
    const float target_fps = 60.0f;
    const float frame_time = 1.0f / target_fps;
    
    auto last_time = std::chrono::high_resolution_clock::now();
    
    while (game_running) {
        auto current_time = std::chrono::high_resolution_clock::now();
        float delta = std::chrono::duration<float>(current_time - last_time).count();
        
        // Cap delta time
        if (delta > frame_time * 2.0f) {
            delta = frame_time;
        }
        
        // Update all systems
        world.tick(delta);
        
        // Handle rendering, input, etc.
        render_game();
        
        last_time = current_time;
    }
}
```

## Performance Considerations

### 1. Component Design
- **Keep components small**: Only store data, no behavior
- **Use efficient data types**: Prefer POD types when possible
- **Avoid dynamic allocations**: Use fixed-size arrays or pre-allocated vectors

### 2. System Optimization
- **Minimize component access**: Cache frequently accessed components
- **Use const references**: For read-only component access
- **Batch operations**: Process entities in groups when possible

### 3. Memory Layout
- Components are stored in dense arrays for cache efficiency
- Entity recycling minimizes memory fragmentation
- Move semantics reduce unnecessary copying

## Extending the Framework

### Adding New Components
```cpp
struct Inventory {
    std::array<ItemType, 10> items{};
    size_t item_count{0};
    
    bool is_full() const { return item_count >= items.size(); }
    bool add_item(ItemType item) {
        if (is_full()) return false;
        items[item_count++] = item;
        return true;
    }
};

// Register in main:
world.register_component<Inventory>();
```

### Adding New Systems
```cpp
class InventorySystem : public ecs::System {
    ecs::World* world_;
public:
    explicit InventorySystem(ecs::World* world) : world_(world) {}
    
    void tick(const float delta) override {
        for (const auto entity : entities_) {
            auto& inventory = world_->get_component<Inventory>(entity);
            // Inventory management logic...
        }
    }
};

// Register and configure:
auto& inventory_system = world.register_system<InventorySystem>(&world);
world.set_system_signature<InventorySystem, Inventory>();
```

### Entity Factory Pattern
```cpp
class EntityFactory {
    World* world_;
public:
    explicit EntityFactory(World* world) : world_(world) {}
    
    Entity create_player(float x, float y) {
        auto entity = world_->add_entity();
        world_->add_component(entity, Position{x, y});
        world_->add_component(entity, Velocity{0, 0});
        world_->add_component(entity, Health{100, 100});
        world_->add_component(entity, PlayerControlled{80.0f});
        world_->add_component(entity, Sprite{"player.png"});
        return entity;
    }
    
    Entity create_enemy(float x, float y) {
        auto entity = world_->add_entity();
        world_->add_component(entity, Position{x, y});
        world_->add_component(entity, Velocity{0, 0});
        world_->add_component(entity, Health{50, 50});
        world_->add_component(entity, AIControlled{});
        world_->add_component(entity, Sprite{"enemy.png"});
        return entity;
    }
};
```

## Framework Specifications

- **Entity Type**: `std::uint64_t`
- **Maximum Entities**: 5,000 simultaneous
- **Maximum Component Types**: 32
- **Signature Type**: `std::bitset<32>`
- **Entity Storage**: Recycling queue for efficiency
- **Component Storage**: Dense arrays with entity-to-index mapping
- **System Storage**: `std::set<Entity>` for automatic sorting
- **Language Standard**: C++20
- **Memory Model**: Move semantics with RAII

## Demo Project

The framework includes a comprehensive demo in `src/demo/` that demonstrates:

- **Multiple entity types**: Player, AI enemies, collectibles, projectiles
- **Complex interactions**: Collision detection, damage systems, lifetime management
- **Real-time simulation**: 60 FPS game loop with delta time
- **Component composition**: Different combinations creating varied behaviors

To run the demo:
```bash
mkdir build && cd build
cmake ..
make
./ecs_example
```

This ECS framework provides a robust foundation for building high-performance games while maintaining clean, maintainable code that follows modern C++ best practices! 