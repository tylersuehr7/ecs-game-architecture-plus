# ECS Framework Examples

This directory contains comprehensive examples demonstrating how to use the ECS (Entity-Component-System) framework.

## Files Overview

- **`components.hpp`** - Example component definitions for common game objects
- **`systems.hpp`** - Example systems that operate on entities with specific components  
- **`main.cpp`** - Complete working example that ties everything together
- **`README.md`** - This documentation file

## Running the Examples

### Prerequisites
- C++20 compatible compiler
- CMake 3.20 or higher

### Building and Running
```bash
# From the project root directory
mkdir build && cd build
cmake ..
make

# Run the example
./ecs_example
```

## What You'll See

The example creates a mini simulation with:

- **Player Entity** - Moves in a figure-8 pattern, can take damage
- **AI Enemy** - Patrols around its spawn point, deals damage on collision
- **Collectible Coin** - Can be picked up by the player for points
- **Projectile** - Moves in a straight line, damages enemies, disappears after 3 seconds
- **Static Tree** - Decorative entity that doesn't move or interact

The simulation runs for about 5 seconds, demonstrating:
- Real-time entity movement and AI behavior
- Collision detection and response
- Entity lifetime management
- Dynamic entity creation and destruction

## Core ECS Concepts Demonstrated

### 1. Components (Data)
Components are pure data structures that define entity properties:

```cpp
struct Position {
    float x{0.0f};
    float y{0.0f};
};

struct Velocity {
    float dx{0.0f};
    float dy{0.0f};
};

struct Health {
    int current{100};
    int maximum{100};
};
```

### 2. Systems (Logic)
Systems contain logic that operates on entities with specific component combinations:

```cpp
class MovementSystem : public ecs::System {
public:
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
```

### 3. Entities (Identifiers)
Entities are lightweight IDs that tie components together:

```cpp
auto player = world.add_entity();
world.add_component(player, Position{100.0f, 100.0f});
world.add_component(player, Velocity{0.0f, 0.0f});
world.add_component(player, Health{100, 100});
```

### 4. System Signatures (Component Requirements)
Systems only process entities that have all required components:

```cpp
// MovementSystem only processes entities with BOTH Position AND Velocity
world.set_system_signature<MovementSystem, Position, Velocity>();

// RenderSystem only processes entities with BOTH Position AND Sprite
world.set_system_signature<RenderSystem, Position, Sprite>();
```

## Usage Patterns

### Basic ECS Setup

```cpp
// 1. Create world
World world;

// 2. Register component types
world.register_component<Position>();
world.register_component<Velocity>();

// 3. Register systems
auto& movement_system = world.register_system<MovementSystem>(&world);

// 4. Set system signatures (which components each system needs)
world.set_system_signature<MovementSystem, Position, Velocity>();

// 5. Create entities and add components
auto entity = world.add_entity();
world.add_component(entity, Position{10.0f, 20.0f});
world.add_component(entity, Velocity{1.0f, 0.5f});

// 6. Run simulation loop
while (running) {
    world.tick(delta_time);
}
```

### Component Composition Examples

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

**Player Character** (Position + Velocity + Sprite + Health + PlayerControlled):
```cpp
auto player = world.add_entity();
world.add_component(player, Position{100, 100});
world.add_component(player, Velocity{0, 0});
world.add_component(player, Sprite{"player.png"});
world.add_component(player, Health{100, 100});
world.add_component(player, PlayerControlled{80.0f});
// Processed by: MovementSystem, RenderSystem, PlayerInputSystem, HealthSystem
```

## System Design Patterns

### 1. Transform System (Movement)
Applies velocity to position each frame:
```cpp
position.x += velocity.dx * delta;
position.y += velocity.dy * delta;
```

### 2. Input System (Player Control)
Reads input and modifies velocity:
```cpp
if (input.left_pressed) velocity.dx = -player_ctrl.move_speed;
if (input.right_pressed) velocity.dx = player_ctrl.move_speed;
```

### 3. AI System (Computer Control)
Implements AI behaviors:
```cpp
if (distance_to_home > ai.patrol_range) {
    // Return to home
    velocity = normalize(home_position - position) * speed;
} else {
    // Patrol randomly
    velocity = random_direction() * speed;
}
```

### 4. Collision System (Interaction)
Detects and responds to entity interactions:
```cpp
if (entities_overlapping(entity1, entity2)) {
    if (has_damage_component(entity1)) {
        apply_damage(entity2, damage_amount);
    }
}
```

### 5. Lifetime System (Cleanup)
Manages temporary entities:
```cpp
lifetime.remaining_time -= delta;
if (lifetime.remaining_time <= 0) {
    world.remove_entity(entity);
}
```

## Benefits of This Architecture

### 🔧 **Composition over Inheritance**
- Mix and match components to create different entity types
- No deep inheritance hierarchies
- Easy to add new behaviors by combining existing components

### ⚡ **Performance**
- Cache-friendly data layout (components stored contiguously)
- Systems only process relevant entities
- No virtual function calls during component access

### 🔄 **Flexibility**
- Add/remove components at runtime
- Systems automatically track relevant entities
- Easy to prototype new game mechanics

### 🧩 **Modularity**
- Components are reusable across different entity types
- Systems are independent and can be easily added/removed
- Clear separation of data (components) and logic (systems)

### 🐛 **Debugging**
- Easy to inspect entity composition
- Systems can be disabled individually for testing
- Clear data flow from components through systems

## Extending the Framework

### Adding New Components
```cpp
struct Inventory {
    std::vector<ItemType> items;
    int capacity{10};
};

// Register in main:
world.register_component<Inventory>();
```

### Adding New Systems
```cpp
class InventorySystem : public ecs::System {
public:
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

### Creating Entity Archetypes
```cpp
// Helper functions for common entity types
Entity create_player(World& world, float x, float y) {
    auto entity = world.add_entity();
    world.add_component(entity, Position{x, y});
    world.add_component(entity, Velocity{0, 0});
    world.add_component(entity, Sprite{"player.png"});
    world.add_component(entity, Health{100, 100});
    world.add_component(entity, PlayerControlled{80.0f});
    world.add_component(entity, Collider{16.0f});
    return entity;
}
```

This example demonstrates the power and flexibility of the ECS architecture for game development! 