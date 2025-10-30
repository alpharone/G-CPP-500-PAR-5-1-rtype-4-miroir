/*
** EPITECH PROJECT, 2025
** repo
** File description:
** ServerNetworkSystem
*/

#include "ClientNetworkSystem.hpp"
#include "Animation.hpp"
#include "AsioNetworkTransport.hpp"
#include "ClientConfig.hpp"
#include "DefaultMessageSerializer.hpp"
#include "Drawable.hpp"
#include "MessageType.hpp"
#include "Position.hpp"
#include "Utils.hpp"
#include <cstdlib>

System::ClientNetworkSystem::ClientNetworkSystem(
    const ClientNetworkConfig &config)
    : _ctx(std::move(config.ctx)),
      _lastPacketTime(std::chrono::steady_clock::now()),
      _player_sprite(config.sprite), _player_frame_w(config.frameW),
      _player_frame_h(config.frameH), _player_frame_count(config.frameCount),
      _player_frame_time(config.frameTime), _player_frame_x(config.frameX),
      _player_frame_y(config.frameY) {
  _server = {config.host, config.port};

  _ctx->serverEndpoint =
      asio::ip::udp::endpoint(asio::ip::make_address(config.host), config.port);
}

void System::ClientNetworkSystem::init(Ecs::Registry &registry) {
  std::lock_guard<std::mutex> lk(_mutex);

  if (_initialized.exchange(true)) {
    Logger::warn("[Client] Network system already initialized");
    return;
  }

  _ctx->registry = &registry;

  _transport = std::make_shared<Network::AsioNetworkTransport>("0.0.0.0", 0);
  _serializer = std::make_shared<Network::DefaultMessageSerializer>();
  _adapter = std::make_shared<Network::ReliableLayerAdapter>(_transport,
                                                             _serializer, 1200);
  _ctx->adapter = _adapter;

  _handlers[Network::ACCEPT_CLIENT] = [this](const Network::Packet &pkt,
                                             const Network::endpoint_t &from) {
    handleAcceptClient(pkt, from);
  };

  _handlers[Network::ENTITY_SPAWN] = [this](const Network::Packet &pkt,
                                            const Network::endpoint_t &from) {
    handleEntitySpawn(pkt, from);
  };

  _handlers[Network::ENTITY_DESPAWN] = [this](const Network::Packet &pkt,
                                              const Network::endpoint_t &from) {
    handleEntityDespawn(pkt, from);
  };

  _handlers[Network::GAME_START] = [this](const Network::Packet &pkt,
                                          const Network::endpoint_t &from) {
    handleGameStart(pkt, from);
  };

  _handlers[Network::SERVER_SNAPSHOT] =
      [this](const Network::Packet &pkt, const Network::endpoint_t &from) {
        handleServerSnapshot(pkt, from);
      };

  _adapter->setAppPacketCallback(
      [this](const Network::Packet &pkt, const Network::endpoint_t &) {
        _ctx->pushPacket(pkt);
      });

  _transport->start();

  if (!_ctx->connected) {
    Network::Packet newClient;
    newClient.header.type = Network::NEW_CLIENT;

    newClient.payload.assign(_player_sprite.begin(), _player_sprite.end());
    newClient.payload.push_back('\0');

    Network::write_u32_le(newClient.payload,
                          static_cast<uint32_t>(_player_frame_x));
    Network::write_u32_le(newClient.payload,
                          static_cast<uint32_t>(_player_frame_y));
    Network::write_u32_le(newClient.payload,
                          static_cast<uint32_t>(_player_frame_w));
    Network::write_u32_le(newClient.payload,
                          static_cast<uint32_t>(_player_frame_h));
    Network::write_u32_le(newClient.payload,
                          static_cast<uint32_t>(_player_frame_count));
    uint32_t frameTimeBits;
    std::memcpy(&frameTimeBits, &_player_frame_time, sizeof(float));
    Network::write_u32_le(newClient.payload, frameTimeBits);

    newClient.header.length = static_cast<uint16_t>(newClient.payload.size());

    _adapter->sendReliable(_server, newClient);
    _ctx->connected = true;

    Logger::info("[Client] Connecting to server at " + _server.address + ":" +
                 std::to_string(_server.port) + " with sprite " +
                 _player_sprite + " and animation config");
  } else {
    Logger::warn("[Client] Already connected, skipping NEW_CLIENT send");
  }
}

void System::ClientNetworkSystem::update(Ecs::Registry &registry, double dt) {
  _adapter->tick(dt);

  Network::Packet pkt;
  while (_ctx->popPacket(pkt)) {
    _lastPacketTime = std::chrono::steady_clock::now();
    _reconnecting = false;
    onAppPacket(pkt, _server);
  }

  auto now = std::chrono::steady_clock::now();
  auto elapsed =
      std::chrono::duration_cast<std::chrono::seconds>(now - _lastPacketTime)
          .count();
  if (elapsed > 5 && !_reconnecting) {
    _reconnecting = true;
    Logger::warn(
        "[Client] No packets received for 5s, attempting reconnection...");
    _ctx->connected = false;
    _entityMap.clear();
    _interpolator = Network::SnapshotInterpolator();
    if (_transport)
      _transport->stop();
    init(registry);
  }

  double renderTime = std::chrono::duration<double>(
                          std::chrono::steady_clock::now() - _startTime)
                          .count() -
                      0.05;

  auto states = _interpolator.interpolate(renderTime);

  for (auto &state : states) {
    auto it = _entityMap.find(state.entityId);
    if (it == _entityMap.end())
      continue;
    size_t entityId = it->second;
    auto &pos = registry.getComponents<Component::position_t>();
    if (!pos[entityId].has_value())
      continue;
    pos[entityId]->x = state.x;
    pos[entityId]->y = state.y;
  }
}

void System::ClientNetworkSystem::shutdown() {
  std::lock_guard<std::mutex> lk(_mutex);

  if (!_initialized)
    return;
  _initialized = false;

  if (_ctx->connected) {
    Logger::info("[Client] Sending ENTITY_DESPAWN (disconnect)");
    Network::Packet disconnect;
    disconnect.header.type = Network::ENTITY_DESPAWN;
    Network::write_u32_le(disconnect.payload, _ctx->roomId);
    Network::write_u32_le(disconnect.payload, _ctx->clientId);
    disconnect.header.length = static_cast<uint16_t>(disconnect.payload.size());
    _adapter->sendReliable(_server, disconnect);
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  _ctx->connected = false;

  if (_transport) {
    Logger::info("[Client] Stopping transport...");
    _transport->stop();
  }

  Logger::info("[Client] Disconnected cleanly");
}

void System::ClientNetworkSystem::onAppPacket(const Network::Packet &pkt,
                                              const Network::endpoint_t &from) {
  auto it = _handlers.find(pkt.header.type);
  if (it != _handlers.end()) {
    it->second(pkt, from);
  } else {
    _ctx->pushPacket(pkt);
  }
}

void System::ClientNetworkSystem::handleAcceptClient(
    const Network::Packet &pkt, const Network::endpoint_t &) {
  if (pkt.payload.size() >= 8) {
    _ctx->roomId =
        Network::read_u32_le(pkt.payload.data(), pkt.payload.size(), 0);
    _ctx->clientId =
        Network::read_u32_le(pkt.payload.data(), pkt.payload.size(), 4);
    Logger::info("[Client] Accepted in room #" + std::to_string(_ctx->roomId) +
                 " with ID=" + std::to_string(_ctx->clientId));

    if (_entityMap.count(0)) {
      _entityMap[_ctx->clientId] = _entityMap[0];
      _entityMap.erase(0);
      Logger::info("[Client] Remapped entity from temp id 0 to clientId " +
                   std::to_string(_ctx->clientId));
    }
  } else {
    Logger::warn("[Client] Invalid ACCEPT_CLIENT payload");
  }
}

void System::ClientNetworkSystem::handleEntitySpawn(
    const Network::Packet &pkt, const Network::endpoint_t &) {
  if (pkt.payload.size() < 12) {
    Logger::warn("[Client] Bad ENTITY_SPAWN payload size=" +
                 std::to_string(pkt.payload.size()));
    return;
  }

  uint32_t clientId =
      Network::read_u32_le(pkt.payload.data(), pkt.payload.size(), 0);
  uint32_t x = Network::read_u32_le(pkt.payload.data(), pkt.payload.size(), 4);
  uint32_t y = Network::read_u32_le(pkt.payload.data(), pkt.payload.size(), 8);

  size_t spriteStart = 12;
  size_t spriteEnd = spriteStart;
  while (spriteEnd < pkt.payload.size() && pkt.payload[spriteEnd] != '\0') {
    spriteEnd++;
  }
  std::string spriteName(pkt.payload.begin() + spriteStart,
                         pkt.payload.begin() + spriteEnd);
  size_t offset = spriteEnd + 1;

  auto &registry = *_ctx->registry;
  auto e = registry.spawnEntity();
  registry.emplaceComponent<Component::position_t>(
      e, Component::position_t{static_cast<float>(x), static_cast<float>(y)});
  registry.emplaceComponent<Component::drawable_t>(
      e, Component::drawable_t(spriteName));

  if (offset + 24 <= pkt.payload.size()) {
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

    int fps = static_cast<int>(1.0f / frame_time);
    Component::animation_t playerAnim(frame_w, frame_h, frame_count, fps, true,
                                      frame_x, frame_y, "player");
    registry.emplaceComponent<Component::animation_t>(e, playerAnim);
    Logger::info("[Client] Applied received animation config to entity " +
                 std::to_string(static_cast<size_t>(e)) + ": " +
                 std::to_string(frame_x) + "," + std::to_string(frame_y) + " " +
                 std::to_string(frame_w) + "x" + std::to_string(frame_h) + " " +
                 std::to_string(frame_count) + " frames, " +
                 std::to_string(frame_time) + "s");
  } else {
    int fps = static_cast<int>(1.0f / _player_frame_time);
    Component::animation_t playerAnim(
        _player_frame_w, _player_frame_h, _player_frame_count, fps, true,
        _player_frame_x, _player_frame_y, "player");
    registry.emplaceComponent<Component::animation_t>(e, playerAnim);
    Logger::info("[Client] Applied fallback animation to entity " +
                 std::to_string(static_cast<size_t>(e)) +
                 " (no animation data received)");
  }

  _entityMap[clientId] = e;

  Logger::info("[Client] Spawned entity for clientId=" +
               std::to_string(clientId) + " at (" + std::to_string(x) + ", " +
               std::to_string(y) + ") sprite=" + spriteName);
}

void System::ClientNetworkSystem::handleEntityDespawn(
    const Network::Packet &pkt, const Network::endpoint_t &) {
  if (pkt.payload.size() < 4) {
    Logger::warn("[Client] Bad ENTITY_DESPAWN payload size=" +
                 std::to_string(pkt.payload.size()));
    return;
  }

  uint32_t clientId =
      Network::read_u32_le(pkt.payload.data(), pkt.payload.size(), 0);

  auto it = _entityMap.find(clientId);
  if (it != _entityMap.end()) {
    size_t entityId = it->second;
    auto &registry = *_ctx->registry;
    registry.killEntity(Ecs::Entity(entityId));
    _entityMap.erase(clientId);
    Logger::info("[Client] Despawned entity for clientId=" +
                 std::to_string(clientId));
  } else {
    Logger::warn("[Client] No entity found for despawn clientId=" +
                 std::to_string(clientId));
  }
}

void System::ClientNetworkSystem::handleGameStart(const Network::Packet &pkt,
                                                  const Network::endpoint_t &) {
  if (pkt.payload.size() >= 4) {
    uint32_t seed =
        Network::read_u32_le(pkt.payload.data(), pkt.payload.size(), 0);
    Logger::info("[Client] Game start received, seed=" + std::to_string(seed));
  }
}

void System::ClientNetworkSystem::handleServerSnapshot(
    const Network::Packet &pkt, const Network::endpoint_t &) {
  if (pkt.payload.size() < 12)
    return;

  size_t offset = 0;
  uint64_t timestampInt =
      Network::read_u64_le(pkt.payload.data(), pkt.payload.size(), offset);
  offset += 8;
  double serverTimestamp;
  std::memcpy(&serverTimestamp, &timestampInt, sizeof(double));

  Network::snapshot_t snap;
  snap.timestamp = serverTimestamp;

  while (offset + 12 <= pkt.payload.size()) {
    uint32_t id =
        Network::read_u32_le(pkt.payload.data(), pkt.payload.size(), offset);
    offset += 4;

    uint32_t xInt =
        Network::read_u32_le(pkt.payload.data(), pkt.payload.size(), offset);
    offset += 4;

    uint32_t yInt =
        Network::read_u32_le(pkt.payload.data(), pkt.payload.size(), offset);
    offset += 4;

    float x;
    float y;
    std::memcpy(&x, &xInt, sizeof(float));
    std::memcpy(&y, &yInt, sizeof(float));

    snap.entities.push_back(Network::snapshot_entity_state_t{id, x, y});

    if (_ctx->registry) {
      auto it = _entityMap.find(id);
      if (it != _entityMap.end()) {
        size_t entityId = it->second;
        auto &reg = *_ctx->registry;
        auto &pos = reg.getComponents<Component::position_t>();
        if (pos[entityId].has_value()) {
          pos[entityId]->x = x;
          pos[entityId]->y = y;
        } else {
          Logger::warn("[Client] Position component not found for entity " +
                       std::to_string(entityId));
        }
      } else {
        Logger::warn("[Client] No entity mapped for id " + std::to_string(id));
      }
    }
  }

  if (!_timeSynced) {
    _startTime =
        std::chrono::steady_clock::now() -
        std::chrono::duration_cast<std::chrono::steady_clock::duration>(
            std::chrono::duration<double>(serverTimestamp));
    _timeSynced = true;
    Logger::info("[Client] Time synced with server");
  }

  _interpolator.addSnapshot(snap);
}

extern "C" std::shared_ptr<System::ISystem>
createClientNetworkSystem(std::any params) {
  try {
    auto vec = std::any_cast<std::vector<std::any>>(params);
    System::ClientNetworkConfig config;
    config.host = std::any_cast<std::string>(vec[0]);
    config.port = static_cast<unsigned short>(std::any_cast<int>(vec[1]));
    config.ctx =
        std::any_cast<std::shared_ptr<Network::network_context_t>>(vec[2]);
    config.sprite = std::any_cast<std::string>(vec[3]);
    config.frameW = std::any_cast<int>(vec[4]);
    config.frameH = std::any_cast<int>(vec[5]);
    config.frameCount = std::any_cast<int>(vec[6]);
    config.frameTime = std::any_cast<float>(vec[7]);
    config.frameX = std::any_cast<int>(vec[8]);
    config.frameY = std::any_cast<int>(vec[9]);
    return std::make_shared<System::ClientNetworkSystem>(config);
  } catch (const std::exception &e) {
    Logger::error(
        std::string("[Factory]: Failed to create ClientNetworkSystem: ") +
        e.what());
  }
  return nullptr;
}
