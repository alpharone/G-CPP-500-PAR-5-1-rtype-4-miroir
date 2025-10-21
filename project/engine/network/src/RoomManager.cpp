/*
** EPITECH PROJECT, 2025
** repo
** File description:
** RoomManager
*/

#include "RoomManager.hpp"
#include "Utils.hpp"
#include "Logger.hpp"
#include "MessageType.hpp"
#include <chrono>

using namespace std::chrono;

namespace Network {

    RoomManager::RoomManager()
    {
    }

    RoomManager::~RoomManager()
    {
    }

    uint32_t RoomManager::createRoom()
    {
        std::lock_guard<std::mutex> lock(_roomsMtx);
        uint32_t id = ++_nextRoomId;
        _rooms.emplace(id, room_t{id});
        Logger::info("[Server] Created new room #" + std::to_string(id));
        return id;
    }

    std::optional<std::pair<uint32_t, uint32_t>> RoomManager::joinAuto(const endpoint_t& endpoint)
    {
        std::lock_guard<std::mutex> lock(_roomsMtx);

        for (auto& [id, room] : _rooms) {
            std::lock_guard<std::mutex> rlk(room.mtx);
            if (room.clients.size() < static_cast<size_t>(MAX_PLAYERS_PER_ROOM)) {
                uint32_t cid = room.nextClientId++;
                room_client_t rc{cid, endpoint, false, steady_clock::now()};
                room.clients.emplace(cid, std::move(rc));

                Network::Packet accept;
                accept.header.type = Network::ACCEPT_CLIENT;
                accept.header.seq = 0;
                std::vector<uint8_t> pay;
                Network::write_u32_le(pay, cid);
                accept.payload = std::move(pay);
                accept.header.length = static_cast<uint16_t>(accept.payload.size());
                room.outgoing.push_back(std::move(accept));

                return std::make_optional(std::make_pair(id, cid));
            }
        }

        uint32_t newRoomId = _nextRoomId += 1;
        room_t newRoom{newRoomId};
        uint32_t cid = newRoom.nextClientId += 1;
        room_client_t rc{cid, endpoint, false, steady_clock::now()};
        newRoom.clients.emplace(cid, std::move(rc));

        Network::Packet accept;
        accept.header.type = Network::ACCEPT_CLIENT;
        std::vector<uint8_t> pay;
        Network::write_u32_le(pay, cid);
        accept.payload = std::move(pay);
        accept.header.length = static_cast<uint16_t>(accept.payload.size());
        newRoom.outgoing.push_back(std::move(accept));

        _rooms.emplace(newRoomId, std::move(newRoom));
        Logger::info("[Server] Created room #" + std::to_string(newRoomId) + " for new client");
        return std::make_optional(std::make_pair(newRoomId, cid));
    }

    void RoomManager::leave(uint32_t roomId, uint32_t clientId)
    {
        std::lock_guard<std::mutex> lock(_roomsMtx);
        auto it = _rooms.find(roomId);
        if (it == _rooms.end())
            return;
        auto& room = it->second;
        std::lock_guard<std::mutex> rlk(room.mtx);
        auto itc = room.clients.find(clientId);
        if (itc == room.clients.end())
            return;

        Network::Packet desp;
        desp.header.type = Network::ENTITY_DESPAWN;
        std::vector<uint8_t> pay;
        Network::write_u32_le(pay, clientId);
        desp.payload = std::move(pay);
        desp.header.length = static_cast<uint16_t>(desp.payload.size());
        room.outgoing.push_back(std::move(desp));

        room.clients.erase(itc);
        Logger::info("[Server] Client " + std::to_string(clientId) + " left room #" + std::to_string(roomId));
    }

    RoomManager::PacketVec RoomManager::collectOutgoing(uint32_t roomId)
    {
        PacketVec ret;
        std::lock_guard<std::mutex> lock(_roomsMtx);
        auto it = _rooms.find(roomId);
        if (it == _rooms.end())
            return ret;
        
        auto& room = it->second;
        std::lock_guard<std::mutex> rlk(room.mtx);
        ret.swap(room.outgoing);
        return ret;
    }

    room_t& RoomManager::getRoom(uint32_t id)
    {
        std::lock_guard<std::mutex> lock(_roomsMtx);
        auto it = _rooms.find(id);
        if (it == _rooms.end()) {
            static room_t dummy{0};
            Logger::warn("[Server] getRoom() called for unknown room #" + std::to_string(id));
            return dummy;
        }
        return it->second;
    }

    std::vector<uint32_t> RoomManager::listRooms()
    {
        std::lock_guard<std::mutex> lock(_roomsMtx);
        std::vector<uint32_t> out;
        out.reserve(_rooms.size());
        for (auto const& [id, _] : _rooms)
            out.push_back(id);
        return out;
    }

    void RoomManager::onPacket(uint32_t roomId, const endpoint_t& from, const Network::Packet& p)
    {
        std::lock_guard<std::mutex> lock(_roomsMtx);
        auto it = _rooms.find(roomId);
        if (it == _rooms.end())
            return;
        auto& room = it->second;
        std::lock_guard<std::mutex> rlk(room.mtx);

        auto itc = std::find_if(room.clients.begin(), room.clients.end(),
            [&](auto const& kv){ return kv.second.endpoint == from; });

        if (itc == room.clients.end())
            return;

        uint32_t clientId = itc->first;
        room.clients[clientId].lastSeen = std::chrono::steady_clock::now();

        switch (p.header.type) {
            case Network::CLIENT_INPUT: {
                pending_input_t pi;
                pi.clientId = clientId;
                pi.payload = p.payload;
                room.pendingInputs.push_back(std::move(pi));
                break;
            }

            case Network::ENTITY_DESPAWN:
            case Network::SERVER_SNAPSHOT:
            default:
                room.outgoing.push_back(p);
                break;
        }
    }

    void RoomManager::tick(double dt)
    {
        std::lock_guard<std::mutex> lock(_roomsMtx);
        auto now = std::chrono::steady_clock::now();

        for (auto &[id, room] : _rooms) {
            std::lock_guard<std::mutex> rlk(room.mtx);

            for (auto &input : room.pendingInputs) {
                auto itClient = room.clients.find(input.clientId);
                if (itClient == room.clients.end())
                    continue;

                auto &client = itClient->second;

                if (input.payload.size() >= 2) {
                    uint8_t pressed = input.payload[0];
                    uint8_t code = input.payload[1];
                
                    float speed = 200.0f * dt;
                    if (pressed) {
                        switch (code) {
                            case Network::UP:
                                client.posY -= speed;
                                break; 
                            case Network::DOWN:
                                client.posY += speed;
                                break; 
                            case Network::LEFT:
                                client.posX -= speed;
                                break;
                            case Network::RIGHT:
                                client.posX += speed;
                                break;
                            default:
                                break;
                        }
                    }
                }
            }

            room.pendingInputs.clear();

            std::vector<uint32_t> toRemove;
            for (auto &[cid, client] : room.clients) {
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - client.lastSeen).count();
                if (elapsed > 60)
                    toRemove.push_back(cid);
            }
            for (auto cid : toRemove) {
                Logger::info("[Server] Removing inactive client " + std::to_string(cid) + " from room #" + std::to_string(id));
                room.clients.erase(cid);
            }

            room.snapshotTimer += dt;
            if (room.snapshotTimer >= 0.05) {
                room.snapshotTimer = 0.0;

                Network::Packet snap;
                snap.header.type = Network::SERVER_SNAPSHOT;

                for (auto &[cid, client] : room.clients) {
                    Network::write_u32_le(snap.payload, cid);
                
                    uint32_t xInt;
                    std::memcpy(&xInt, &client.posX, sizeof(float));
                    Network::write_u32_le(snap.payload, xInt);
                
                    uint32_t yInt;
                    std::memcpy(&yInt, &client.posY, sizeof(float));
                    Network::write_u32_le(snap.payload, yInt);
                }

                snap.header.length = static_cast<uint16_t>(snap.payload.size());
                room.outgoing.push_back(std::move(snap));
            }
        }
    }

}
