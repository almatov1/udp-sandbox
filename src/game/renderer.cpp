#include "game/renderer.h"
#include "game_config.h"

#include <cstdio>

void DrawGame(const Player& player,
              const std::vector<RemotePlayer>& players,
              float frameTimeSeconds,
              bool connected,
              bool hasSnapshot,
              const std::string& error)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawRectangleLines(0, 0,
        GameConfig::WindowWidth - 1,
        GameConfig::WindowHeight - 1, RED);

    for (const auto& p : players) {
        if (p.id == player.id) continue;

        DrawRectangle(
            static_cast<int>(p.position.x + 0.5f),
            static_cast<int>(p.position.y + 0.5f),
            static_cast<int>(player.size),
            static_cast<int>(player.size),
            p.color);

        char label[32];
        std::snprintf(label, sizeof(label), "#%u", p.id);
        DrawText(label, static_cast<int>(p.position.x),
                 static_cast<int>(p.position.y) - 18, 14, DARKGRAY);
    }

    DrawRectangle(
        static_cast<int>(player.position.x + 0.5f),
        static_cast<int>(player.position.y + 0.5f),
        static_cast<int>(player.size),
        static_cast<int>(player.size),
        player.color);

    constexpr int margin = GameConfig::DebugMargin;
    constexpr int lineHeight = GameConfig::DebugLineHeight;
    constexpr int fontSize = GameConfig::DebugFontSize;

    DrawRectangle(margin - 8, margin - 8, 390, 240,
                  Fade(LIGHTGRAY, 0.90f));

    char text[160];

    std::snprintf(text, sizeof(text), "FPS: %d", GetFPS());
    DrawText(text, margin, margin, fontSize, DARKGRAY);

    std::snprintf(text, sizeof(text), "Frame time: %.3f ms",
                  frameTimeSeconds * 1000.0f);
    DrawText(text, margin, margin + lineHeight, fontSize, DARKGRAY);

    DrawText(connected ? (hasSnapshot ? "Connected" : "Waiting for server")
                       : "Disconnected",
             margin, margin + 2 * lineHeight, fontSize,
             connected && hasSnapshot ? DARKGREEN : RED);

    std::snprintf(text, sizeof(text), "Player ID: %u", player.id);
    DrawText(text, margin, margin + 3 * lineHeight, fontSize, DARKGRAY);

    std::snprintf(text, sizeof(text), "X: %.2f  Y: %.2f",
                  player.position.x, player.position.y);
    DrawText(text, margin, margin + 4 * lineHeight, fontSize, DARKGRAY);

    std::snprintf(text, sizeof(text), "Players: %d",
                  static_cast<int>(players.size()));
    DrawText(text, margin, margin + 5 * lineHeight, fontSize, DARKGRAY);

    DrawText("WASD / Arrows | ESC: Exit",
             margin, margin + 6 * lineHeight, fontSize, DARKGRAY);

    if (!error.empty()) {
        DrawText(error.c_str(), margin, margin + 7 * lineHeight,
                 14, RED);
    }

    EndDrawing();
}
