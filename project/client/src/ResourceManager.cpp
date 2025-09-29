/*
** EPITECH PROJECT, 2025
** Client
** File description:
** ResourceManager.cpp
*/

#include <iostream>
#include "ResourceManager.hpp"

void ResourceManager::loadTexture(const std::string& id, const std::string& path)
{
    sf::Texture tex;

    if (!tex.loadFromFile(path)) {
        throw std::runtime_error("ResourceManager::loadTexture - failed to load: " + path);
    }
    tex.setSmooth(true);
    _textures.emplace(id, std::move(tex));
    _spritePrototypes.erase(id);
}

bool ResourceManager::hasTexture(const std::string& id) const noexcept
{
    return _textures.find(id) != _textures.end();
}

sf::Texture& ResourceManager::getTexture(const std::string& id)
{
    auto it = _textures.find(id);

    if (it == _textures.end())
        throw std::runtime_error("ResourceManager::getTexture - not found: " + id);
    return it->second;
}

void ResourceManager::loadFont(const std::string& id, const std::string& path)
{
    sf::Font f;

    if (!f.loadFromFile(path)) {
        throw std::runtime_error("ResourceManager::loadFont - failed to load: " + path);
    }
    _fonts.emplace(id, std::move(f));
}

bool ResourceManager::hasFont(const std::string& id) const noexcept
{
    return _fonts.find(id) != _fonts.end();
}

sf::Font& ResourceManager::getFont(const std::string& id)
{
    auto it = _fonts.find(id);

    if (it == _fonts.end())
        throw std::runtime_error("ResourceManager::getFont - not found: " + id);
    return it->second;
}

sf::Sprite ResourceManager::makeSprite(const std::string& textureId)
{
    auto it = _spritePrototypes.find(textureId);
    if (it != _spritePrototypes.end())
        return it->second;

    auto texIt = _textures.find(textureId);
    if (texIt == _textures.end())
        throw std::runtime_error("ResourceManager::makeSprite - texture not loaded: " + textureId);

    sf::Sprite proto;
    proto.setTexture(texIt->second);
    sf::FloatRect b = proto.getLocalBounds();
    proto.setOrigin(b.width * 0.5f, b.height * 0.5f);
    _spritePrototypes.emplace(textureId, proto);
    return _spritePrototypes[textureId];
}

sf::Text ResourceManager::makeText(const std::string& fontId, const std::string& str, unsigned int charSize)
{
    sf::Text t;

    t.setFont(getFont(fontId));
    t.setString(str);
    t.setCharacterSize(charSize);
    return t;
}
