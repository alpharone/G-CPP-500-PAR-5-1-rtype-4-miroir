#include "SpawnSystem.hpp"
#include "Components.hpp"
#include "Logger.hpp"
#include <cstdlib>

void SpawnSystem::update(Ecs::Registry& registry, float dt)
{
    _spawnTimer += dt;

    if (_spawnTimer >= _spawnInterval) {
        _spawnTimer = 0.f;

        auto enemy = registry.spawnEntity();

        float x = 800 + (std::rand() % 200);
        float y = std::rand() % 600;

        registry.addComponent<position_t>(enemy, {x, y});
        registry.addComponent<velocity_t>(enemy, {-100.f, 0.f});
        registry.addComponent<collider_t>(enemy, {32, 32});
        registry.addComponent<health_t>(enemy, {100});
       // registry.addComponent<>(enemy, ); //faut que je fasse un truc pour foutre un fichier ia si on garde le set par file
        registry.addComponent<sprite_t>(enemy, {""});

    }
}
