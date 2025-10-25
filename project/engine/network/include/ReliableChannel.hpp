/*
** EPITECH PROJECT, 2025
** repo
** File description:
** ReliableChannel
*/

#pragma once

#include <chrono>
#include <cstdint>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <vector>

namespace Network {

struct pending_msg_t {
  uint32_t seq;
  std::vector<uint8_t> payload;
  std::vector<std::vector<uint8_t>> fragments;
  std::chrono::steady_clock::time_point lastSend;
  unsigned retries = 0;
  bool acked = false;
};

class ReliableChannel {
public:
  struct outgoing_datagram_t {
    uint32_t seq;
    std::vector<uint8_t> bytes;
  };

  struct incoming_reassembly_t {
    uint16_t fragmentCount = 0;
    std::unordered_map<uint16_t, std::vector<uint8_t>> parts;
    std::chrono::steady_clock::time_point firstSeen;
  };

  ReliableChannel(size_t mtu = 1200, double rto = 0.25, unsigned maxRetries = 5,
                  size_t window = 32);
  ~ReliableChannel() = default;

  uint32_t queueReliable(const std::vector<uint8_t> &payload);
  std::vector<outgoing_datagram_t> collectOutgoing();
  void onAck(uint32_t seq);
  void tick(double dt);

  std::optional<std::pair<uint32_t, std::vector<uint8_t>>>
  onIncomingFragmentPayload(const uint8_t *data, size_t size);
  bool isPending(uint32_t seq);
  size_t pendingCount() const;

private:
  std::vector<std::vector<uint8_t>>
  buildFragments(uint32_t parentSeq, const std::vector<uint8_t> &payload);
  std::vector<uint8_t> buildFragmentPayload(uint32_t parentSeq, uint16_t fragId,
                                            uint16_t fragCount,
                                            const uint8_t *data, size_t len);
  static std::vector<uint8_t> buildAckPayload(uint32_t seq);

  mutable std::mutex _mtx;
  size_t _mtu;
  double _rto;
  unsigned _maxRetries;
  size_t _windowSize;

  uint32_t _nextSeq;
  std::unordered_map<uint32_t, pending_msg_t> _pendings;
  std::vector<outgoing_datagram_t> _outgoing;
  std::unordered_map<uint32_t, incoming_reassembly_t> _reassembly;
};

} // namespace Network