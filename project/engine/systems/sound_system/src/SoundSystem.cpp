#include "SoundSystem.hpp"


void System::SoundSystem::init(Ecs::Registry&)
{
    InitAudioDevice();
    Logger::info("[SoundSystem] Audio device initialized");
}

void System::SoundSystem::update(Ecs::Registry& registry, double dt)
{
    auto& sound_array = registry.getComponents<Component::sound_t>();

    for (size_t entity = 0; entity < sound_array.size(); ++entity) {
        if (!sound_array[entity].has_value())
            continue;
        auto& sound = sound_array[entity].value();
        if (sound.play) {
           // playSound(sound);
            sound.play = false;
        }
    }
}
void System::SoundSystem::playSound(const Sound& sound)
{
}

void System::SoundSystem::shutdown()
{
    CloseAudioDevice();
    Logger::info("[SoundSystem] Audio device closed");
}

extern "C" std::shared_ptr<System::ISystem> createInputSystem(std::any params)
{
    try {
        return std::make_shared<System::SoundSystem>();
    } catch (const std::exception& e) {
        Logger::error(std::string("[Factory]: Failed to create InputSystem: ") + e.what());
    }
    return nullptr;
}