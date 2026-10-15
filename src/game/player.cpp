#include "game/player.h"
#include "game_config.h"

#include <algorithm>
#include <cmath>

void InitializePlayer(Player& player)
{
    player.size = GameConfig::PlayerSize;
    player.speed = GameConfig::PlayerSpeed;
    player.position = {
        (GameConfig::WindowWidth - player.size) / 2.0f,
        (GameConfig::WindowHeight - player.size) / 2.0f
    };
}

void UpdatePredictedPlayer(Player& player, std::uint8_t buttons,
                           float deltaTime)
{
    float dx = 0.0f;
    float dy = 0.0f;

    if (buttons & (1u << 2)) dx -= 1.0f; // A / Left
    if (buttons & (1u << 3)) dx += 1.0f; // D / Right
    if (buttons & (1u << 0)) dy -= 1.0f; // W / Up
    if (buttons & (1u << 1)) dy += 1.0f; // S / Down

    const float length = std::sqrt(dx * dx + dy * dy);

    if (length > 0.0f) {
        dx /= length;
        dy /= length;

        player.position.x += dx * player.speed * deltaTime;
        player.position.y += dy * player.speed * deltaTime;
    }

    player.position.x = std::clamp(
        player.position.x, 0.0f,
        static_cast<float>(GameConfig::WindowWidth) - player.size);

    player.position.y = std::clamp(
        player.position.y, 0.0f,
        static_cast<float>(GameConfig::WindowHeight) - player.size);
}

void ReconcilePlayer(Player& player, Vector2 authoritativePosition,
                     float deltaTime)
{
    player.serverPosition = authoritativePosition;

    const float dx = authoritativePosition.x - player.position.x;
    const float dy = authoritativePosition.y - player.position.y;
    const float error = std::sqrt(dx * dx + dy * dy);

    // A large difference usually means that the local prediction diverged.
    if (error > 120.0f) {
        player.position = authoritativePosition;
        return;
    }

    const float alpha = 1.0f - std::exp(-14.0f * deltaTime);
    player.position.x += dx * alpha;
    player.position.y += dy * alpha;
}

Color ParseHexColor(const std::string& hex)
{
    if (hex.size() != 7 || hex[0] != '#') {
        return WHITE;
    }

    try {
        const auto r = static_cast<unsigned char>(
            std::stoul(hex.substr(1, 2), nullptr, 16));
        const auto g = static_cast<unsigned char>(
            std::stoul(hex.substr(3, 2), nullptr, 16));
        const auto b = static_cast<unsigned char>(
            std::stoul(hex.substr(5, 2), nullptr, 16));

        return Color{r, g, b, 255};
    } catch (...) {
        return WHITE;
    }
}
