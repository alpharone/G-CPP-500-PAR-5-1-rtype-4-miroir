#include "SoundSystem.hpp"

void System::SoundSystem::update(Ecs::Registry& r, float dt)
{
    auto& sound_array = registry.getComponents<sound_t>();

    for (size_t entity = 0; entity < sound_array.size(); ++entity) {
        if (!sound_array[entity].has_value())
            continue;
        auto& sound = sound_array[entity].value();
        if (sound.play) {
            playSound(sound);
            sound.play = false;
        }
    }
}
void System::SoundSystem::playSound(const Sound& sound)
{
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