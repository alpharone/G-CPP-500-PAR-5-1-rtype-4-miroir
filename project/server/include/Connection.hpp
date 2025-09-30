/*
** EPITECH PROJECT, 2025
** repo
** File description:
** Connection
*/

#pragma once

#include <asio.hpp>
#include <queue>
#include <mutex>
#include <chrono>
#include "Packets.hpp"
#include "ReliableChannel.hpp"

namespace Server {

    struct Connection {
        asio::ip::udp::endpoint endpoint;
        ReliableChannel reliable;
        std::queue<Network::Packet> pending_packet;
        bool ready{false};
        uint32_t clientId{0};
    };

}