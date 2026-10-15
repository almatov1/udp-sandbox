#pragma once

#include "raylib.h"

#include <cstdint>
#include <string>
#include <vector>

struct RemotePlayer {
    std::uint32_t id = 0;
    Vector2 position{};
    Color color = WHITE;
};

struct Player {
    Vector2 position{};
    Vector2 serverPosition{};
    float size = 40.0f;
    float speed = 600.0f;

    std::uint32_t id = 0;
    Color color = BLUE;
    bool initialized = false;
};

void InitializePlayer(Player& player);
void UpdatePredictedPlayer(Player& player, std::uint8_t buttons,
                           float deltaTime);
void ReconcilePlayer(Player& player, Vector2 authoritativePosition,
                     float deltaTime);
Color ParseHexColor(const std::string& hex);
