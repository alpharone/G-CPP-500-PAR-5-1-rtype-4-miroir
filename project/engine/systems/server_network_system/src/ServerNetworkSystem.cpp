/*
** EPITECH PROJECT, 2025
** repo
** File description:
** ServerNetworkSystem
*/

#include "ServerNetworkSystem.hpp"
#include "Logger.hpp"
#include "Packets.hpp"
#include "MessageType.hpp"
#include "Position.hpp"
#include "Player.hpp"
#include "Velocity.hpp"
#include "Drawable.hpp"

ServerNetworkSystem::ServerNetworkSystem(unsigned short port)
    : _socket(_io, udp::endpoint(udp::v4(), port))
{
    Logger::info("[Server] Listening on port " + std::to_string(port));
    startReceive();
}

void ServerNetworkSystem::startReceive()
{
    _socket.async_receive_from(asio::buffer(_buffer), _remoteEndpoint,
        [this](std::error_code ec, std::size_t bytes) {
            if (!ec && bytes > 0)
                onReceive(_remoteEndpoint, _buffer.data(), bytes);
            startReceive();
        });
}

void ServerNetworkSystem::onReceive(const udp::endpoint& from, const uint8_t* data, size_t len)
{
    try {
        auto pkt = Network::Packet::deserialize(data, len);
        std::string key = from.address().to_string() + ":" + std::to_string(from.port());

        switch (pkt.header.type) {
            case Network::NEW_CLIENT: {
                Room* targetRoom = nullptr;
                for (auto& room : _rooms) {
                    if (room->clients.size() < 4) {
                        targetRoom = room.get();
                        break;
                    }
                }

                if (!targetRoom) {
                    _rooms.push_back(std::make_unique<Room>());
                    targetRoom = _rooms.back().get();
                    Logger::info("[Server] Created new room #" + std::to_string(_rooms.size() - 1));
                }

                if (targetRoom->clients.find(key) == targetRoom->clients.end()) {
                    uint32_t localId = static_cast<uint32_t>(targetRoom->clients.size());
                    ClientInfo info{ localId, from, std::chrono::steady_clock::now() };
                    targetRoom->clients.emplace(key, info);
                    {
                        std::lock_guard<std::mutex> lock(_inputMutex);
                        targetRoom->pendingSpawns.emplace_back(localId, from);
                    }
                    Logger::info("[Server] New client connected id=" + std::to_string(localId) +
                                 " in room #" + std::to_string(_rooms.size() - 1));
                } else {
                    targetRoom->clients[key].lastSeen = std::chrono::steady_clock::now();
                }
                break;
            }

            case Network::PLAYER_INPUT: {
                if (pkt.payload.size() < sizeof(float) * 2)
                    return;

                float dx;
                float dy;
                std::memcpy(&dx, pkt.payload.data(), sizeof(float));
                std::memcpy(&dy, pkt.payload.data() + sizeof(float), sizeof(float));

                for (auto& room : _rooms) {
                    auto it = room->clients.find(key);
                    if (it != room->clients.end()) {
                        it->second.lastSeen = std::chrono::steady_clock::now();
                        std::lock_guard<std::mutex> lock(_inputMutex);
                        PlayerInput in{ it->second.localId, dx, dy };
                        room->pendingInputs.push_back(in);
                        break;
                    }
                }
                break;
            }
            case Network::ENTITY_DESPAWN: {
                bool removed = false;
                for (auto& room : _rooms) {
                    auto it = std::find_if(room->clients.begin(), room->clients.end(),
                        [&](const auto& kv){
                            return kv.second.endpoint == from;
                        });
                    if (it != room->clients.end()) {
                        uint32_t localId = it->second.localId;
                        uint32_t roomIndex = static_cast<uint32_t>(std::distance(_rooms.begin(),
                            std::find_if(_rooms.begin(), _rooms.end(), [&](const std::unique_ptr<Room>& p){ return p.get() == room.get(); })));
                        uint32_t globalId = roomIndex * 4 + localId;
                    
                        broadcastDespawn(*room, globalId);
                    
                        room->clients.erase(it);
                        Logger::info("[Server] Client requested disconnect. Removed client localId=" + std::to_string(localId) +
                                     " globalId=" + std::to_string(globalId));
                        removed = true;
                        break;
                    }
                }
                if (!removed) {
                    Logger::warn("[Server] Received ENTITY_DESPAWN but endpoint not found: " + from.address().to_string()
                                 + ":" + std::to_string(from.port()));
                }
                break;
            }

            default:
                break;
        }
    } catch (const std::exception& e) {
        Logger::error(std::string("[Server] Packet error: ") + e.what());
    }
}

void ServerNetworkSystem::broadcastDespawn(Room& room, uint32_t globalId)
{
    Network::Packet pkt;
    pkt.header.type = Network::ENTITY_DESPAWN;
    pkt.payload.resize(sizeof(uint32_t));
    std::memcpy(pkt.payload.data(), &globalId, sizeof(uint32_t));

    for (auto& kv : room.clients)
        _socket.send_to(asio::buffer(pkt.serialize()), kv.second.endpoint);

    Logger::info("[Server] Broadcast DESPAWN globalId=" + std::to_string(globalId));
}

void ServerNetworkSystem::spawnPlayer(Room& room, uint32_t localId, const udp::endpoint& endpoint, Ecs::Registry& registry)
{
    Logger::info("[Server] Spawning player id=" + std::to_string(localId));

    const uint8_t kMaxColors = 4;
    const Color kColorTable[kMaxColors] = { RED, BLUE, GREEN, YELLOW };
    auto it = std::find_if(_rooms.begin(), _rooms.end(), [&](const std::unique_ptr<Room>& ptr){
        return ptr.get() == &room;
    });
    uint32_t roomIndex = static_cast<uint32_t>(std::distance(_rooms.begin(), it));
    uint32_t globalId = roomIndex * 4 + localId;

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

    pos[globalId].value() = { 100.f + 60.f * localId, 150.f + 150.f * roomIndex };
    vel[globalId].value() = { 200.f, 200.f };
    draw[globalId].value().color = kColorTable[localId % kMaxColors];
    draw[globalId].value().isPlayer = true;
    draw[globalId].value().z = 10;

    Network::Packet accept;
    accept.header.type = Network::ACCEPT_CLIENT;
    accept.payload.resize(sizeof(uint32_t));
    std::memcpy(accept.payload.data(), &globalId, sizeof(uint32_t));
    _socket.send_to(asio::buffer(accept.serialize()), endpoint);

    Network::Packet spawn;
    spawn.header.type = Network::ENTITY_SPAWN;
    spawn.payload.resize(sizeof(uint32_t) + 2*sizeof(float) + sizeof(uint8_t));
    size_t off = 0;
    std::memcpy(spawn.payload.data()+off, &globalId, sizeof(uint32_t)); off += sizeof(uint32_t);
    std::memcpy(spawn.payload.data()+off, &pos[globalId]->x, sizeof(float)); off += sizeof(float);
    std::memcpy(spawn.payload.data()+off, &pos[globalId]->y, sizeof(float)); off += sizeof(float);
    std::memcpy(spawn.payload.data()+off, &localId, sizeof(uint8_t));

    for (auto& kv : room.clients)
        _socket.send_to(asio::buffer(spawn.serialize()), kv.second.endpoint);

    Logger::info("[Server] Spawned player entity " + std::to_string(globalId));
}

void ServerNetworkSystem::applyInputs(Room& room, float dt, Ecs::Registry& registry)
{
    std::lock_guard<std::mutex> lock(_inputMutex);
    auto& pos = registry.getComponents<Component::position_t>();
    auto& vel = registry.getComponents<Component::velocity_t>();

    for (auto& in : room.pendingInputs) {
        auto it = std::find_if(_rooms.begin(), _rooms.end(), [&](const std::unique_ptr<Room>& ptr){
            return ptr.get() == &room;
        });
        uint32_t roomIndex = static_cast<uint32_t>(std::distance(_rooms.begin(), it));
        uint32_t globalId = roomIndex * 4 + in.id;

        if (globalId >= pos.size() || !pos[globalId].has_value() || globalId >= vel.size() || !vel[globalId].has_value())
            continue;

        pos[globalId].value().x += in.dx * dt * vel[globalId]->vx;
        pos[globalId].value().y += in.dy * dt * vel[globalId]->vy;
    }
    room.pendingInputs.clear();
}

void ServerNetworkSystem::sendSnapshotToRoom(Room& room, Ecs::Registry& registry)
{
    auto& pos = registry.getComponents<Component::position_t>();
    auto& vel = registry.getComponents<Component::velocity_t>();
    uint16_t count = 0;
    std::vector<uint8_t> payload(sizeof(uint16_t));

    auto it = std::find_if(_rooms.begin(), _rooms.end(), [&](const std::unique_ptr<Room>& ptr){
        return ptr.get() == &room;
    });
    uint32_t roomIndex = static_cast<uint32_t>(std::distance(_rooms.begin(), it));

    for (uint32_t localId = 0; localId < 4; localId++) {
        uint32_t globalId = roomIndex * 4 + localId;
        if (globalId >= pos.size() || !pos[globalId].has_value() || globalId >= vel.size() || !vel[globalId].has_value())
            continue;
        count++;

        const auto& p = pos[globalId].value();
        const auto& v = vel[globalId].value();

        size_t oldSize = payload.size();
        payload.resize(oldSize + sizeof(uint32_t) + 4*sizeof(float));
        std::memcpy(payload.data() + oldSize, &globalId, sizeof(uint32_t));
        std::memcpy(payload.data() + oldSize + 4, &p.x, sizeof(float));
        std::memcpy(payload.data() + oldSize + 8, &p.y, sizeof(float));
        std::memcpy(payload.data() + oldSize + 12, &v.vx, sizeof(float));
        std::memcpy(payload.data() + oldSize + 16, &v.vy, sizeof(float));
    }

    std::memcpy(payload.data(), &count, sizeof(uint16_t));

    Network::Packet pkt;
    pkt.header.type = Network::SERVER_SNAPSHOT;
    pkt.payload = std::move(payload);
    auto data = pkt.serialize();

    for (auto& kv : room.clients)
        _socket.send_to(asio::buffer(data), kv.second.endpoint);

    Logger::info("[Server] Sending snapshot to " + std::to_string(room.clients.size()) +
                 " clients (" + std::to_string(count) + " entities)");
}

void ServerNetworkSystem::update(Ecs::Registry& registry, float dt)
{
    _io.poll();
    _io.reset();

    const auto now = std::chrono::steady_clock::now();
    const auto kClientTimeout = std::chrono::seconds(10);

    for (auto& room : _rooms) {
        {
            std::lock_guard<std::mutex> lock(_inputMutex);
            for (auto& s : room->pendingSpawns)
                spawnPlayer(*room, s.first, s.second, registry);
            room->pendingSpawns.clear();
        }

        applyInputs(*room, dt, registry);

        // std::vector<std::string> toRemove;
        // for (auto& [key, info] : room->clients) {
        //     if (now - info.lastSeen > kClientTimeout)
        //         toRemove.push_back(key);
        // }

        // for (const auto& key : toRemove) {
        //     auto it = room->clients.find(key);
        //     if (it != room->clients.end()) {
        //         uint32_t localId = it->second.localId;
        //         uint32_t roomIndex = static_cast<uint32_t>(
        //             std::distance(_rooms.begin(), std::find_if(_rooms.begin(), _rooms.end(), [&](auto& p){return p.get()==room.get();}))
        //         );
        //         uint32_t globalId = roomIndex * 4 + localId;

        //         broadcastDespawn(*room, globalId);
        //         room->clients.erase(it);
        //         Logger::info("[Server] Client timeout, despawned id=" + std::to_string(globalId));
        //     }
        // }

        sendSnapshotToRoom(*room, registry);
    }
}

extern "C" std::shared_ptr<ISystem> createServerNetworkSystem(std::any params)
{
    unsigned short port = 4242;

    if (params.has_value())
        port = std::any_cast<unsigned short>(params);
    return std::make_shared<ServerNetworkSystem>(port);
}
