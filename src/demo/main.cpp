#include "ecs/world.hpp"
#include "components.hpp"
#include "systems.hpp"
#include <iostream>
#include <chrono>
#include <thread>

using namespace game::ecs;
using namespace game::example;

int main() {
    std::cout << "=== ECS Framework Example ===\n\n";

    // Create the ECS world
    World world;

    // Step 1: Register all component types
    std::cout << "1. Registering components...\n";
    world.register_component<Position>();
    world.register_component<Velocity>();
    world.register_component<Sprite>();
    world.register_component<Health>();
    world.register_component<PlayerControlled>();
    world.register_component<AIControlled>();
    world.register_component<Damage>();
    world.register_component<Lifetime>();
    world.register_component<Collectible>();
    world.register_component<Collider>();

    // Step 2: Register and configure systems
    std::cout << "2. Registering systems...\n";
    auto& movement_system = world.register_system<MovementSystem>(&world);
    auto& render_system = world.register_system<RenderSystem>(&world);
    auto& player_input_system = world.register_system<PlayerInputSystem>(&world);
    auto& ai_system = world.register_system<AISystem>(&world);
    auto& health_system = world.register_system<HealthSystem>(&world);
    auto& lifetime_system = world.register_system<LifetimeSystem>(&world);
    auto& collision_system = world.register_system<CollisionSystem>(&world);

    // Step 3: Set system signatures (which components each system requires)
    std::cout << "3. Setting system signatures...\n";
    
    // Each system signature is defined by the component types it requires
    world.set_system_signature<MovementSystem, Position, Velocity>();
    world.set_system_signature<RenderSystem, Position, Sprite>();
    world.set_system_signature<PlayerInputSystem, Position, Velocity, PlayerControlled>();
    world.set_system_signature<AISystem, Position, Velocity, AIControlled>();
    world.set_system_signature<HealthSystem, Health>();
    world.set_system_signature<LifetimeSystem, Lifetime>();
    world.set_system_signature<CollisionSystem, Position, Collider>();

    // Step 4: Create entities with different component combinations
    std::cout << "4. Creating entities...\n";

    // Player entity - moves in figure-8 pattern
    auto player = world.add_entity();
    world.add_component(player, Position{100.0f, 100.0f});
    world.add_component(player, Velocity{0.0f, 0.0f});
    world.add_component(player, Sprite{"player.png", 32, 32});
    world.add_component(player, Health{100, 100});
    world.add_component(player, PlayerControlled{80.0f});
    world.add_component(player, Collider{16.0f});
    std::cout << "  Created player entity: " << player << "\n";

    // AI enemy - patrols around its spawn point
    auto enemy = world.add_entity();
    world.add_component(enemy, Position{200.0f, 150.0f});
    world.add_component(enemy, Velocity{0.0f, 0.0f});
    world.add_component(enemy, Sprite{"enemy.png", 24, 24});
    world.add_component(enemy, Health{50, 50});
    world.add_component(enemy, AIControlled{100.0f, 80.0f, Position{200.0f, 150.0f}});
    world.add_component(enemy, Collider{12.0f});
    world.add_component(enemy, Damage{25, false});
    std::cout << "  Created enemy entity: " << enemy << "\n";

    // Collectible coin - gives points when collected
    auto coin = world.add_entity();
    world.add_component(coin, Position{150.0f, 200.0f});
    world.add_component(coin, Sprite{"coin.png", 16, 16});
    world.add_component(coin, Collectible{50, "coin_pickup.wav"});
    world.add_component(coin, Collider{8.0f, true});  // Trigger collider
    std::cout << "  Created coin entity: " << coin << "\n";

    // Temporary projectile - damages enemies and disappears after 3 seconds
    auto projectile = world.add_entity();
    world.add_component(projectile, Position{80.0f, 80.0f});
    world.add_component(projectile, Velocity{120.0f, 60.0f});
    world.add_component(projectile, Sprite{"bullet.png", 8, 8});
    world.add_component(projectile, Damage{15, true});
    world.add_component(projectile, Lifetime{3.0f});
    world.add_component(projectile, Collider{4.0f});
    std::cout << "  Created projectile entity: " << projectile << "\n";

    // Static decoration - only has position and sprite (no systems will process it)
    auto tree = world.add_entity();
    world.add_component(tree, Position{300.0f, 250.0f});
    world.add_component(tree, Sprite{"tree.png", 48, 64});
    std::cout << "  Created tree entity: " << tree << "\n";

    std::cout << "\nTotal entities created: " << world.get_entity_count() << "\n\n";

    // Step 5: Run the simulation
    std::cout << "5. Running simulation...\n";
    std::cout << "   (Press Ctrl+C to stop)\n\n";

    const float target_fps = 60.0f;
    const float frame_time = 1.0f / target_fps;
    auto last_time = std::chrono::high_resolution_clock::now();
    
    int frame_count = 0;
    const int max_frames = 300;  // Run for ~5 seconds at 60 FPS

    while (frame_count < max_frames) {
        auto current_time = std::chrono::high_resolution_clock::now();
        float delta = std::chrono::duration<float>(current_time - last_time).count();
        
        // Cap delta time to prevent large jumps
        if (delta > frame_time * 2.0f) {
            delta = frame_time;
        }

        // Update all systems
        world.tick(delta);

        // Print frame info every second
        if (frame_count % 60 == 0) {
            std::cout << "Frame " << frame_count << " - Entities alive: " 
                     << world.get_entity_count() << "\n";
        }

        // Sleep to maintain target framerate
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        
        last_time = current_time;
        frame_count++;
    }

    std::cout << "\n=== Simulation Complete ===\n";
    std::cout << "Final entity count: " << world.get_entity_count() << "\n";
    std::cout << "Total frames processed: " << frame_count << "\n";

    std::cout << "\n=== Example Summary ===\n";
    std::cout << "This example demonstrated:\n";
    std::cout << "• Component registration and type management\n";
    std::cout << "• System registration with different component requirements\n";
    std::cout << "• Entity creation with various component combinations\n";
    std::cout << "• Real-time simulation loop with multiple systems\n";
    std::cout << "• Dynamic entity management (creation/destruction)\n";
    std::cout << "• Component interaction (collision, damage, lifetime)\n\n";

    return 0;
} 