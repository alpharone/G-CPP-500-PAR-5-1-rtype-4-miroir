#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <optional>
#include <memory>
#include "ISystem.hpp"
#include "Registry.hpp"
#include "Position.hpp"
#include "Velocity.hpp"
#include "Registry.hpp"
#include "Friction.hpp"
#include "Sprite.hpp"
#include "Logger.hpp"
#include "Healt.hpp"
#include "Collision.hpp"

namespace System {

class SpawnSystem : public ISystem {
    public:
    SpawnSystem() = default;
    ~SpawnSystem() = default;
    void update(Ecs::Registry& r, float dt);
    private:
};
}