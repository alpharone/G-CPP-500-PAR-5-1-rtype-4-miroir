/*
** EPITECH PROJECT, 2025
** repo
** File description:
** ServerNetworkSystem
*/

#include <cstring>
#include <tuple>
#include <chrono>
#include "ClientNetworkSystem.hpp"
#include "Position.hpp"
#include "Velocity.hpp"
#include "MessageType.hpp"
#include "Packets.hpp"
#include "Drawable.hpp"

ClientNetworkSystem::ClientNetworkSystem(const std::string& host, unsigned short port, std::shared_ptr<NetworkContext> ctx) : _ctx(std::move(ctx))
{
    try {
        udp::resolver resolver(_io);
        auto endpoints = resolver.resolve(udp::v4(), host, std::to_string(port));
        if (endpoints.begin() == endpoints.end())
            throw std::runtime_error("resolve returned empty");

        _serverEndpoint = *endpoints.begin();
        _socket = std::make_shared<udp::socket>(_io, udp::endpoint(udp::v4(), 0));

        if (_ctx) {
            _ctx->socket = _socket;
            _ctx->serverEndpoint = _serverEndpoint;
            _ctx->playerId = UINT32_MAX;
        }

        Logger::info("[Client] Constructed connecting to " + host + ":" + std::to_string(port));
    } catch (const std::exception& e) {
        Logger::error(std::string("[Client] error: ") + e.what());
        throw;
    }
}

ClientNetworkSystem::~ClientNetworkSystem()
{
    _running = false;
    if (_socket && _socket->is_open()) {
        std::error_code ec;
        _socket->close(ec);
    }
    _io.stop();
    if (_ioThread.joinable())
        _ioThread.join();
}

void ClientNetworkSystem::start()
{
    _running = true;
    sendJoinRequest();
    startReceive();
    _ioThread = std::thread([this]() {
        try {
            _io.run();
        } catch (const std::exception& e) {
            Logger::error(std::string("[Client] io thread: ") + e.what());
        }
    });
}

void ClientNetworkSystem::sendJoinRequest()
{
    Network::Packet pkt;
    pkt.header.type = Network::NEW_CLIENT;
    auto data = pkt.serialize();
    _socket->send_to(asio::buffer(data), _serverEndpoint);
}

void ClientNetworkSystem::startReceive()
{
    auto self_w = weak_from_this();
    _socket->async_receive_from(asio::buffer(_buffer), _senderEndpoint,
        [this, self_w](std::error_code ec, std::size_t bytes) {
            if (!ec && bytes > 0) {
                std::vector<uint8_t> copy(bytes);
                std::memcpy(copy.data(), _buffer.data(), bytes);
                if (auto self = self_w.lock())
                    self->onReceiveData(copy.data(), copy.size());
            }
            if (auto self = self_w.lock(); self && self->_running)
                self->startReceive();
        });
}

void ClientNetworkSystem::onReceiveData(const uint8_t* data, size_t len)
{
    try {
        auto pkt = Network::Packet::deserialize(data, len);
        switch (pkt.header.type) {
            case Network::ACCEPT_CLIENT: {
                if (pkt.payload.size() >= sizeof(uint32_t) && _ctx) {
                    uint32_t id;
                    std::memcpy(&id, pkt.payload.data(), sizeof(uint32_t));
                    _ctx->playerId = id;
                    Logger::info("[Client] Accepted id=" + std::to_string(id));
                }
                break;
            }

            case Network::ENTITY_SPAWN:
            case Network::SERVER_SNAPSHOT:
            case Network::ENTITY_DESPAWN: {
                std::lock_guard<std::mutex> lock(_queueMutex);
                _snapQueue.emplace_back(pkt);
                break;
            }

            default: break;
        }
    } catch (const std::exception& e) {
        Logger::error(std::string("[Client] deserialize error: ") + e.what());
    }
}

void ClientNetworkSystem::handleEntitySpawn(const Network::Packet& pkt, Ecs::Registry& registry)
{
    if (pkt.payload.size() < sizeof(uint32_t) + 2 * sizeof(float) + sizeof(uint8_t))
        return;

    size_t off = 0;
    uint32_t globalId;
    uint8_t localId;
    float x;
    float y;
    std::memcpy(&globalId, pkt.payload.data()+off, sizeof(uint32_t)); off += sizeof(uint32_t);
    std::memcpy(&x, pkt.payload.data()+off, sizeof(float)); off += sizeof(float);
    std::memcpy(&y, pkt.payload.data()+off, sizeof(float)); off += sizeof(float);
    std::memcpy(&localId, pkt.payload.data()+off, sizeof(uint8_t));

    uint32_t myRoom = (_ctx && _ctx->playerId != UINT32_MAX) ? _ctx->playerId / 4 : UINT32_MAX;
    uint32_t entityRoom = globalId / 4;
    if (myRoom != entityRoom)
        return;

    auto& pos = registry.getComponents<Component::position_t>();
    auto& vel = registry.getComponents<Component::velocity_t>();
    auto& draw = registry.getComponents<Component::drawable_t>();

    if (globalId >= pos.size()) {
        while (pos.size() <= globalId) 
            pos.insert_at(pos.size(), Component::position_t{});
        while (vel.size() <= globalId) 
            vel.insert_at(vel.size(), Component::velocity_t{});
        while (draw.size() <= globalId) 
            draw.insert_at(draw.size(), Component::drawable_t{});
    }

    pos[globalId].value() = {x, y};
    vel[globalId].value() = {0.f, 0.f};
    draw[globalId].value().isPlayer = true;

    static const Color colorTable[4] = { RED, BLUE, GREEN, YELLOW };
    draw[globalId].value().color = colorTable[localId % 4];
    draw[globalId].value().z = 10;

    static const std::string textures[4] = {
        "assets/sprites/r-typesheet42.gif",
        "assets/sprites/r-typesheet42.gif",
        "assets/sprites/r-typesheet42.gif",
        "assets/sprites/r-typesheet42.gif"
    };
    draw[globalId].value().texturePath = textures[localId % 4];

    Logger::info("[Client] Spawn entity id=" + std::to_string(globalId) +
                 " color=" + std::to_string(localId) +
                 " room=" + std::to_string(entityRoom));
}

void ClientNetworkSystem::handleSnapshot(const Network::Packet& pkt, Ecs::Registry& registry)
{
    if (pkt.payload.size() < sizeof(uint16_t))
        return;

    size_t offset = 0;
    uint16_t count;
    std::memcpy(&count, pkt.payload.data()+offset, sizeof(uint16_t));
    offset += sizeof(uint16_t);

    auto& pos = registry.getComponents<Component::position_t>();
    auto& vel = registry.getComponents<Component::velocity_t>();

    uint32_t myRoom = (_ctx && _ctx->playerId != UINT32_MAX) ? _ctx->playerId / 4 : UINT32_MAX;

    for (uint16_t i = 0; i < count; i++) {
        if (offset + sizeof(uint32_t) + 4*sizeof(float) > pkt.payload.size())
            break;

        uint32_t id;
        float x;
        float y;
        float vx;
        float vy;
        std::memcpy(&id, pkt.payload.data()+offset, sizeof(uint32_t)); offset += sizeof(uint32_t);
        std::memcpy(&x, pkt.payload.data()+offset, sizeof(float)); offset += sizeof(float);
        std::memcpy(&y, pkt.payload.data()+offset, sizeof(float)); offset += sizeof(float);
        std::memcpy(&vx, pkt.payload.data()+offset, sizeof(float)); offset += sizeof(float);
        std::memcpy(&vy, pkt.payload.data()+offset, sizeof(float)); offset += sizeof(float);

        uint32_t entityRoom = id / 4;
        if (entityRoom != myRoom)
            continue;

        if (id >= pos.size()) {
            while (pos.size() <= id)
                pos.insert_at(pos.size(), Component::position_t{});
            while (vel.size() <= id)
                vel.insert_at(vel.size(), Component::velocity_t{});
        }

        pos[id].value().x = x;
        pos[id].value().y = y;
        vel[id].value().vx = vx;
        vel[id].value().vy = vy;
    }
}

void ClientNetworkSystem::handleEntityDespawn(const Network::Packet& pkt, Ecs::Registry& registry)
{
    if (pkt.payload.size() < sizeof(uint32_t))
        return;

    uint32_t id;
    std::memcpy(&id, pkt.payload.data(), sizeof(uint32_t));

    auto& pos = registry.getComponents<Component::position_t>();
    auto& vel = registry.getComponents<Component::velocity_t>();
    auto& draw = registry.getComponents<Component::drawable_t>();

    if (id < pos.size())
        pos[id].reset();
    if (id < vel.size())
        vel[id].reset();
    if (id < draw.size())
        draw[id].reset();

    Logger::info("[Client] ENTITY_DESPAWN id=" + std::to_string(id));
}

void ClientNetworkSystem::update(Ecs::Registry& registry, float dt)
{
    std::deque<Network::Packet> queueCopy;
    {
        std::lock_guard<std::mutex> lock(_queueMutex);
        queueCopy.swap(_snapQueue);
    }

    while (!queueCopy.empty()) {
        auto pkt = std::move(queueCopy.front());
        queueCopy.pop_front();

        switch (pkt.header.type) {
            case Network::ENTITY_SPAWN:
                handleEntitySpawn(pkt, registry);
                break;
            case Network::SERVER_SNAPSHOT:
                handleSnapshot(pkt, registry);
                break;
            case Network::ENTITY_DESPAWN:
                handleEntityDespawn(pkt, registry);
                break;
            default:
                break;
        }
    }

    uint64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    if (now - _lastPingTs > 3000) {
        Network::Packet ping;
        ping.header.type = Network::PING;
        ping.payload.resize(sizeof(uint64_t));
        std::memcpy(ping.payload.data(), &now, sizeof(uint64_t));
        try {
            _socket->send_to(asio::buffer(ping.serialize()), _serverEndpoint);
        } catch (const std::exception& e) {
            Logger::error(std::string("[Client]: Faile during update: ") + e.what());
        }
        _lastPingTs = now;
    }
}

extern "C" std::shared_ptr<ISystem> createClientNetworkSystem(std::any params)
{
    try {
        auto t = std::any_cast<std::tuple<std::string, unsigned short, std::shared_ptr<NetworkContext>>>(params);
        auto sys = std::make_shared<ClientNetworkSystem>(std::get<0>(t), std::get<1>(t), std::get<2>(t));
        sys->start();
        return sys;
    } catch (const std::exception& e) {
        Logger::error(std::string("[ClientFactory] ") + e.what());
        return nullptr;
    }
}