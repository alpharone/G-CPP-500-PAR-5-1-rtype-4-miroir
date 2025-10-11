/*
** EPITECH PROJECT, 2025
** repo
** File description:
** ReliableChannel
*/

#include "ReliableChannel.hpp"

ReliableChannel::ReliableChannel() : _nextSeq(1)
{
}

uint32_t ReliableChannel::pushMessage(const std::vector<uint8_t>& data)
{
    uint32_t seq = _nextSeq++;
    PendingMsg pm{seq, data, std::chrono::steady_clock::now(), 0};

    _pending[seq] = pm;
    _order.push_back(seq);
    return seq;
}

std::vector<std::vector<uint8_t>> ReliableChannel::collectToSend(std::chrono::milliseconds resendAfter)
{
    std::vector<std::vector<uint8_t>> out;
    auto now = std::chrono::steady_clock::now();
    for (auto seq : _order) {
        auto &pm = _pending[seq];
        if (pm.tries == 0 || now - pm.lastSent >= resendAfter) {
            std::vector<uint8_t> frame(sizeof(uint32_t) + pm.data.size());
            std::memcpy(frame.data(), &pm.seq, sizeof(uint32_t));
            if (!pm.data.empty())
                std::memcpy(frame.data() + sizeof(uint32_t), pm.data.data(), pm.data.size());
            out.push_back(std::move(frame));
            pm.lastSent = now;
            pm.tries++;
        }
    }
    return out;
}

void ReliableChannel::ack(uint32_t seq)
{
    auto it = _pending.find(seq);

    if (it != _pending.end()) {
        _pending.erase(it);
        _order.erase(std::remove(_order.begin(), _order.end(), seq), _order.end());
    }
}

size_t ReliableChannel::pendingCount() const
{
    return _pending.size();
}
