// /*
// ** EPITECH PROJECT, 2025
// ** Client
// ** File description:
// ** ResourceManager.hpp
// */

// #pragma once

// #include <string>
// #include <stdexcept>
// #include <unordered_map>
// #include <SFML/Graphics.hpp>
// #include <SFML/Audio.hpp>

// class ResourceManager {
//     public:
//         ResourceManager() = default;
//         ~ResourceManager() = default;

//         void loadTexture(const std::string& id, const std::string& path);
//         bool hasTexture(const std::string& id) const noexcept;
//         sf::Texture& getTexture(const std::string& id);

//         void loadFont(const std::string& id, const std::string& path);
//         bool hasFont(const std::string& id) const noexcept;
//         sf::Font& getFont(const std::string& id);

//         sf::Sprite makeSprite(const std::string& textureId);
//         sf::Text makeText(const std::string& fontId, const std::string& str, unsigned int charSize = 14u);

//     private:
//         std::unordered_map<std::string, sf::Texture> _textures;
//         std::unordered_map<std::string, sf::Font> _fonts;
//         std::unordered_map<std::string, sf::Sprite> _spritePrototypes;
// };

