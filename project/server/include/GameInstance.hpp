/*
** EPITECH PROJECT, 2025
** repo
** File description:
** GameInstance
*/

#pragma once

#include <vector>
#include <cstdint>
#include <cstring>


// struct EntityState {
//     uint32_t id;
//     float x, y;
//     float vx, vy;
// };

// class GameInstance {
// public:
//     GameInstance() : _nextEntity(1) {}
//     uint32_t spawn(float x, float y) {
//         uint32_t id = _nextEntity++;
//         EntityState s{id, x, y, 0.f, 0.f};
//         _entities.push_back(s);
//         return id;
//     }

//     void applyInput(uint32_t clientId, uint8_t ev, uint8_t key) {
//         // naive: map clientId -> entity index (clientId-1)
//         size_t idx = clientId - 1;
//         if (idx >= _entities.size()) return;
//         auto &e = _entities[idx];
//         constexpr float speed = 50.f;
//         if (ev == 0x01) { // PRESS
//             switch (key) {
//                 case 0: e.vy = -speed; break; // UP
//                 case 1: e.vy = speed; break; // DOWN
//                 case 2: e.vx = -speed; break; // LEFT
//                 case 3: e.vx = speed; break; // RIGHT
//                 default: break;
//             }
//         } else { // RELEASE
//             e.vx = e.vy = 0.f;
//         }
//     }

//     void tick(float dt) {
//         for (auto &e : _entities) {
//             e.x += e.vx * dt;
//             e.y += e.vy * dt;
//         }
//     }

//     // serialize snapshot: uint16_t count, then count * (uint32_t id, float x, float y, float vx, float vy)
//     std::vector<uint8_t> serializeSnapshot() const {
//         uint16_t count = static_cast<uint16_t>(_entities.size());
//         size_t payloadSize = sizeof(uint16_t) + _entities.size() * (sizeof(uint32_t) + 4*sizeof(float));
//         std::vector<uint8_t> out(payloadSize);
//         size_t offset = 0;
//         std::memcpy(out.data() + offset, &count, sizeof(uint16_t)); offset += sizeof(uint16_t);
//         for (auto &e : _entities) {
//             std::memcpy(out.data() + offset, &e.id, sizeof(uint32_t)); offset += sizeof(uint32_t);
//             std::memcpy(out.data() + offset, &e.x, sizeof(float)); offset += sizeof(float);
//             std::memcpy(out.data() + offset, &e.y, sizeof(float)); offset += sizeof(float);
//             std::memcpy(out.data() + offset, &e.vx, sizeof(float)); offset += sizeof(float);
//             std::memcpy(out.data() + offset, &e.vy, sizeof(float)); offset += sizeof(float);
//         }
//         return out;
//     }

//     void ensurePlayers(size_t n) {
//         while (_entities.size() < n) spawn(100.f + 20.f*_entities.size(), 100.f);
//     }

// private:
//     std::vector<EntityState> _entities;
//     uint32_t _nextEntity;
// };