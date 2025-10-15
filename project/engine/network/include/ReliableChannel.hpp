/*
** EPITECH PROJECT, 2025
** repo
** File description:
** ReliableChannel
*/

#pragma once

#include <chrono>
#include <deque>
#include <map>
#include <vector>
#include <cstdint>
#include <cstring>
#include <bits/algorithmfwd.h>

struct PendingMsg {
    uint32_t seq;
    std::vector<uint8_t> data;
    std::chrono::steady_clock::time_point lastSent;
    int tries = 0;
};

class ReliableChannel {
    public:
        ReliableChannel();
        uint32_t pushMessage(const std::vector<uint8_t>& data);
        std::vector<std::vector<uint8_t>> collectToSend(std::chrono::milliseconds resendAfter);
        void ack(uint32_t seq);
        size_t pendingCount() const;
    
    private:
        uint32_t _nextSeq;
        std::map<uint32_t, PendingMsg> _pending;
        std::vector<uint32_t> _order;
};
