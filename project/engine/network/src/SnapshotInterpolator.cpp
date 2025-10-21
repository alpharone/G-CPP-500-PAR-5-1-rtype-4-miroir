/*
** EPITECH PROJECT, 2025
** arbo_save
** File description:
** SnapshotInterpolator
*/

#include "SnapshotInterpolator.hpp"

void Network::SnapshotInterpolator::addSnapshot(const snapshot_t& snap)
{
    snapshots.push_back(snap);
    if (snapshots.size() > 3)
        snapshots.pop_front();
}

std::vector<Network::snapshot_entity_state_t> Network::SnapshotInterpolator::interpolate(double renderTime)
{
    if (snapshots.size() < 2)
        return {};

    const auto& s0 = snapshots[0];
    const auto& s1 = snapshots[1];

    double t0 = s0.timestamp;
    double t1 = s1.timestamp;

    if (renderTime <= t0)
        return s0.entities;
    if (renderTime >= t1)
        return s1.entities;

    double alpha = (renderTime - t0) / (t1 - t0);
    std::vector<snapshot_entity_state_t> result;

    for (size_t i = 0; i < s0.entities.size() && i < s1.entities.size(); i++) {
        const auto& a = s0.entities[i];
        const auto& b = s1.entities[i];
        snapshot_entity_state_t interp = {
            a.entityId,
            static_cast<float>(a.x + (b.x - a.x) * alpha),
            static_cast<float>(a.y + (b.y - a.y) * alpha)
        };
        result.push_back(interp);
    }
    return result;
}
