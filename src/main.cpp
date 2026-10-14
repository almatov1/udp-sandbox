#include "raylib.h"

#include "game/player.h"
#include "game/renderer.h"
#include "game_config.h"

#include <algorithm>

int main()
{
    // No RESIZABLE, BORDERLESS, FULLSCREEN, or VSYNC flags.
    // The window is created at a fixed size and no in-game fullscreen toggle exists.
    InitWindow(GameConfig::WindowWidth, GameConfig::WindowHeight, GameConfig::WindowTitle);

    // 0 disables raylib's target-FPS limiter. VSync is not enabled above.
    SetTargetFPS(0);

    Player player;
    InitializePlayer(player);

    while (!WindowShouldClose()) {
        // Raw elapsed time is used for the debug display.
        const float rawDeltaTime = GetFrameTime();

        // Clamp only movement delta, not the displayed frame time.
        const float movementDeltaTime =
            std::min(rawDeltaTime, GameConfig::MaxDeltaTime);

        UpdatePlayer(player, movementDeltaTime);
        DrawGame(player, rawDeltaTime);
    }

    CloseWindow();
    return 0;
}
