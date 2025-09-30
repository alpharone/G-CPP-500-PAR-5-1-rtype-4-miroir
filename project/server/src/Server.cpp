/*
** EPITECH PROJECT, 2025
** repo
** File description:
** Server
*/

#include "Server.hpp"
#include "ReliableChannel.hpp"
#include "MessageType.hpp"
#include "MessageHeader.hpp"

Server::Server::Server(asio::io_context& ctx, unsigned short port) : _socket(ctx, asio::ip::udp::endpoint(asio::ip::udp::v4(), port)) 
{
    std::cout << "[Server] Listening on " << port << " (UDP)\n";
    doReceive();
}

void Server::Server::doReceive()
{
    _socket.async_receive_from(
        asio::buffer(_buffer), _remote,
        [this](std::error_code ec, std::size_t bytes) {
            if (!ec && bytes >= sizeof(Network::MessageHeader)) {
                auto pkt = Network::Packet::deserialize(_buffer.data(), bytes);
                handlePacket(pkt, _remote);
            }
            doReceive();
        }
    );
}

void Server::Server::handlePacket(Network::Packet& p, asio::ip::udp::endpoint& sender)
{
    std::string key = sender.address().to_string() + ":" + std::to_string(sender.port());

    if (_connections.find(key) == _connections.end()) {
        _connections[key].endpoint = sender;
        _connections[key].clientId = _nextClientId++;
        std::cout << "[Server] New connection: " << key 
                  << " assigned id=" << _connections[key].clientId << "\n";
    }
    auto& conn = _connections[key];

    switch (p.header.type) {
        case Network::NEW_CLIENT: {
            Network::Packet reply;
            reply.header.type = Network::ACCEPT_CLIENT;
            reply.header.length = sizeof(uint32_t);
            reply.header.seq = 1;
            reply.header.flags = 0;
            reply.payload.resize(sizeof(uint32_t));
            std::memcpy(reply.payload.data(), &conn.clientId, sizeof(uint32_t));
            sendPacket(conn, reply, true);
            break;
        }
        case Network::CLIENT_INPUT:
            std::cout << "[Server] Input from client " << conn.clientId << "\n";
            break;
        case Network::PLAYER_READY:
            conn.ready = (p.payload[0] == 1);
            std::cout << "[Server] Client " << conn.clientId 
                      << (conn.ready ? " READY" : " NOT READY") << "\n";
            break;
        case Network::PING: {
            Network::Packet pong;
            pong.header.type = Network::PONG;
            pong.header.length = p.payload.size();
            pong.header.seq = 0;
            pong.header.flags = 0;
            pong.payload = p.payload;
            sendPacket(conn, pong, false);
            break;
        }
        case Network::ACK:
            if (p.payload.size() >= sizeof(uint32_t)) {
                uint32_t seq;
                std::memcpy(&seq, p.payload.data(), sizeof(uint32_t));
                conn.reliable.onAck(seq);
            }
            break;
        default:
            std::cout << "[Server] Unknown msg " << (int)p.header.type << "\n";
    }
}

void Server::Server::sendPacket(Connection& connect, Network::Packet& p, bool reliable)
{
    if (reliable) {
        connect.reliable.sendReliable(p);
    }
    auto data = p.serialize();
    _socket.async_send_to(
        asio::buffer(data), connect.endpoint,
        [](std::error_code ec, std::size_t){ if(ec) std::cerr << "Send error\n"; }
    );
}
