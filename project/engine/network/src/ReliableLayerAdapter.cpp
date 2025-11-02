/*
** EPITECH PROJECT, 2025
** repo
** File description:
** ReliableLayerAdapter
*/

#include "ReliableLayerAdapter.hpp"
#include "Logger.hpp"
#include "MessageType.hpp"
#include "Utils.hpp"

#include <optional>

namespace Network {

ReliableLayerAdapter::ReliableLayerAdapter(
    std::shared_ptr<INetworkTransport> transport,
    std::shared_ptr<IMessageSerializer> serializer, size_t mtu)
    : _transport(std::move(transport)), _serializer(std::move(serializer)),
      _mtu(mtu) {
  if (!_transport) {
    Logger::error("[Adapter] Constructed with null transport!");
    throw std::runtime_error("ReliableLayerAdapter: transport is null");
  }
  if (!_serializer) {
    Logger::error("[Adapter] Constructed with null serializer!");
    throw std::runtime_error("ReliableLayerAdapter: serializer is null");
  }

  _transport->setRecvCallback(
      [this](const std::vector<uint8_t> &data, const endpoint_t &from) {
        this->onTransportRecv(data, from);
      });
}

void ReliableLayerAdapter::setAppPacketCallback(AppPacketCallback cb) {
  _deliverCb = std::move(cb);
}

uint32_t ReliableLayerAdapter::sendReliable(const endpoint_t &to,
                                            const Network::Packet &pkt) {
  std::string key = to.address + ":" + std::to_string(to.port);

  std::shared_ptr<peer_channel_t> peerPtr;
  {
    std::lock_guard<std::mutex> lk(_mtx);
    auto it = _peers.find(key);
    if (it == _peers.end()) {
      auto p = std::make_shared<peer_channel_t>(_mtu);
      p->endpoint = to;
      p->lastActive = std::chrono::steady_clock::now();
      auto res = _peers.emplace(key, p);
      peerPtr = res.first->second;
    } else {
      peerPtr = it->second;
      if (peerPtr->lastActive.time_since_epoch().count() == 0)
        peerPtr->lastActive = std::chrono::steady_clock::now();
    }
  }

  if (!peerPtr) {
    Logger::warn("[Adapter] sendReliable: failed to get/create peer for " +
                 key);
    return 0;
  }

  std::vector<uint8_t> bytes;
  try {
    bytes = _serializer->serialize(pkt);
  } catch (const std::exception &e) {
    Logger::warn(std::string("[Adapter] Serialization failed: ") + e.what());
    return 0;
  }

  uint32_t seq = 0;
  try {
    seq = peerPtr->channel.queueReliable(bytes);
  } catch (const std::exception &e) {
    Logger::warn(std::string("[Adapter] queueReliable threw: ") + e.what());
    return 0;
  }

  if (seq == 0)
    return 0;

  auto outs = peerPtr->channel.collectOutgoing();

  for (auto &dg : outs) {
    if (_transport) {
      _transport->sendTo(peerPtr->endpoint, dg.bytes);
    } else {
      Logger::warn("[Adapter] transport is null while sending");
    }
  }

  return seq;
}

void ReliableLayerAdapter::sendUnreliable(const endpoint_t &to,
                                          const Network::Packet &pkt) {
  std::vector<uint8_t> raw;
  try {
    raw = _serializer->serialize(pkt);
  } catch (const std::exception &e) {
    Logger::warn(std::string("[Adapter] serialize unreliable failed: ") +
                 e.what());
    return;
  }
  if (_transport)
    _transport->sendTo(to, raw);
  else
    Logger::warn("[Adapter] sendUnreliable: transport null");
}

void ReliableLayerAdapter::tick(double dt) {
  std::vector<std::shared_ptr<peer_channel_t>> peersSnapshot;
  peersSnapshot.reserve(_peers.size());
  {
    std::lock_guard<std::mutex> lk(_mtx);
    for (auto &kv : _peers)
      peersSnapshot.push_back(kv.second);
  }

  for (auto &peerPtr : peersSnapshot) {
    if (!peerPtr)
      continue;

    peerPtr->channel.tick(dt);

    auto outs = peerPtr->channel.collectOutgoing();

    for (auto &dg : outs) {
      if (_transport)
        _transport->sendTo(peerPtr->endpoint, dg.bytes);
      else
        Logger::warn("[Adapter] tick: transport null while sending");
    }
  }
}

void ReliableLayerAdapter::onTransportRecv(const std::vector<uint8_t> &data,
                                           const endpoint_t &from) {
  if (data.empty())
    return;

  std::string key = from.address + ":" + std::to_string(from.port);

  std::shared_ptr<peer_channel_t> peerPtr;
  {
    std::lock_guard<std::mutex> lk(_mtx);
    auto it = _peers.find(key);
    if (it == _peers.end()) {
      auto p = std::make_shared<peer_channel_t>(_mtu);
      p->endpoint = from;
      p->lastActive = std::chrono::steady_clock::now();
      auto res = _peers.emplace(key, p);
      peerPtr = res.first->second;
    } else {
      peerPtr = it->second;
      peerPtr->lastActive = std::chrono::steady_clock::now();
    }
  }

  if (!peerPtr) {
    Logger::warn("[Adapter] onTransportRecv: no peer for " + key);
    return;
  }

  uint8_t msgType = data[0];

  try {
    Network::Packet pkt;

    if (msgType == static_cast<uint8_t>(Network::RELIABLE_FRAGMENT)) {
      if (data.size() < 7) {
        Logger::warn("[Adapter] Reliable fragment too short");
        return;
      }

      uint16_t fragLen = Network::read_u16_le(data.data(), data.size(), 1);
      uint32_t wrapperSeq = Network::read_u32_le(data.data(), data.size(), 3);

      const uint8_t *fragPayloadPtr = data.data() + 7;
      size_t fragPayloadLen = data.size() - 7;

      if (fragLen != fragPayloadLen) {
        Logger::warn("[Adapter] fragment length mismatch: header=" +
                     std::to_string(fragLen) +
                     " actual=" + std::to_string(fragPayloadLen));
      }

      auto opt = peerPtr->channel.onIncomingFragmentPayload(fragPayloadPtr,
                                                            fragPayloadLen);
      if (opt) {
        auto parentSeq = opt->first;
        auto &assembled = opt->second;
        pkt = _serializer->deserialize(assembled.data(), assembled.size());
        if (_deliverCb)
          _deliverCb(pkt, from);

        if (_transport) {
          std::vector<uint8_t> ack;
          ack.reserve(1 + 4);
          ack.push_back(static_cast<uint8_t>(Network::ACK));
          Network::write_u32_le(ack, parentSeq);
          _transport->sendTo(from, ack);
          Logger::debug("[Adapter] Sent ACK for seq=" +
                        std::to_string(parentSeq));
        }
      } else {
        Logger::info("[Adapter] Fragment stored, waiting for other fragments");
      }
      return;
    }

    if (msgType == static_cast<uint8_t>(Network::ACK)) {
      if (data.size() >= 1 + 4) {
        uint32_t ackSeq = Network::read_u32_le(data.data(), data.size(), 1);
        Logger::debug("[Adapter] Received ACK for seq=" +
                      std::to_string(ackSeq));
        peerPtr->channel.onAck(ackSeq);
      } else {
        Logger::warn("[Adapter] ACK packet too short");
      }
      return;
    }

    pkt = _serializer->deserialize(data.data(), data.size());
    if (_deliverCb)
      _deliverCb(pkt, from);
  } catch (const std::exception &e) {
    Logger::warn(std::string("[Adapter] Deserialize/processing failed: ") +
                 e.what());
    return;
  }
}

} // namespace Network
