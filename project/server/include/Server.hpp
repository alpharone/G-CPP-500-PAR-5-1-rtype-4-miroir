// /*
// ** EPITECH PROJECT, 2025
// ** repo
// ** File description:
// ** Server
// */

// #pragma once

// #include <asio.hpp>
// #include <thread>
// #include <atomic>
// #include <unordered_map>
// #include <mutex>
// #include <queue>
// #include <iostream>
// #include "Packets.hpp"
// #include "Connection.hpp"
// #include "LobbyManager.hpp"

// namespace Server {

//     class Server {
//         public:
//             Server(asio::io_context& ctx, unsigned short port);
//             ~Server() = default;
        
//         private:
//             asio::ip::udp::socket _socket;
//             asio::ip::udp::endpoint _remote;
//             std::array<uint8_t, 2048> _buffer;
        
//             std::unordered_map<std::string, Connection> _connections;
//             LobbyManager _lobbies;
//             uint32_t _nextClientId{1};
        
//             void doReceive();
//             void handlePacket(Network::Packet& p, asio::ip::udp::endpoint& sender);
//             void sendPacket(Connection& c, Network::Packet& p, bool reliable);
//     };

// }