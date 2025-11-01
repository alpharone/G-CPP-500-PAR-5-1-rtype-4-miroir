#include "PhysicsSystem.hpp"
#include "Logger.hpp"
#include <algorithm>

void System::PhysicsSystem::init(Ecs::Registry&)
{
    Logger::info("[PhysicsSystem] initialized");
}

void System::PhysicsSystem::update(Ecs::Registry& registry, double dt)
{
    auto& positions = registry.getComponents<Component::position_t>();
    auto& velocities = registry.getComponents<Component::velocity_t>();
    auto& gravities = registry.getComponents<Component::gravity_t>();
    auto& frictions = registry.getComponents<Component::friction_t>();

    for (size_t i = 0; i < positions.size(); ++i) {
        if (!positions[i].has_value() || !velocities[i].has_value())
            continue;

        auto& pos = positions[i].value();
        auto& vel = velocities[i].value();

        if (i < gravities.size() && gravities[i].has_value()) {
            float g = gravities[i].value().strength;
            vel.vy += g * dt;
        }

        if (i < frictions.size() && frictions[i].has_value()) {
            float drag = std::clamp(frictions[i].value().drag, 0.f, 1.f);
            vel.vx *= (1.0f - drag * dt);
            vel.vy *= (1.0f - drag * dt);
        }

        pos.x += vel.vx * dt;
        pos.y += vel.vy * dt;
        pos.x = std::clamp(pos.x, 0.0f, 1920.0f);
        pos.y = std::clamp(pos.y, 0.0f, 1080.0f);
    }
}

void System::PhysicsSystem::shutdown()
{
    Logger::info("[PhysicsSystem] shutdown complete");
}


extern "C" std::shared_ptr<System::ISystem> createInputSystem(std::any params)
{
    try {
        return std::make_shared<System::PhysicsSystem>();
    } catch (const std::exception& e) {
        Logger::error(std::string("[Factory]: Failed to create InputSystem: ") + e.what());
    }
    return nullptr;
}
