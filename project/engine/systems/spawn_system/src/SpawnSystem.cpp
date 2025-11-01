#include "SpawnSystem.hpp"
#include <cstdlib>

void System::SpawnSystem::update(Ecs::Registry& r, float dt)
{
    // _spawnTimer += dt;

    // if (_spawnTimer >= _spawnInterval) {
    //     _spawnTimer = 0.f;

        auto enemy = r.spawnEntity();

        float x = 800 + (std::rand() % 200);
        float y = std::rand() % 600;

        r.emplaceComponent<Component::position_t>(enemy, x, y);
        r.emplaceComponent<Component::velocity_t>(enemy, -100.f, 0.f);
        r.emplaceComponent<Component::collision_t>(enemy, 32, 32);
        r.emplaceComponent<Component::healt_t>(enemy, 100);
        r.emplaceComponent<Component::sprite_t>(enemy, "");
}
       // r.emplaceComponent<>(enemy, ); //faut que je fasse un truc pour foutre un fichier ia si on garde le set par file
