#pragma once

#include "raylib.h"

struct Player {
    Vector2 position{};
    float size = 40.0f;
    float speed = 600.0f;
};

void InitializePlayer(Player& player);
void UpdatePlayer(Player& player, float deltaTime);
void KeepPlayerInsideWorld(Player& player);
