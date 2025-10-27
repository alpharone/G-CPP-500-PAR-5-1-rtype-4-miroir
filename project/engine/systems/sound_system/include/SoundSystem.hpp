#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <optional>
#include <memory>
#include "ISystem.hpp"
#include "Registry.hpp"
#include "Sound.hpp"

namespace System {

class SoundSystem : public ISystem {
    public:
    SoundSystem() = default;
    ~SoundSystem() = default;
    void update(Ecs::Registry& r, float dt);
    private:
};
}