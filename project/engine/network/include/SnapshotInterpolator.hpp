/*
** EPITECH PROJECT, 2025
** arbo_save
** File description:
** SnapshotInterpolator
*/


#pragma once
#include <unordered_map>
#include <deque>
#include <cstdint>
#include <chrono>
#include <vector>

namespace Network {

    struct snapshot_entity_state_t {
        uint32_t entityId;
        float x;
        float y;
    };

    struct snapshot_t {
        double timestamp;
        std::vector<snapshot_entity_state_t> entities;
    };

    class SnapshotInterpolator {
        public:
            void addSnapshot(const snapshot_t& snap);
            std::vector<snapshot_entity_state_t> interpolate(double renderTime);

        private:
            std::deque<snapshot_t> snapshots;
    };

}
