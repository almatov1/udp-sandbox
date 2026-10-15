#pragma once

#include "game/player.h"

#include <cstdint>
#include <string>
#include <vector>

class UdpClient {
public:
    UdpClient();
    ~UdpClient();

    bool Connect(const char* host, unsigned short port);
    void Disconnect();

    bool IsConnected() const;
    void SendInput(std::uint8_t buttons);
    void Poll();

    bool HasSnapshot() const;
    std::uint32_t GetSelfID() const;
    const std::vector<RemotePlayer>& GetPlayers() const;
    const std::string& GetError() const;

    std::uint64_t GetSnapshotSequence() const;
    double GetSnapshotReceiveTime() const;

private:
    int socketFd = -1;
    bool connected = false;
    bool hasSnapshot = false;
    std::uint32_t selfID = 0;

    std::vector<RemotePlayer> players;
    std::string error;

    std::uint64_t snapshotSequence = 0;
    double snapshotReceiveTime = 0.0;
};
