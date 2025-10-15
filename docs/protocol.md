Network Working Group                                         R-Type Project
Request for Comments: R-TYPE-1                                October 2025
Category: Informational


                   R-Type Network Protocol Specification
                   -------------------------------------
                           Version 1.0 – Informational

Abstract

   This document defines the R-Type Network Protocol, a lightweight
   binary protocol designed for real-time client–server synchronization
   in multiplayer games. It specifies message envelopes, message types,
   payload formats, and reliability mechanisms.

---

1.  MESSAGE ENVELOPE

   All messages begin with a common header:

      uint8_t  type;
      uint16_t length;
      uint32_t seq;

   Fields:

      type    = unique identifier (0x01–0xFF)
      length  = payload size, excluding header
      seq     = sequence number for reliable messages (0 for unreliable)

---

2.  MESSAGE TYPES

   The following table defines all supported message types.

   +------+-------------------+----------+------+------------------------------------+
   | Code | Name              | Reliable | Dir  | Purpose                            |
   +------+-------------------+----------+------+------------------------------------+
   | 0x01 | NEW_CLIENT        | yes      | C→S  | Client requests to join            |
   | 0x02 | ACCEPT_CLIENT     | yes      | S→C  | Server accepts, provides clientId  |
   | 0x03 | CLIENT_INPUT      | no       | C→S  | Player inputs (move/shoot)         |
   | 0x04 | SERVER_SNAPSHOT   | no       | S→C  | Full world state update            |
   | 0x05 | ENTITY_SPAWN      | yes      | S→C  | Entity creation                    |
   | 0x06 | ENTITY_DESPAWN    | yes      | S→C  | Entity removal                     |
   | 0x07 | GAME_EVENT        | yes      | S→C  | Score, explosion, hit, pickup      |
   | 0x08 | ACK               | no       | ↔    | Acknowledgment for reliable msgs   |
   | 0x09 | PING              | no       | ↔    | Latency test                       |
   | 0x0A | PONG              | no       | ↔    | Latency response                   |
   | 0x0B | RELIABLE_FRAGMENT | yes      | ↔    | Fragmentation of large messages    |
   | 0x0C | PLAYER_READY      | yes      | C→S  | Player ready/unready in lobby      |
   | 0x0D | GAME_START        | yes      | S→C  | Start of the game                  |
   | 0x0E | GAME_OVER         | yes      | S→C  | End of the game, result            |
   | 0xFF | ADMIN_TEXT        | opt      | ↔    | Debug / Chat text                  |
   +------+-------------------+----------+------+------------------------------------+

---

3.  DETAILED PAYLOADS

3.1  0x01 — NEW_CLIENT
     Direction: C→S — Reliable
     Purpose: Client requests to join the server.

     Payload:
        (empty)

---

3.2  0x02 — ACCEPT_CLIENT
     Direction: S→C — Reliable
     Purpose: Server response, assigning a unique clientId.

     Payload:
        uint32_t clientId

---

3.3  0x03 — CLIENT_INPUT
     Direction: C→S — Unreliable
     Purpose: Player input (sent continuously).

     Payload:
        uint8_t eventType   // 0x00=RELEASE, 0x01=PRESS
        uint8_t keyCode     // 0x00=UP, 0x01=DOWN, 0x02=LEFT,
                            // 0x03=RIGHT, 0x04=SHOOT ...

---

3.4  0x04 — SERVER_SNAPSHOT
     Direction: S→C — Unreliable
     Purpose: Full world state snapshot.

     Payload:
        uint16_t entityCount
        entityCount × (
            uint32_t entityId
            uint8_t  type
            uint16_t x
            uint16_t y
            int16_t  vx
            int16_t  vy
        )

---

3.5  0x05 — ENTITY_SPAWN
     Direction: S→C — Reliable
     Purpose: Entity creation.

     Payload:
        uint32_t entityId
        uint8_t  type
        uint16_t x
        uint16_t y

---

3.6  0x06 — ENTITY_DESPAWN
     Direction: S→C — Reliable
     Purpose: Entity removal.

     Payload:
        uint32_t entityId

---

3.7  0x07 — GAME_EVENT
     Direction: S→C — Reliable
     Purpose: Game event (score, explosion, hit, pickup).

     Payload:
        uint8_t  eventType   // 0x00=EXPLOSION, 0x01=SCORE,
                             // 0x02=HIT, 0x03=PICKUP
        uint32_t entityId
        uint32_t value

---

3.8  0x08 — ACK
     Direction: ↔ — Unreliable
     Purpose: Acknowledgment for reliable messages.

     Payload:
        uint32_t seq

---

3.9  0x09 — PING
     Direction: ↔ — Unreliable
     Purpose: Latency test request.

     Payload:
        uint64_t timestamp

---

3.10  0x0A — PONG
      Direction: ↔ — Unreliable
      Purpose: Latency response.

      Payload:
         uint64_t timestamp

---

3.11  0x0B — RELIABLE_FRAGMENT
      Direction: ↔ — Reliable
      Purpose: Fragmentation of large reliable messages.

      Payload:
         uint32_t parentSeq
         uint16_t fragmentId
         uint16_t fragmentCount
         bytes[]

---

3.12  0x0C — PLAYER_READY
      Direction: C→S — Reliable
      Purpose: Indicates player ready/unready in the lobby.

      Payload:
         uint8_t isReady   // 0x00=NOT_READY, 0x01=READY

---

3.13  0x0D — GAME_START
      Direction: S→C — Reliable
      Purpose: Start of the game.

      Payload:
         uint32_t seed

---

3.14  0x0E — GAME_OVER
      Direction: S→C — Reliable
      Purpose: End of game and result.

      Payload:
         uint8_t result   // 0x00=VICTORY, 0x01=DEFEAT, 0x02=DRAW

---

3.15  0xFF — ADMIN_TEXT
      Direction: ↔ — Optional
      Purpose: Debug or chat text message.

      Payload:
         char text[]

---

4.  RELIABILITY MECHANISM

   Reliable messages (seq ≠ 0) must be acknowledged with an ACK (0x08)
   message. The ACK payload contains the sequence number of the message
   being acknowledged.

   If an ACK is not received within a timeout period, the message MUST
   be retransmitted. Unreliable messages (seq = 0) are not retried.

---

5.  FRAGMENTATION

   Messages exceeding the maximum transmission unit (MTU) size are
   divided into fragments. Each fragment is sent as a RELIABLE_FRAGMENT
   message containing:

      parentSeq     // sequence of parent message
      fragmentId    // current fragment index
      fragmentCount // total fragment count
      bytes[]       // fragment data

   Once all fragments are received, the original message is reassembled
   using parentSeq as the identifier.

---

6.  ADMINISTRATIVE TEXT MESSAGES

   The ADMIN_TEXT (0xFF) message is used for optional debug or chat
   communication. Reliability is implementation-dependent.

---

7.  SECURITY CONSIDERATIONS

   This protocol does not include encryption or authentication.
   Implementations SHOULD encapsulate R-Type traffic in a secure
   transport layer (e.g., DTLS or VPN) when confidentiality is required.

---

8.  AUTHOR’S ADDRESS

   R-Type Network Team
   Email: network@rtype.dev
   Version: 1.0
   Date: October 2025

---

End of R-Type Network Protocol Specification

