#include "raylib.h"

#include "game/player.h"
#include "game/renderer.h"
#include "game_config.h"
#include "net/udp_client.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <deque>
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

struct SnapshotFrame {
    double time = 0.0;
    std::vector<RemotePlayer> players;
};

constexpr double InputRate = 64.0;
constexpr double InterpolationDelay = 2.0 / 64.0;
constexpr double SnapshotHistoryDuration = 1.0;

const RemotePlayer* FindPlayer(
    const std::vector<RemotePlayer>& players,
    std::uint32_t id)
{
    const auto it = std::find_if(
        players.begin(), players.end(),
        [id](const RemotePlayer& p) {
            return p.id == id;
        });

    return it == players.end() ? nullptr : &*it;
}

std::vector<RemotePlayer> InterpolateSnapshots(
    const SnapshotFrame& older,
    const SnapshotFrame& newer,
    double renderTime)
{
    const double duration = newer.time - older.time;

    const float alpha = duration > 0.0
        ? static_cast<float>(std::clamp(
            (renderTime - older.time) / duration, 0.0, 1.0))
        : 1.0f;

    std::vector<RemotePlayer> result;
    result.reserve(older.players.size());

    for (const auto& oldPlayer : older.players) {
        const auto* newPlayer = FindPlayer(newer.players, oldPlayer.id);

        // A player missing from the newer snapshot may have disconnected.
        if (!newPlayer) {
            result.push_back(oldPlayer);
            continue;
        }

        RemotePlayer p = oldPlayer;

        p.position.x = oldPlayer.position.x
            + (newPlayer->position.x - oldPlayer.position.x) * alpha;

        p.position.y = oldPlayer.position.y
            + (newPlayer->position.y - oldPlayer.position.y) * alpha;

        p.color = newPlayer->color;

        result.push_back(p);
    }

    // Include players who appeared only in the newer snapshot.
    for (const auto& newPlayer : newer.players) {
        if (!FindPlayer(older.players, newPlayer.id)) {
            result.push_back(newPlayer);
        }
    }

    return result;
}

std::vector<RemotePlayer> GetInterpolatedPlayers(
    const std::deque<SnapshotFrame>& history,
    double renderTime)
{
    if (history.empty()) {
        return {};
    }

    if (history.size() == 1 || renderTime <= history.front().time) {
        return history.front().players;
    }

    for (std::size_t i = 1; i < history.size(); ++i) {
        if (renderTime <= history[i].time) {
            return InterpolateSnapshots(
                history[i - 1], history[i], renderTime);
        }
    }

    // No future snapshot is available yet.
    return history.back().players;
}

} // namespace

int main()
{
    InitWindow(
        GameConfig::WindowWidth,
        GameConfig::WindowHeight,
        GameConfig::WindowTitle);

    SetTargetFPS(0);

    Player player;
    InitializePlayer(player);

    UdpClient client;
    const bool connected = client.Connect("127.0.0.1", 9000);

    double lastInputSend = 0.0;
    double lastSnapshotTime = 0.0;

    std::uint64_t lastProcessedSequence = 0;

    std::deque<SnapshotFrame> snapshotHistory;
    std::vector<RemotePlayer> renderedPlayers;

    while (!WindowShouldClose()) {
        const float rawDeltaTime = GetFrameTime();
        const float dt = std::min(
            rawDeltaTime, GameConfig::MaxDeltaTime);

        client.Poll();

        const std::uint8_t buttons = ReadButtons();
        const double now = GetTime();

        if (connected && now - lastInputSend >= 1.0 / InputRate) {
            client.SendInput(buttons);
            lastInputSend = now;
        }

        // Process each newly received snapshot only once.
        const auto sequence = client.GetSnapshotSequence();

        if (client.HasSnapshot() &&
            sequence != lastProcessedSequence) {
            lastProcessedSequence = sequence;

            const auto& players = client.GetPlayers();
            const auto selfID = client.GetSelfID();

            player.id = selfID;

            for (const auto& p : players) {
                if (p.id != selfID) {
                    continue;
                }

                player.color = p.color;

                if (!player.initialized) {
                    player.position = p.position;
                    player.serverPosition = p.position;
                    player.initialized = true;
                } else {
                    ReconcilePlayer(player, p.position, dt);
                }

                break;
            }

            const double snapshotTime =
                client.GetSnapshotReceiveTime();

            // Keep snapshots ordered by receive time.
            if (snapshotHistory.empty() ||
                snapshotTime > snapshotHistory.back().time) {
                snapshotHistory.push_back(
                    SnapshotFrame{snapshotTime, players});
            }

            lastSnapshotTime = snapshotTime;
        }

        // Local player remains responsive through prediction.
        if (connected && player.initialized) {
            UpdatePredictedPlayer(player, buttons, dt);
        }

        // Render remote players slightly in the past so that both
        // endpoints of each interpolation interval are available.
        const double renderTime = now - InterpolationDelay;

        while (snapshotHistory.size() > 2 &&
               snapshotHistory[1].time < renderTime) {
            snapshotHistory.pop_front();
        }

        renderedPlayers = GetInterpolatedPlayers(
            snapshotHistory, renderTime);

        const bool snapshotFresh =
            client.HasSnapshot() &&
            now - lastSnapshotTime < 2.0;

        DrawGame(
            player,
            renderedPlayers,
            rawDeltaTime,
            connected,
            snapshotFresh,
            client.GetError());
    }

    client.Disconnect();
    CloseWindow();

    return 0;
}
