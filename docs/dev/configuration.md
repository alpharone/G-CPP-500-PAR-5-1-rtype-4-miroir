# Configuration

The engine uses JSON files for configuration, allowing runtime settings without code changes.

## Files

- `config/client.json`: Client-side configs (connection, graphics, systems to load).
- `config/server.json`: Server-side configs (network, systems, levels, enemies).

## Structure

### Client JSON

```json
{
  "systems": [
    {
      "name": "system_name",
      "lib": "./plugins/systems/libSystem.dylib",
      "factory": "createFactoryFunction",
      "params": ["param1", "param2"]
    }
  ],
  "globals": { "key": "value" },
  "player": { /* settings */ },
  "network": { "server_ip": "127.0.0.1", "server_port": 4242 },
  "display": { "window_width": 1200, "window_height": 1000, "window_title": "R-Type" }
}
```

### Server JSON

Includes systems, rooms, levels with enemies, patterns.

## Adding Config Values

1. Add to relevant JSON file.

2. Access in code via parameters passed to systems.

3. For example, systems receive `std::any params` from config, cast to use.

## Notes

- Systems are loaded dynamically based on JSON.
- Params allow passing strings or numbers.
- Use nlohmann::json in code for complex configs.
- Example: AnimationSystem params for sprite sheets.
