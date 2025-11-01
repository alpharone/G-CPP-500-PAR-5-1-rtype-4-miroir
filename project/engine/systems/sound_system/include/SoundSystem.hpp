#pragma once
#include <string>
#include <raylib.h>
#include <unordered_map>
#include <vector>
#include <functional>
#include <optional>
#include <memory>
#include "Logger.hpp"
#include "ISystem.hpp"
#include "Registry.hpp"
#include "Sound.hpp"

namespace System {

class SoundSystem : public ISystem {
    public:
        SoundSystem() = default;
        ~SoundSystem() override = default;

        void init(Ecs::Registry& registry) override;
        void update(Ecs::Registry& registry, double dt);
        void playSound(const Sound& sound);
        void shutdown() override;
    private:
};
}