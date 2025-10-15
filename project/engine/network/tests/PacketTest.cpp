/*
** EPITECH PROJECT, 2025
** repo
** File description:
** PacketTest
*/

// #define CATCH_CONFIG_MAIN
// #include <catch2/catch_test_macros.hpp>
// #include "MessageType.hpp"
// #include "NetworkContext.hpp"
// #include "ReliableChannel.hpp"
// #include "Packets.hpp"
// #include <chrono>
// #include <thread>

// using namespace std::chrono_literals;

// TEST_CASE("Packet serialize/deserialize roundtrip", "[Network][Packet]") {
//     Network::Packet p;
//     p.header.type = static_cast<uint8_t>(Network::MessageType::PLAYER_INPUT);
//     p.header.length = 3;
//     p.header.seq = 0x11223344;
//     p.header.flags = 0x7;
//     p.payload = { 1, 2, 3 };

//     auto data = p.serialize();
//     REQUIRE(data.size() == sizeof(Network::MessageHeader) + 3);

//     auto p2 = Network::Packet::deserialize(data.data(), data.size());
//     REQUIRE(p2.header.type == p.header.type);
//     REQUIRE(p2.header.length == p.header.length);
//     REQUIRE(p2.header.seq == p.header.seq);
//     REQUIRE(p2.header.flags == p.header.flags);
//     REQUIRE(p2.payload == p.payload);
// }

// TEST_CASE("ReliableChannel push/collect/ack/pendingCount", "[Network][ReliableChannel]") {
//     ReliableChannel rc;

//     REQUIRE(rc.pendingCount() == 0);

//     std::vector<uint8_t> d1 = {10,11,12};
//     std::vector<uint8_t> d2 = {20,21};

//     uint32_t s1 = rc.pushMessage(d1);
//     uint32_t s2 = rc.pushMessage(d2);

//     REQUIRE(rc.pendingCount() == 2);
//     REQUIRE(s1 != s2);

//     auto frames1 = rc.collectToSend(1000ms);
//     REQUIRE(frames1.size() == 2);
//     auto extract_seq = [](const std::vector<uint8_t>& frame)->uint32_t {
//         uint32_t seq = 0;
//         std::memcpy(&seq, frame.data(), sizeof(uint32_t));
//         return seq;
//     };
//     REQUIRE((extract_seq(frames1[0]) == s1 || extract_seq(frames1[0]) == s2));
//     REQUIRE((extract_seq(frames1[1]) == s1 || extract_seq(frames1[1]) == s2));
//     REQUIRE(rc.pendingCount() == 2);

//     auto frames2 = rc.collectToSend(1000ms);
//     REQUIRE(frames2.size() == 0);

//     auto frames3 = rc.collectToSend(0ms);
//     REQUIRE(frames3.size() == 2);

//     rc.ack(s1);
//     REQUIRE(rc.pendingCount() == 1);

//     rc.ack(s2);
//     REQUIRE(rc.pendingCount() == 0);
// }

// TEST_CASE("ReliableChannel order preserved and tries increment", "[Network][ReliableChannel]") {
//     ReliableChannel rc;
//     uint32_t s1 = rc.pushMessage({1});
//     uint32_t s2 = rc.pushMessage({2});
//     auto f1 = rc.collectToSend(1000ms);
//     REQUIRE(f1.size() == 2);
//     auto f2 = rc.collectToSend(0ms);
//     REQUIRE(f2.size() == 2);
//     rc.ack(s1);
//     auto f3 = rc.collectToSend(0ms);
//     REQUIRE(f3.size() == 1);
// }
