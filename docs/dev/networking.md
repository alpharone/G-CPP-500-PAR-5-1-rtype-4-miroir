# Networking

The network system uses reliable UDP via Asio for client-server communication. This guide covers using and extending the networking capabilities.

## Overview

- **Protocols**: Reliable UDP with retransmission, sequencing, and fragmentation.
- **Client**: `ClientNetworkSystem` handles connection, input send, snapshot receive.
- **Server**: `ServerNetworkSystem` manages rooms, entities sync, input apply.

## Key Classes

- `AsioNetworkTransport`: Low-level UDP socket.
- `ReliableLayerAdapter`: Adds reliability on top of UDP.
- `DefaultMessageSerializer`: Serializes packets with messages.
- `RoomManager`: Handles client joining, room creation, position updates.
- `ServerNetworkSystem`: Synchronizes ECS states via snapshots.

## Sending Messages

From `ServerNetworkSystem`:

```cpp
Network::Packet pkt;
pkt.header.type = Network::MESSAGE_TYPE;
pkt.payload = serializeData();
// Send via adapter
_adapter->sendReliable(clientEndpoint, pkt);
```

## Adding New Message Types

1. Add to `MessageType` enum in `Packets.hpp`.

2. Handle in `ServerNetworkSystem::onAppPacket` or `ClientNetworkSystem`.

3. Add serialization to `DefaultMessageSerializer`.

## Snapshots

Server collects positions from Registry and sends position data via `SERVER_SNAPSHOT`.

Client applies to local Registry for rendering.

## Notes

- Use `.so` in configs for Linux.
- Timestamps are in snapshots for interpolation.
- Ensure deterministic server logic for consistency.
