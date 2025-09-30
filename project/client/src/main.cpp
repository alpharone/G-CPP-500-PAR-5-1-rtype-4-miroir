/*
** EPITECH PROJECT, 2025
** Client
** File description:
** main.cpp
*/

// #include "MessageHeader.hpp"
// #include "MessageType.hpp"
// #include "Packets.hpp"
// #include <asio.hpp>
// #include <iostream>

// int main() {
//     asio::io_context ctx;
//     asio::ip::udp::socket sock(ctx, asio::ip::udp::endpoint(asio::ip::udp::v4(), 0));
//     asio::ip::udp::endpoint server(asio::ip::make_address("127.0.0.1"), 4242);

//     Network::Packet p;
//     p.header.type = Network::NEW_CLIENT;
//     p.header.length = 0;
//     p.header.seq = 1;
//     p.header.flags = 0;
//     auto data = p.serialize();
//     sock.send_to(asio::buffer(data), server);
//     std::cout << "[Client] Sent NEW_CLIENT\n";

//     std::array<uint8_t, 1024> buf;
//     asio::ip::udp::endpoint sender;
//     size_t len = sock.receive_from(asio::buffer(buf), sender);

//     auto reply = Network::Packet::deserialize(buf.data(), len);
//     if (reply.header.type == Network::ACCEPT_CLIENT) {
//         uint32_t clientId;
//         std::memcpy(&clientId, reply.payload.data(), sizeof(uint32_t));
//         std::cout << "[Client] Got ACCEPT_CLIENT -> id=" << clientId << "\n";
//     }

#include "ClientApp.hpp"

int main()
{
    ClientApp app;

    if (!app.init())
        return -1;
    app.mainLoop();
    app.shutdown();
    return 0;
}
