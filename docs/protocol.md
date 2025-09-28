# Protocole réseau R-Type

---

## 1. Enveloppe des messages

Tous les messages commencent par un en-tête commun :

```
- uint8_t  type;
- uint16_t length;
- uint32_t seq;
```

- `type` = identifiant unique (0x01–0xFF)  
- `length` = taille utile (exclut l’en-tête)  
- `seq` = utilisé pour les messages fiables (0 pour les non-fiables)  

---

## 2. Types de messages

| Code | Name               | Reliable | Dir | Purpose                                  |
| ---- | ------------------ | -------- | --- | ---------------------------------------- |
| 0x01 | NEW_CLIENT         | yes      | C→S | Client demande à rejoindre               |
| 0x02 | ACCEPT_CLIENT      | yes      | S→C | Serveur accepte, donne un `clientId`     |
| 0x03 | CLIENT_INPUT       | no       | C→S | Entrées du joueur (move/shoot)           |
| 0x04 | SERVER_SNAPSHOT    | no       | S→C | État complet du monde (entités visibles) |
| 0x05 | ENTITY_SPAWN       | yes      | S→C | Création d’une entité                    |
| 0x06 | ENTITY_DESPAWN     | yes      | S→C | Suppression d’une entité                 |
| 0x07 | GAME_EVENT         | yes      | S→C | Score, explosion, hit, pickup            |
| 0x08 | ACK                | no       | ↔   | Ack pour messages fiables                |
| 0x09 | PING               | no       | ↔   | Test de latence                          |
| 0x0A | PONG               | no       | ↔   | Réponse latence                          |
| 0x0B | RELIABLE_FRAGMENT  | yes      | ↔   | Fragmentation de gros messages           |
| 0x0C | PLAYER_READY       | yes      | C→S | Joueur prêt / non-prêt dans le lobby     |
| 0x0D | GAME_START         | yes      | S→C | Début de la partie                       |
| 0x0E | GAME_OVER          | yes      | S→C | Fin de partie, résultat                  |
| 0xFF | ADMIN_TEXT         | opt      | ↔   | Debug / Chat texte                       |

---

## 3. Payloads détaillés

### 0x01 — NEW_CLIENT  
**C→S — fiable**  
> Demande de connexion.

Payload :  
```
(empty)
```

---

### 0x02 — ACCEPT_CLIENT  
**S→C — fiable**  
> Réponse à une connexion.

Payload :  
```
uint32_t clientId
```

---

### 0x03 — CLIENT_INPUT  
**C→S — non-fiable**  
> Entrées du joueur (en continu).

Payload :  
```
uint8_t eventType   // 0x00=RELEASE, 0x01=PRESS
uint8_t keyCode     // enum: 0x00=UP, 0x01=DOWN, 0x02=LEFT, 0x03=RIGHT, 0x04=SHOOT ...
```

---

### 0x04 — SERVER_SNAPSHOT  
**S→C — non-fiable**  
> État global du monde.

Payload :  
```
uint16_t entityCount
entityCount × (
    uint32_t entityId
    uint8_t type
    uint16_t x
    uint16_t y
    int16_t vx
    int16_t vy
)
```

---

### 0x05 — ENTITY_SPAWN  
**S→C — fiable**  
> Création d’une entité.

Payload :  
```
uint32_t entityId
uint8_t  type
uint16_t x
uint16_t y
```

---

### 0x06 — ENTITY_DESPAWN  
**S→C — fiable**  
> Suppression d’une entité.

Payload :  
```
uint32_t entityId
```

---

### 0x07 — GAME_EVENT  
**S→C — fiable**  
> Événement particulier.

Payload :  
```
uint8_t eventType   // 0x00=EXPLOSION, 0x01=SCORE, 0x02=HIT, 0x03=PICKUP
uint32_t entityId
uint32_t value
```

---

### 0x08 — ACK  
**↔ — non-fiable**  
> Accusé de réception d’un message fiable.

Payload :  
```
uint32_t seq
```

---

### 0x09 — PING  
**↔ — non-fiable**  
> Test de latence.

Payload :  
```
uint64_t timestamp
```

---

### 0x0A — PONG  
**↔ — non-fiable**  
> Réponse au ping.

Payload :  
```
uint64_t timestamp
```

---

### 0x0B — RELIABLE_FRAGMENT  
**↔ — fiable**  
> Découpe d’un gros message.

Payload :  
```
uint32_t parentSeq
uint16_t fragmentId
uint16_t fragmentCount
bytes[]
```

---

### 0x0C — PLAYER_READY  
**C→S — fiable**  
> Signale que le joueur est prêt ou non dans le lobby.

Payload :  
```
uint8_t isReady   // 0x00=NOT_READY, 0x01=READY
```

---

### 0x0D — GAME_START  
**S→C — fiable**  
> Début de partie.

Payload :  
```
uint32_t seed
```

---

### 0x0E — GAME_OVER  
**S→C — fiable**  
> Fin de partie.

Payload :  
```
uint8_t result   // 0x00=VICTORY, 0x01=DEFEAT, 0x02=DRAW
```

---

### 0xFF — ADMIN_TEXT  
**↔ — optionnel**  
> Messages texte pour debug/chat.

Payload :  
```
char text[]
```

---
