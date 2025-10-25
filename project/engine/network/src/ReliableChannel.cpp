/*
** EPITECH PROJECT, 2025
** repo
** File description:
** ReliableChannel
*/

#include "ReliableChannel.hpp"

#include <algorithm>
#include <chrono>
#include <cstring>

#include "MessageType.hpp"
#include "Utils.hpp"

using namespace std::chrono;

Network::ReliableChannel::ReliableChannel(size_t mtu, double rto,
                                          unsigned maxRetries, size_t window)
    : _mtu(mtu), _rto(rto), _maxRetries(maxRetries), _windowSize(window),
      _nextSeq(1) {}

uint32_t
Network::ReliableChannel::queueReliable(const std::vector<uint8_t> &payload) {
  std::lock_guard<std::mutex> lk(_mtx);

  if (_pendings.size() >= _windowSize) {
    return 0;
  }
  uint32_t seq = _nextSeq++;
  pending_msg_t p;
  p.seq = seq;
  p.payload = payload;
  p.fragments = buildFragments(seq, payload);
  p.lastSend = steady_clock::now();
  p.retries = 0;
  p.acked = false;

  for (auto &fragPayload : p.fragments) {
    outgoing_datagram_t dg;
    dg.seq = seq;
    std::vector<uint8_t> dat;
    dat.push_back(Network::RELIABLE_FRAGMENT);
    Network::write_u16_le(dat, static_cast<uint16_t>(fragPayload.size()));
    Network::write_u32_le(dat, seq);
    dat.insert(dat.end(), fragPayload.begin(), fragPayload.end());
    dg.bytes = std::move(dat);
    _outgoing.push_back(std::move(dg));
  }

  _pendings.emplace(seq, std::move(p));
  return seq;
}

std::vector<Network::ReliableChannel::outgoing_datagram_t>
Network::ReliableChannel::collectOutgoing() {
  std::lock_guard<std::mutex> lk(_mtx);
  auto out = std::move(_outgoing);

  _outgoing.clear();
  return out;
}

void Network::ReliableChannel::onAck(uint32_t seq) {
  std::lock_guard<std::mutex> lk(_mtx);
  auto it = _pendings.find(seq);

  if (it != _pendings.end()) {
    it->second.acked = true;
    _pendings.erase(it);
  }
}

void Network::ReliableChannel::tick(double dt) {
  std::lock_guard<std::mutex> lk(_mtx);
  auto now = steady_clock::now();

  for (auto it = _pendings.begin(); it != _pendings.end(); it++) {
    pending_msg_t &p = it->second;
    if (p.acked) {
      it = _pendings.erase(it);
      continue;
    }

    auto elapsed =
        duration_cast<milliseconds>(now - p.lastSend).count() / 1000.0;
    double timeout = _rto * (1ULL << std::min<unsigned>(p.retries, 10));

    if (elapsed >= timeout) {
      if (p.retries >= _maxRetries) {
        p.acked = true;
        it = _pendings.erase(it);
        continue;
      }

      for (auto &fragPayload : p.fragments) {
        outgoing_datagram_t dg;
        dg.seq = p.seq;
        std::vector<uint8_t> dat;
        dat.push_back(Network::RELIABLE_FRAGMENT);
        Network::write_u16_le(dat, static_cast<uint16_t>(fragPayload.size()));
        Network::write_u32_le(dat, p.seq);
        dat.insert(dat.end(), fragPayload.begin(), fragPayload.end());
        dg.bytes = std::move(dat);
        _outgoing.push_back(std::move(dg));
      }

      p.lastSend = now;
      p.retries += 1;
    }
  }

  std::vector<uint32_t> toRemove;
  for (auto &kv : _reassembly) {
    auto &r = kv.second;
    if (duration_cast<seconds>(steady_clock::now() - r.firstSeen).count() > 30)
      toRemove.push_back(kv.first);
  }
  for (auto k : toRemove)
    _reassembly.erase(k);
}

std::optional<std::pair<uint32_t, std::vector<uint8_t>>>
Network::ReliableChannel::onIncomingFragmentPayload(const uint8_t *data,
                                                    size_t size) {
  if (size < 8)
    return std::nullopt;
  uint32_t parent = Network::read_u32_le(data, size, 0);
  uint16_t fragId = Network::read_u16_le(data, size, 4);
  uint16_t fragCount = Network::read_u16_le(data, size, 6);
  const uint8_t *payloadPtr = data + 8;
  size_t payloadLen = size - 8;

  std::lock_guard<std::mutex> lk(_mtx);
  auto &entry = _reassembly[parent];
  if (entry.fragmentCount == 0) {
    entry.fragmentCount = fragCount;
    entry.firstSeen = steady_clock::now();
  }
  entry.parts[fragId] =
      std::vector<uint8_t>(payloadPtr, payloadPtr + payloadLen);

  if (entry.parts.size() == entry.fragmentCount) {
    std::vector<uint8_t> assembled;

    for (uint16_t i = 0; i < entry.fragmentCount; i++) {
      auto it = entry.parts.find(i);
      if (it == entry.parts.end()) {
        return std::nullopt;
      }
      assembled.insert(assembled.end(), it->second.begin(), it->second.end());
    }
    _reassembly.erase(parent);
    return std::make_optional(std::make_pair(parent, std::move(assembled)));
  }
  return std::nullopt;
}

bool Network::ReliableChannel::isPending(uint32_t seq) {
  std::lock_guard<std::mutex> lk(_mtx);

  return _pendings.find(seq) != _pendings.end();
}

size_t Network::ReliableChannel::pendingCount() const {
  std::lock_guard<std::mutex> lk(_mtx);

  return _pendings.size();
}

std::vector<std::vector<uint8_t>>
Network::ReliableChannel::buildFragments(uint32_t parentSeq,
                                         const std::vector<uint8_t> &payload) {
  size_t headerOverhead = 8;
  size_t maxDataPerFragment =
      (_mtu > headerOverhead) ? (_mtu - headerOverhead) : 32;
  size_t total = payload.size();
  uint16_t fragmentCount = static_cast<uint16_t>(
      (total + maxDataPerFragment - 1) / maxDataPerFragment);

  if (fragmentCount == 0)
    fragmentCount = 1;
  std::vector<std::vector<uint8_t>> frags;
  frags.reserve(fragmentCount);
  size_t offset = 0;
  for (uint16_t fid = 0; fid < fragmentCount; fid++) {
    size_t chunk = std::min(maxDataPerFragment, total - offset);
    auto frag = buildFragmentPayload(parentSeq, fid, fragmentCount,
                                     payload.data() + offset, chunk);
    frags.push_back(std::move(frag));
    offset += chunk;
  }
  return frags;
}

std::vector<uint8_t> Network::ReliableChannel::buildFragmentPayload(
    uint32_t parentSeq, uint16_t fragId, uint16_t fragCount,
    const uint8_t *data, size_t len) {
  std::vector<uint8_t> out;

  Network::write_u32_le(out, parentSeq);
  Network::write_u16_le(out, fragId);
  Network::write_u16_le(out, fragCount);
  out.insert(out.end(), data, data + len);
  return out;
}

std::vector<uint8_t> Network::ReliableChannel::buildAckPayload(uint32_t seq) {
  std::vector<uint8_t> out;

  Network::write_u32_le(out, seq);
  return out;
}
