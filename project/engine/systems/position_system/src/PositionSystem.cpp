#include "PositionSystem.hpp"

void System::PositionSystem::init(Ecs::Registry& registry)
{
    Logger::info("[PositionSystem] initialized");
}

void System::PositionSystem::update(Ecs::Registry& registry, double dt) {
        auto& pos_array = registry.getComponents<Component::position_t>();
        auto& vel_array = registry.getComponents<Component::velocity_t>();

        for (size_t entity = 0; entity < pos_array.size(); ++entity) {
            if (!pos_array[entity].has_value() || !vel_array[entity].has_value())
                continue;
            auto& pos = pos_array[entity].value();
            auto& vel = vel_array[entity].value();

            pos.x += vel.vx * dt;
            pos.y += vel.vy * dt;
        }
}

void System::PositionSystem::shutdown()
{
    Logger::info("[PositionSystem] shutdown complete");
}

extern "C" std::shared_ptr<System::ISystem> createInputSystem(std::any params)
{
    try {
        return std::make_shared<System::PositionSystem>();
    } catch (const std::exception& e) {
        Logger::error(std::string("[Factory]: Failed to create InputSystem: ") + e.what());
    }
    return nullptr;
}