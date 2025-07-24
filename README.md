# ECS Architecture Plus

A performance-optimized lightweight C++ Entity-Component-System (ECS) framework designed for game development. This implementation prioritizes performance through data-oriented design while maintaining clean, modern C++ code.

## 🚀 Features

- **🎯 Simple & Clean API** - Intuitive interface with minimal boilerplate
- **🔒 Type Safety** - Template-based design with compile-time type checking
- **💾 Memory Efficient** - Dense component storage and entity recycling
- **⚡ Performance Focused** - Cache-friendly data layout for optimal iteration
- **🛡️ Modern C++** - Uses C++20 features including fold expressions
- **📚 Well Documented** - Comprehensive usage guide and working examples
- **🧪 Robust Architecture** - SOLID principles with clean separation of concerns

## 🏗️ Architecture Overview

The framework consists of four core components:

- **📦 Components** - Pure data structures that define entity properties
- **🎮 Entities** - Lightweight 64-bit IDs that aggregate components  
- **⚙️ Systems** - Logic processors that operate on entities with specific components
- **🌍 World** - Main coordinator that manages entities, components, and systems

## 🔧 Quick Start

### Prerequisites
- C++20 compatible compiler (GCC 10+, Clang 11+, MSVC 2019+)
- CMake 3.20 or higher

### Building
```bash
git clone <your-repo-url>
cd ecs-architecture-plus
mkdir build && cd build
cmake ..
make
```

### Basic Example
```cpp
#include "ecs/world.hpp"
#include <iostream>

using namespace game::ecs;

// Define components as simple data structures
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

// Create a system that processes entities with Position and Velocity
class MovementSystem : public System {
    World* world_;
public:
    explicit MovementSystem(World* world) : world_(world) {}
    
    void tick(const float delta) override {
        for (const auto entity : entities_) {
            auto& pos = world_->get_component<Position>(entity);
            const auto& vel = world_->get_component<Velocity>(entity);
            
            // Apply movement
            pos.x += vel.dx * delta;
            pos.y += vel.dy * delta;
        }
    }
};

int main() {
    // Create ECS world
    World world;
    
    // Register component types
    world.register_component<Position>();
    world.register_component<Velocity>();
    
    // Register system and set its signature
    auto& movement_system = world.register_system<MovementSystem>(&world);
    world.set_system_signature<MovementSystem, Position, Velocity>();
    
    // Create entity with components
    auto entity = world.add_entity();
    world.add_component(entity, Position{0.0f, 0.0f});
    world.add_component(entity, Velocity{10.0f, 5.0f});
    
    // Simulate one frame
    world.tick(1.0f);
    
    // Check result
    const auto& pos = world.get_component<Position>(entity);
    std::cout << "New position: (" << pos.x << ", " << pos.y << ")" << std::endl;
    // Output: New position: (10, 5)
    
    return 0;
}
```

## 📖 Documentation

- **[Complete Usage Guide](ECS_USAGE_GUIDE.md)** - Comprehensive documentation with examples
- **[Demo Project](src/demo/)** - Full working example with multiple systems
- **[API Documentation](#api-overview)** - Core framework interface

## 🎯 Use Cases

This ECS framework is perfect for:

- **🕹️ Real-time Games** - Fast-paced games requiring high performance
- **🎨 Indie Projects** - Flexible architecture for creative game mechanics
- **📚 Learning Projects** - Educational ECS implementation with clear patterns
- **🧪 Prototyping** - Rapid iteration and experimentation
- **🎮 2D/3D Games** - Suitable for any game requiring entity management

## 📁 Project Structure

```
ecs-architecture-plus/
├── src/
│   ├── ecs/                    # Core ECS framework
│   │   ├── world.hpp           # Main ECS coordinator
│   │   ├── entity.hpp          # Entity definitions and constants
│   │   ├── system.hpp          # Base system class
│   │   ├── component_manager.hpp   # Component storage and management
│   │   ├── entity_manager.hpp      # Entity lifecycle management
│   │   ├── system_manager.hpp      # System registration and updates
│   │   └── component_array.hpp     # Dense component storage
│   ├── demo/                   # Complete working example
│   │   ├── main.cpp           # Demo application
│   │   ├── components.hpp     # Example game components
│   │   ├── systems.hpp        # Example game systems
│   │   └── README.md          # Demo documentation
│   └── main.cpp               # Simple test file
├── CMakeLists.txt             # Build configuration
├── README.md                  # This file
└── ECS_USAGE_GUIDE.md        # Comprehensive usage guide
```

## 🎮 Demo Application

Run the included demo to see the framework in action:

```bash
cd build
./ecs_example
```

The demo creates a mini-simulation featuring:
- **Player entity** with figure-8 movement pattern
- **AI enemy** that patrols around its spawn point
- **Collectible items** that can be picked up
- **Projectiles** with limited lifetime
- **Collision detection** and response system

Expected output:
```
=== ECS Framework Example ===

1. Registering components...
2. Registering systems...
3. Setting system signatures...
4. Creating entities...
  Created player entity: 0
  Created enemy entity: 1
  Created coin entity: 2
  Created projectile entity: 3
  Created tree entity: 4

Total entities created: 5

5. Running simulation...
Frame 0 - Entities alive: 5
Rendering player.png at (100.034, 100.423)
Rendering enemy.png at (183.245, 164.851)
...
```

## 🔨 Building Your Game

### 1. Define Components
```cpp
struct Health {
    int current{100};
    int maximum{100};
    bool is_alive() const { return current > 0; }
};

struct Transform {
    float x{0.0f}, y{0.0f}, rotation{0.0f};
};
```

### 2. Create Systems
```cpp
class HealthSystem : public ecs::System {
    ecs::World* world_;
public:
    explicit HealthSystem(ecs::World* world) : world_(world) {}
    
    void tick(const float delta) override {
        for (const auto entity : entities_) {
            const auto& health = world_->get_component<Health>(entity);
            if (!health.is_alive()) {
                world_->remove_entity(entity);
                break;
            }
        }
    }
};
```

### 3. Set Up World
```cpp
// Register everything
world.register_component<Health>();
world.register_component<Transform>();

auto& health_system = world.register_system<HealthSystem>(&world);
world.set_system_signature<HealthSystem, Health>();
```

### 4. Create Entities
```cpp
auto player = world.add_entity();
world.add_component(player, Health{100, 100});
world.add_component(player, Transform{100.0f, 100.0f, 0.0f});
```

### 5. Game Loop
```cpp
while (game_running) {
    float delta = get_delta_time();
    world.tick(delta);
}
```

## ⚡ Performance Features

- **Dense Component Storage** - Components stored in contiguous arrays for cache efficiency
- **Entity Recycling** - Reuses entity IDs to minimize memory fragmentation  
- **Bitset Signatures** - Fast entity-system matching using bitwise operations
- **Move Semantics** - Efficient component transfers with minimal copying
- **Template Optimization** - Compile-time type safety with zero runtime overhead

## 🔧 API Overview

### World Management
```cpp
World world;                                    // Create ECS world
world.tick(delta_time);                        // Update all systems
std::uint64_t count = world.get_entity_count(); // Get active entity count
```

### Entity Operations
```cpp
Entity entity = world.add_entity();           // Create entity
world.remove_entity(entity);                  // Remove entity and all components
```

### Component Management
```cpp
world.register_component<ComponentType>();    // Register component type
world.add_component(entity, Component{...});  // Add component to entity
auto& comp = world.get_component<ComponentType>(entity); // Get component
bool has = world.has_component<ComponentType>(entity);   // Check if has component
world.remove_component<ComponentType>(entity); // Remove component
```

### System Management
```cpp
auto& system = world.register_system<SystemType>(&world); // Register system
world.set_system_signature<SystemType, Comp1, Comp2>(); // Set required components
```

## 📊 Framework Specifications

- **Entity Type**: `std::uint64_t` (64-bit integers)
- **Maximum Entities**: 5,000 simultaneous
- **Maximum Component Types**: 32 unique types
- **Signature System**: `std::bitset<32>` for efficient matching
- **Language Standard**: C++20 with fold expressions
- **Memory Model**: RAII with move semantics
- **Threading**: Single-threaded (systems run sequentially)

## 🧪 Testing

The framework includes comprehensive examples:

1. **Basic Test** - Simple entity creation and movement
2. **Demo Application** - Full game simulation with multiple systems
3. **Performance Tests** - Stress testing with many entities

## 🤝 Contributing

This is an educational project, but contributions are welcome:

- **Bug Reports** - Found an issue? Please report it
- **Performance Improvements** - Optimizations are always appreciated  
- **Documentation** - Help improve examples and guides
- **Feature Requests** - Suggest new functionality

## 🌟 Acknowledgments

This ECS implementation is inspired by:
- Modern game engine architectures
- Data-oriented design principles  
- Clean code and SOLID design patterns
- Educational ECS resources and tutorials

Built for learning and practical game development.

## License

> Copyright © 2025 Tyler R. Suehr

This project is provided as-is for educational and personal use.
