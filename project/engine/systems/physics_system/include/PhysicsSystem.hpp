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
#include "Logger.hpp"


namespace System {

class PhysicsSystem : public ISystem {
    public:
    PhysicsSystem() = default;
    ~PhysicsSystem() = default;
    void update(Registry& r, float dt);
    private:
};
}