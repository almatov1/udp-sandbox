#include <cmath>
#include "game/player.h"
#include "game_config.h"

void InitializePlayer(Player& player)
{
    player.size = GameConfig::PlayerSize;
    player.speed = GameConfig::PlayerSpeed;
    player.position = {
        (GameConfig::WindowWidth - player.size) / 2.0f,
        (GameConfig::WindowHeight - player.size) / 2.0f
    };
}

void UpdatePlayer(Player& player, float deltaTime)
{
    // Normalize diagonal movement so it isn't faster than horizontal/vertical movement.
    Vector2 direction = {0.0f, 0.0f};

    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    direction.y -= 1.0f;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  direction.y += 1.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  direction.x -= 1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) direction.x += 1.0f;

    if (direction.x != 0.0f || direction.y != 0.0f) {
        float length = sqrtf(direction.x * direction.x +
                     direction.y * direction.y);

        if (length > 0.0f)
        {
            direction.x /= length;
            direction.y /= length;
        }
        player.position.x += direction.x * player.speed * deltaTime;
        player.position.y += direction.y * player.speed * deltaTime;
    }

    KeepPlayerInsideWorld(player);
}

void KeepPlayerInsideWorld(Player& player)
{
    const float maxX = static_cast<float>(GameConfig::WindowWidth) - player.size;
    const float maxY = static_cast<float>(GameConfig::WindowHeight) - player.size;

    if (player.position.x < 0.0f) player.position.x = 0.0f;
    if (player.position.y < 0.0f) player.position.y = 0.0f;
    if (player.position.x > maxX) player.position.x = maxX;
    if (player.position.y > maxY) player.position.y = maxY;
}
