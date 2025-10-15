/*
** EPITECH PROJECT, 2025
** repo
** File description:
** ReliableChannel
*/

#pragma once

#include <map>
#include "Packets.hpp"

namespace Server {

    class ReliableChannel {
        public:
            ReliableChannel() = default;
            ~ReliableChannel() = default;
            void sendReliable(Network::Packet& pack);
            void onAck(uint32_t seq);
            std::vector<Network::Packet> getPendingResend();

        private:
            uint32_t _nextSeq{1};
            std::map<uint32_t, Network::Packet> _sendBuffer;
    };

}