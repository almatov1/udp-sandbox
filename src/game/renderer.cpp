#include "game/renderer.h"
#include "game_config.h"

#include <cstdio>

void DrawGame(const Player& player, float frameTimeSeconds)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Fixed world boundaries: never use monitor dimensions for game logic.
    DrawRectangleLines(
        0, 0,
        GameConfig::WindowWidth - 1,
        GameConfig::WindowHeight - 1,
        RED
    );

    DrawRectangle(
        static_cast<int>(player.position.x + 0.5f),
        static_cast<int>(player.position.y + 0.5f),
        static_cast<int>(player.size),
        static_cast<int>(player.size),
        BLUE
    );

    constexpr int margin = GameConfig::DebugMargin;
    constexpr int fontSize = GameConfig::DebugFontSize;
    constexpr int lineHeight = GameConfig::DebugLineHeight;
    constexpr int panelWidth = 330;
    constexpr int panelHeight = 8 * lineHeight + 16;

    DrawRectangle(margin - 8, margin - 8, panelWidth, panelHeight,
                  Fade(LIGHTGRAY, 0.88f));

    char text[160];

    std::snprintf(text, sizeof(text), "FPS: %d", GetFPS());
    DrawText(text, margin, margin, fontSize, DARKGRAY);

    std::snprintf(text, sizeof(text), "Frame time: %.3f ms", frameTimeSeconds * 1000.0f);
    DrawText(text, margin, margin + lineHeight, fontSize, DARKGRAY);

    std::snprintf(text, sizeof(text), "Window/world: %d x %d",
                  GameConfig::WindowWidth, GameConfig::WindowHeight);
    DrawText(text, margin, margin + 2 * lineHeight, fontSize, DARKGRAY);

    std::snprintf(text, sizeof(text), "Player X: %.2f", player.position.x);
    DrawText(text, margin, margin + 3 * lineHeight, fontSize, DARKGRAY);

    std::snprintf(text, sizeof(text), "Player Y: %.2f", player.position.y);
    DrawText(text, margin, margin + 4 * lineHeight, fontSize, DARKGRAY);

    std::snprintf(text, sizeof(text), "Player size: %.0f x %.0f", player.size, player.size);
    DrawText(text, margin, margin + 5 * lineHeight, fontSize, DARKGRAY);

    std::snprintf(text, sizeof(text), "Speed: %.0f px/s", player.speed);
    DrawText(text, margin, margin + 6 * lineHeight, fontSize, DARKGRAY);

    std::snprintf(text, sizeof(text), "Keys: WASD / arrows");
    DrawText(text, margin, margin + 7 * lineHeight, fontSize, DARKGRAY);

    DrawText("ESC: Exit", margin, GameConfig::WindowHeight - margin - fontSize,
             fontSize, DARKGRAY);

    EndDrawing();
}
