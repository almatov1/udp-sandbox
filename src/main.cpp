#include "raylib.h"

#include "game/player.h"
#include "game/renderer.h"
#include "game_config.h"
#include "net/udp_client.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

namespace {

std::uint8_t ReadButtons()
{
    std::uint8_t buttons = 0;

    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) buttons |= 1u << 0;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) buttons |= 1u << 1;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) buttons |= 1u << 2;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) buttons |= 1u << 3;

    return buttons;
}

} // namespace

int main()
{
    InitWindow(GameConfig::WindowWidth,
               GameConfig::WindowHeight,
               GameConfig::WindowTitle);

    SetTargetFPS(0);

    Player player;
    InitializePlayer(player);

    UdpClient client;
    const bool connected = client.Connect("127.0.0.1", 9000);

    double lastInputSend = 0.0;
    double lastSnapshotTime = 0.0;

    while (!WindowShouldClose()) {
        const float rawDeltaTime = GetFrameTime();
        const float dt = std::min(rawDeltaTime, GameConfig::MaxDeltaTime);

        client.Poll();

        const std::uint8_t buttons = ReadButtons();
        const double now = GetTime();

        if (connected && now - lastInputSend >= 1.0 / 30.0) {
            client.SendInput(buttons);
            lastInputSend = now;
        }

        if (client.HasSnapshot()) {
            const auto& players = client.GetPlayers();
            const auto selfID = client.GetSelfID();

            player.id = selfID;

            for (const auto& p : players) {
                if (p.id == selfID) {
                    player.color = p.color;

                    if (!player.initialized) {
                        player.position = p.position;
                        player.serverPosition = p.position;
                        player.initialized = true;
                    } else {
                        // Smoothly reconcile local prediction with server state.
                        ReconcilePlayer(player, p.position, dt);
                    }

                    lastSnapshotTime = now;
                    break;
                }
            }
        }

        if (connected && player.initialized) {
            // Immediate local response; server remains authoritative.
            UpdatePredictedPlayer(player, buttons, dt);
        }

        // Basic smoothing for other players. A later version can interpolate
        // between timestamped snapshots for better latency compensation.
        static std::vector<RemotePlayer> smoothedPlayers;
        const auto& authoritativePlayers = client.GetPlayers();

        for (const auto& target : authoritativePlayers) {
            auto it = std::find_if(smoothedPlayers.begin(), smoothedPlayers.end(),
                [&target](const RemotePlayer& p) { return p.id == target.id; });

            if (it == smoothedPlayers.end()) {
                smoothedPlayers.push_back(target);
            } else {
                const float alpha = 1.0f - std::exp(-18.0f * dt);
                it->position.x += (target.position.x - it->position.x) * alpha;
                it->position.y += (target.position.y - it->position.y) * alpha;
                it->color = target.color;
            }
        }

        smoothedPlayers.erase(
            std::remove_if(smoothedPlayers.begin(), smoothedPlayers.end(),
                [&authoritativePlayers](const RemotePlayer& p) {
                    return std::none_of(authoritativePlayers.begin(),
                        authoritativePlayers.end(),
                        [&p](const RemotePlayer& current) {
                            return current.id == p.id;
                        });
                }),
            smoothedPlayers.end());

        const bool snapshotFresh =
            client.HasSnapshot() && now - lastSnapshotTime < 2.0;

        DrawGame(player, smoothedPlayers, rawDeltaTime,
                 connected, snapshotFresh, client.GetError());
    }

    client.Disconnect();
    CloseWindow();
    return 0;
}
