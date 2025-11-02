/*
** EPITECH PROJECT, 2025
** repo
** File description:
** ServerNetworkSystem
*/

#include "ServerNetworkSystem.hpp"
#include "Animation.hpp"
#include "AsioNetworkTransport.hpp"
#include "DefaultMessageSerializer.hpp"
#include "Drawable.hpp"
#include "EnemyAI.hpp"
#include "MessageType.hpp"
#include "Position.hpp"
#include "ReliableLayerAdapter.hpp"
#include "Utils.hpp"
#include "Velocity.hpp"

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
  Network::room_manager_config_t config{
      _start_x, _start_y,      _speed,         _max_players,       _max_rooms,
      _timeout, _screen_width, _screen_height, _snapshot_interval, _startTime};
  _rooms = std::make_unique<Network::RoomManager>(config);

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

void System::ServerNetworkSystem::update(Ecs::Registry &registry, double dt) {
  _adapter->tick(dt);
  _rooms->tick(dt);

  checkForNewEntities(registry);

  updateSnapshotsWithEntities(registry);

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

  Logger::info("[Server] Received NEW_CLIENT from " + key +
               " payload size: " + std::to_string(pkt.payload.size()));

  size_t offset = 0;
  std::string sprite;
  while (offset < pkt.payload.size() && pkt.payload[offset] != '\0') {
    sprite += pkt.payload[offset];
    offset++;
  }
  offset++;

  Logger::info("[Server] Parsed sprite: '" + sprite +
               "' offset after sprite: " + std::to_string(offset));

  if (offset + 24 > pkt.payload.size()) {
    Logger::warn(
        "[Server] Invalid NEW_CLIENT payload size: expected at least " +
        std::to_string(offset + 24) + " but got " +
        std::to_string(pkt.payload.size()));
    return;
  }

  int frame_x = static_cast<int>(
      Network::read_u32_le(pkt.payload.data(), pkt.payload.size(), offset));
  offset += 4;
  int frame_y = static_cast<int>(
      Network::read_u32_le(pkt.payload.data(), pkt.payload.size(), offset));
  offset += 4;
  int frame_w = static_cast<int>(
      Network::read_u32_le(pkt.payload.data(), pkt.payload.size(), offset));
  offset += 4;
  int frame_h = static_cast<int>(
      Network::read_u32_le(pkt.payload.data(), pkt.payload.size(), offset));
  offset += 4;
  int frame_count = static_cast<int>(
      Network::read_u32_le(pkt.payload.data(), pkt.payload.size(), offset));
  offset += 4;
  uint32_t frameTimeBits =
      Network::read_u32_le(pkt.payload.data(), pkt.payload.size(), offset);
  float frame_time;
  std::memcpy(&frame_time, &frameTimeBits, sizeof(float));

  Logger::info("[Server] Client sprite: " + sprite +
               " animation config: " + std::to_string(frame_x) + "," +
               std::to_string(frame_y) + " " + std::to_string(frame_w) + "x" +
               std::to_string(frame_h) + " " + std::to_string(frame_count) +
               " frames, " + std::to_string(frame_time) + "s");

  Network::client_join_info_t info{sprite,  frame_x,     frame_y,   frame_w,
                                   frame_h, frame_count, frame_time};
  auto joinResult = _rooms->joinAuto(from, info);
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
    spawnExisting.payload.push_back('\0');

    Network::write_u32_le(spawnExisting.payload,
                          static_cast<uint32_t>(existingClient.frame_x));
    Network::write_u32_le(spawnExisting.payload,
                          static_cast<uint32_t>(existingClient.frame_y));
    Network::write_u32_le(spawnExisting.payload,
                          static_cast<uint32_t>(existingClient.frame_w));
    Network::write_u32_le(spawnExisting.payload,
                          static_cast<uint32_t>(existingClient.frame_h));
    Network::write_u32_le(spawnExisting.payload,
                          static_cast<uint32_t>(existingClient.frame_count));
    uint32_t frameTimeBits;
    std::memcpy(&frameTimeBits, &existingClient.frame_time, sizeof(float));
    Network::write_u32_le(spawnExisting.payload, frameTimeBits);
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
  spawnNew.payload.push_back('\0');

  Network::write_u32_le(spawnNew.payload, static_cast<uint32_t>(frame_x));
  Network::write_u32_le(spawnNew.payload, static_cast<uint32_t>(frame_y));
  Network::write_u32_le(spawnNew.payload, static_cast<uint32_t>(frame_w));
  Network::write_u32_le(spawnNew.payload, static_cast<uint32_t>(frame_h));
  Network::write_u32_le(spawnNew.payload, static_cast<uint32_t>(frame_count));
  uint32_t newFrameTimeBits;
  std::memcpy(&newFrameTimeBits, &frame_time, sizeof(float));
  Network::write_u32_le(spawnNew.payload, newFrameTimeBits);
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

void System::ServerNetworkSystem::checkForNewEntities(Ecs::Registry &registry) {
  auto &positions = registry.getComponents<Component::position_t>();
  auto &drawables = registry.getComponents<Component::drawable_t>();
  auto &animations = registry.getComponents<Component::animation_t>();
  auto &enemyAIs = registry.getComponents<Component::enemy_ai_t>();

  std::lock_guard<std::mutex> lk(_entitiesMtx);

  for (size_t i = 0; i < positions.size() && i < drawables.size() &&
                     i < animations.size() && i < enemyAIs.size();
       ++i) {
    Ecs::Entity entity(i);

    auto posOpt = positions[i];
    auto drawableOpt = drawables[i];
    auto animOpt = animations[i];
    auto enemyAIOpt = enemyAIs[i];

    if (!posOpt.has_value() || !drawableOpt.has_value() ||
        !animOpt.has_value() || !enemyAIOpt.has_value()) {
      continue;
    }

    if (_trackedEntities.find(i) == _trackedEntities.end()) {
      _trackedEntities.insert(i);
      _lastEntityPositions[i] = {posOpt->x, posOpt->y};

      Network::Packet spawnPacket;
      spawnPacket.header.type = Network::ENTITY_SPAWN;

      Network::write_u32_le(spawnPacket.payload,
                            static_cast<uint32_t>(1000 + i));

      uint32_t xInt;
      std::memcpy(&xInt, &posOpt->x, sizeof(float));
      Network::write_u32_le(spawnPacket.payload, xInt);

      uint32_t yInt;
      std::memcpy(&yInt, &posOpt->y, sizeof(float));
      Network::write_u32_le(spawnPacket.payload, yInt);

      std::string sprite = drawableOpt->meta.count("sprite_path")
                               ? drawableOpt->meta.at("sprite_path")
                               : "";
      spawnPacket.payload.insert(spawnPacket.payload.end(), sprite.begin(),
                                 sprite.end());
      spawnPacket.payload.push_back('\0');

      Network::write_u32_le(spawnPacket.payload,
                            static_cast<uint32_t>(animOpt->startX));
      Network::write_u32_le(spawnPacket.payload,
                            static_cast<uint32_t>(animOpt->startY));
      Network::write_u32_le(spawnPacket.payload,
                            static_cast<uint32_t>(animOpt->frameW));
      Network::write_u32_le(spawnPacket.payload,
                            static_cast<uint32_t>(animOpt->frameH));
      Network::write_u32_le(spawnPacket.payload,
                            static_cast<uint32_t>(animOpt->frameCount));

      uint32_t frameTimeBits;
      float frameTime = 1.0f / animOpt->fps;
      std::memcpy(&frameTimeBits, &frameTime, sizeof(float));
      Network::write_u32_le(spawnPacket.payload, frameTimeBits);

      spawnPacket.header.length =
          static_cast<uint16_t>(spawnPacket.payload.size());

      auto rooms = _rooms->listRooms();
      for (auto roomId : rooms) {
        auto &room = _rooms->getRoom(roomId);
        std::lock_guard<std::mutex> rlk(room.mtx);
        for (auto &[_, client] : room.clients) {
          _adapter->sendReliable(client.endpoint, spawnPacket);
        }
      }

      Logger::info("[Server] Sent ENTITY_SPAWN for enemy entity " +
                   std::to_string(i));
    }
  }
}

void System::ServerNetworkSystem::updateSnapshotsWithEntities(
    Ecs::Registry &registry) {
  auto &positions = registry.getComponents<Component::position_t>();

  std::lock_guard<std::mutex> lk(_entitiesMtx);

  auto rooms = _rooms->listRooms();
  for (auto roomId : rooms) {
    auto &room = _rooms->getRoom(roomId);
    std::lock_guard<std::mutex> rlk(room.mtx);

    Network::Packet snap;
    snap.header.type = Network::SERVER_SNAPSHOT;

    double timestamp = std::chrono::duration<double>(
                           std::chrono::steady_clock::now() - _startTime)
                           .count();
    uint64_t timestampInt;
    std::memcpy(&timestampInt, &timestamp, sizeof(double));
    Network::write_u64_le(snap.payload, timestampInt);

    for (auto &[cid, client] : room.clients) {
      if (client.posX != client.lastSnapX || client.posY != client.lastSnapY) {
        client.lastSnapX = client.posX;
        client.lastSnapY = client.posY;
      }

      Network::write_u32_le(snap.payload, cid);

      uint32_t xInt;
      std::memcpy(&xInt, &client.posX, sizeof(float));
      Network::write_u32_le(snap.payload, xInt);

      uint32_t yInt;
      std::memcpy(&yInt, &client.posY, sizeof(float));
      Network::write_u32_le(snap.payload, yInt);
    }

    for (auto entity : _trackedEntities) {
      size_t entityIndex = static_cast<size_t>(entity);
      if (entityIndex < positions.size()) {
        auto posOpt = positions[entityIndex];
        if (posOpt.has_value()) {
          auto lastPosIt = _lastEntityPositions.find(entity);
          if (lastPosIt == _lastEntityPositions.end() ||
              lastPosIt->second.first != posOpt->x ||
              lastPosIt->second.second != posOpt->y) {

            _lastEntityPositions[entity] = {posOpt->x, posOpt->y};

            Network::write_u32_le(snap.payload,
                                  static_cast<uint32_t>(1000 + entityIndex));

            uint32_t xInt;
            std::memcpy(&xInt, &posOpt->x, sizeof(float));
            Network::write_u32_le(snap.payload, xInt);

            uint32_t yInt;
            std::memcpy(&yInt, &posOpt->y, sizeof(float));
            Network::write_u32_le(snap.payload, yInt);
          }
        }
      }
    }

    snap.header.length = static_cast<uint16_t>(snap.payload.size());
    room.outgoing.push_back(std::move(snap));
  }
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
