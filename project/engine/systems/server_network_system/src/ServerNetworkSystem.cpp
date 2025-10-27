/*
** EPITECH PROJECT, 2025
** repo
** File description:
** ServerNetworkSystem
*/

#include "ServerNetworkSystem.hpp"
#include "AsioNetworkTransport.hpp"
#include "DefaultMessageSerializer.hpp"
#include "MessageType.hpp"
#include "ReliableLayerAdapter.hpp"
#include "Utils.hpp"

System::ServerNetworkSystem::ServerNetworkSystem(unsigned short port)
    : _port(port), _start_x(100), _start_y(200), _speed(200), _max_players(4),
      _max_rooms(10), _timeout(60), _screen_width(1200), _screen_height(1000),
      _snapshot_interval(0.016), _startTime(std::chrono::steady_clock::now()) {}

void System::ServerNetworkSystem::init(Ecs::Registry &) {
  _transport =
      std::make_shared<Network::AsioNetworkTransport>("0.0.0.0", _port);
  _serializer = std::make_shared<Network::DefaultMessageSerializer>();
  _adapter = std::make_unique<Network::ReliableLayerAdapter>(_transport,
                                                             _serializer, 1200);
  _rooms = std::make_unique<Network::RoomManager>(
      _start_x, _start_y, _speed, _max_players, _max_rooms, _timeout,
      _screen_width, _screen_height, _snapshot_interval, _startTime);

  _mainRoomId = _rooms->createRoom();
  Logger::info("[Server] Created main room #" + std::to_string(_mainRoomId));

  _handlers[Network::NEW_CLIENT] = [this](const Network::Packet &pkt,
                                          const Network::endpoint_t &from) {
    handleNewClient(pkt, from);
  };

  _handlers[Network::CLIENT_INPUT] = [this](const Network::Packet &pkt,
                                            const Network::endpoint_t &from) {
    std::string key = from.address + ":" + std::to_string(from.port);
    std::lock_guard<std::mutex> lk(_clientsMtx);
    auto it = _knownClients.find(key);
    if (it != _knownClients.end()) {
      uint32_t roomId = it->second.first;
      _rooms->onPacket(roomId, from, pkt);
    }
  };

  _handlers[Network::ENTITY_DESPAWN] = [this](const Network::Packet &pkt,
                                              const Network::endpoint_t &from) {
    handleDisconnect(pkt, from);
  };

  _adapter->setAppPacketCallback(
      [this](const Network::Packet &pkt, const Network::endpoint_t &from) {
        onAppPacket(pkt, from);
      });

  _transport->start();
  Logger::info("[Server] Network system started on port " +
               std::to_string(_port));
}

void System::ServerNetworkSystem::update(Ecs::Registry &, double dt) {
  _adapter->tick(dt);
  _rooms->tick(dt);

  auto rooms = _rooms->listRooms();
  for (auto roomId : rooms) {
    auto out = _rooms->collectOutgoing(roomId);
    auto &room = _rooms->getRoom(roomId);
    std::lock_guard<std::mutex> rlk(room.mtx);
    for (auto &pkt : out) {
      for (auto &[_, client] : room.clients) {
        if (pkt.header.type == Network::SERVER_SNAPSHOT)
          _adapter->sendUnreliable(client.endpoint, pkt);
        else
          _adapter->sendReliable(client.endpoint, pkt);
      }
    }
  }
}

void System::ServerNetworkSystem::shutdown() {
  Logger::info("[Server] Network system stopped");
}

void System::ServerNetworkSystem::onAppPacket(const Network::Packet &pkt,
                                              const Network::endpoint_t &from) {
  auto it = _handlers.find(pkt.header.type);
  if (it != _handlers.end())
    it->second(pkt, from);
  else
    Logger::warn("[Server] Unknown packet type: " +
                 std::to_string(pkt.header.type));
}

void System::ServerNetworkSystem::handleNewClient(
    const Network::Packet &pkt, const Network::endpoint_t &from) {
  std::string key = from.address + ":" + std::to_string(from.port);

  {
    std::lock_guard<std::mutex> lk(_clientsMtx);
    if (_knownClients.find(key) != _knownClients.end()) {
      Logger::info("[Server] Ignoring duplicate NEW_CLIENT from " + key);
      return;
    }
  }

  Logger::info("[Server] Received NEW_CLIENT from " + key);

  std::string sprite(pkt.payload.begin(), pkt.payload.end());
  Logger::info("[Server] Client sprite: " + sprite);

  auto joinResult = _rooms->joinAuto(from, sprite);
  if (!joinResult.has_value()) {
    Logger::warn("[Server] Could not assign client to any room.");
    return;
  }

  auto [roomId, clientId] = joinResult.value();
  {
    std::lock_guard<std::mutex> lk(_clientsMtx);
    _knownClients[key] = {roomId, clientId};
  }

  Logger::info("[Server] Client joined room #" + std::to_string(roomId) +
               " with ID=" + std::to_string(clientId));

  auto &room = _rooms->getRoom(roomId);
  std::lock_guard<std::mutex> rlk(room.mtx);

  for (auto &[existingId, existingClient] : room.clients) {
    if (existingId == clientId)
      continue;
    Network::Packet spawnExisting;
    spawnExisting.header.type = Network::ENTITY_SPAWN;
    Network::write_u32_le(spawnExisting.payload, existingId);
    Network::write_u32_le(spawnExisting.payload,
                          static_cast<uint32_t>(existingClient.posX));
    Network::write_u32_le(spawnExisting.payload,
                          static_cast<uint32_t>(existingClient.posY));
    std::string sprite = existingClient.sprite;
    spawnExisting.payload.insert(spawnExisting.payload.end(), sprite.begin(),
                                 sprite.end());
    spawnExisting.header.length =
        static_cast<uint16_t>(spawnExisting.payload.size());
    _adapter->sendReliable(from, spawnExisting);
    Logger::info("[Server] Sent ENTITY_SPAWN for client " +
                 std::to_string(existingId) + " to new client " +
                 std::to_string(clientId));
  }

  Network::Packet spawnNew;
  spawnNew.header.type = Network::ENTITY_SPAWN;
  Network::write_u32_le(spawnNew.payload, clientId);
  uint32_t x = 100;
  uint32_t y = 200;
  std::string newClientSprite = sprite;
  Network::write_u32_le(spawnNew.payload, x);
  Network::write_u32_le(spawnNew.payload, y);
  spawnNew.payload.insert(spawnNew.payload.end(), newClientSprite.begin(),
                          newClientSprite.end());
  spawnNew.header.length = static_cast<uint16_t>(spawnNew.payload.size());
  for (auto &[existingId, existingClient] : room.clients) {
    if (existingId == clientId)
      continue;
    _adapter->sendReliable(existingClient.endpoint, spawnNew);
    Logger::info("[Server] Sent ENTITY_SPAWN for new client " +
                 std::to_string(clientId) + " to existing client " +
                 std::to_string(existingId));
  }

  _adapter->sendReliable(from, spawnNew);
  Logger::info("[Server] Sent ENTITY_SPAWN to new client " + key);
}

void System::ServerNetworkSystem::handleDisconnect(
    const Network::Packet &pkt, const Network::endpoint_t &from) {
  std::string key = from.address + ":" + std::to_string(from.port);

  std::pair<uint32_t, uint32_t> rc;
  {
    std::lock_guard<std::mutex> lk(_clientsMtx);
    auto it = _knownClients.find(key);
    if (it == _knownClients.end()) {
      Logger::warn("[Server] Received disconnect from unknown client " + key);
      return;
    }
    rc = it->second;
    _knownClients.erase(it);
  }

  uint32_t roomId = rc.first;
  uint32_t clientId = rc.second;
  Logger::info("[Server] Client " + key +
               " disconnecting (room=" + std::to_string(roomId) +
               ", id=" + std::to_string(clientId) + ")");

  _rooms->leave(roomId, clientId);
}

extern "C" std::shared_ptr<System::ISystem>
createServerNetworkSystem(std::any params) {
  try {
    auto vec = std::any_cast<std::vector<std::any>>(params);
    auto port = std::any_cast<int>(vec[0]);
    return std::make_shared<System::ServerNetworkSystem>(
        static_cast<unsigned short>(port));
  } catch (const std::exception &e) {
    Logger::error(
        std::string("[Factory]: Failed to create ServerNetworkSystem: ") +
        e.what());
  }
  return nullptr;
}
