/*
** EPITECH PROJECT, 2025
** repo
** File description:
** Room
*/

#pragma once

#include <asio.hpp>
#include <unordered_map>
#include <vector>
#include <string>
#include <chrono>

using asio::ip::udp;

struct PlayerInput {
    uint32_t id;
    float dx;
    float dy;
};

struct ClientInfo {
    uint32_t localId;
    udp::endpoint endpoint;
    std::chrono::steady_clock::time_point lastSeen;
};

struct Room {
    std::unordered_map<std::string, ClientInfo> clients;
    std::vector<std::pair<uint32_t, udp::endpoint>> pendingSpawns;
    std::vector<PlayerInput> pendingInputs;
};
