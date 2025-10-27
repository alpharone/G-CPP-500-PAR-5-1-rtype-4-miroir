#include "PositionSystem.hpp"

void PositionSystem::update(Ecs::Registry& r, float dt) {
        auto& pos_array = r.getComponents<position_t>();
        auto& vel_array = r.getComponents<velocity_t>();

        for (size_t entity = 0; entity < pos_array.size(); ++entity) {
            if (!pos_array[entity].has_value() || !vel_array[entity].has_value())
                continue;
            auto& pos = pos_array[entity].value();
            auto& vel = vel_array[entity].value();

            pos.x += vel.vx * dt;
            pos.y += vel.vy * dt;
        }
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