#pragma once

#include "game/player.h"
#include <string>
#include <vector>

void DrawGame(const Player& player,
              const std::vector<RemotePlayer>& players,
              float frameTimeSeconds,
              bool connected,
              bool hasSnapshot,
              const std::string& error);
