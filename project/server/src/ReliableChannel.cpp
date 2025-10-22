/*
** EPITECH PROJECT, 2025
** repo
** File description:
** ReliableChannel
*/

#include "ReliableChannel.hpp"

void Server::ReliableChannel::sendReliable(Network::Packet& pack)
{
    pack.header.seq = _nextSeq;
    _nextSeq += 1;
    _sendBuffer[pack.header.seq] = pack;
}

void Server::ReliableChannel::onAck(uint32_t seq)
{
    _sendBuffer.erase(seq);
}

std::vector<Network::Packet> Server::ReliableChannel::getPendingResend()
{
    std::vector<Network::Packet> out;
    for (auto& [seq, pkt] : _sendBuffer) {
        out.push_back(pkt);
    }
    return out;
}
