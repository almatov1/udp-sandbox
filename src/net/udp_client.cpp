#include "net/udp_client.h"

#include "raylib.h"

#include <arpa/inet.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <string>

namespace {

bool ReadNumber(const std::string& s, const char* key, double& value)
{
    const std::string needle = std::string("\"") + key + "\"";
    auto pos = s.find(needle);
    if (pos == std::string::npos) return false;

    pos = s.find(':', pos + needle.size());
    if (pos == std::string::npos) return false;

    char* end = nullptr;
    value = std::strtod(s.c_str() + pos + 1, &end);
    return end != s.c_str() + pos + 1;
}

bool ReadString(const std::string& s, const char* key, std::string& value)
{
    const std::string needle = std::string("\"") + key + "\"";
    auto pos = s.find(needle);
    if (pos == std::string::npos) return false;

    pos = s.find(':', pos + needle.size());
    if (pos == std::string::npos) return false;

    pos = s.find('"', pos + 1);
    if (pos == std::string::npos) return false;

    const auto end = s.find('"', pos + 1);
    if (end == std::string::npos) return false;

    value = s.substr(pos + 1, end - pos - 1);
    return true;
}

bool ParseSnapshot(const std::string& json,
                   std::uint32_t& selfID,
                   std::vector<RemotePlayer>& players)
{
    double self = 0.0;
    if (!ReadNumber(json, "selfId", self)) return false;

    selfID = static_cast<std::uint32_t>(self);
    players.clear();

    const auto key = json.find("\"players\"");
    if (key == std::string::npos) return false;

    const auto arrayStart = json.find('[', key);
    const auto arrayEnd = json.find(']', arrayStart);

    if (arrayStart == std::string::npos ||
        arrayEnd == std::string::npos) {
        return false;
    }

    std::size_t pos = arrayStart + 1;

    while (pos < arrayEnd) {
        const auto begin = json.find('{', pos);
        if (begin == std::string::npos || begin >= arrayEnd) break;

        const auto end = json.find('}', begin);
        if (end == std::string::npos || end > arrayEnd) return false;

        const std::string object = json.substr(begin, end - begin + 1);

        double id = 0.0, x = 0.0, y = 0.0;
        std::string color;

        if (ReadNumber(object, "id", id) &&
            ReadNumber(object, "x", x) &&
            ReadNumber(object, "y", y) &&
            ReadString(object, "color", color)) {
            players.push_back(RemotePlayer{
                static_cast<std::uint32_t>(id),
                Vector2{static_cast<float>(x), static_cast<float>(y)},
                ParseHexColor(color)
            });
        }

        pos = end + 1;
    }

    return true;
}

} // namespace

UdpClient::UdpClient() = default;

UdpClient::~UdpClient()
{
    Disconnect();
}

bool UdpClient::Connect(const char* host, unsigned short port)
{
    Disconnect();

    socketFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketFd < 0) {
        error = "socket() failed";
        return false;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host, &addr.sin_addr) != 1) {
        error = "Invalid server IP";
        Disconnect();
        return false;
    }

    const int flags = fcntl(socketFd, F_GETFL, 0);
    if (flags < 0 || fcntl(socketFd, F_SETFL, flags | O_NONBLOCK) < 0) {
        error = "Failed to set non-blocking socket";
        Disconnect();
        return false;
    }

    if (connect(socketFd, reinterpret_cast<sockaddr*>(&addr),
                sizeof(addr)) < 0) {
        error = "UDP connect failed";
        Disconnect();
        return false;
    }

    connected = true;
    error.clear();
    return true;
}

void UdpClient::Disconnect()
{
    if (socketFd >= 0) {
        close(socketFd);
    }

    socketFd = -1;
    connected = false;
    hasSnapshot = false;
    selfID = 0;
    players.clear();
}

bool UdpClient::IsConnected() const
{
    return connected;
}

bool UdpClient::HasSnapshot() const
{
    return hasSnapshot;
}

std::uint32_t UdpClient::GetSelfID() const
{
    return selfID;
}

const std::vector<RemotePlayer>& UdpClient::GetPlayers() const
{
    return players;
}

const std::string& UdpClient::GetError() const
{
    return error;
}

void UdpClient::SendInput(std::uint8_t buttons)
{
    if (!connected) return;

    char packet[96];
    const int n = std::snprintf(
        packet, sizeof(packet),
        "{\"type\":\"input\",\"buttons\":%u}",
        static_cast<unsigned>(buttons));

    if (n > 0 && n < static_cast<int>(sizeof(packet))) {
        const auto sent = send(socketFd, packet, static_cast<std::size_t>(n), 0);
        if (sent < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
            error = "UDP send failed";
        }
    }
}

void UdpClient::Poll()
{
    if (!connected) return;

    char buffer[65536];

    for (;;) {
        const auto n = recv(socketFd, buffer, sizeof(buffer) - 1, 0);

        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) break;
            error = "UDP receive failed";
            break;
        }

        if (n == 0) break;

        buffer[n] = '\0';

        std::uint32_t newSelfID = 0;
        std::vector<RemotePlayer> newPlayers;

        if (ParseSnapshot(std::string(buffer, static_cast<std::size_t>(n)),
                          newSelfID, newPlayers)) {
            selfID = newSelfID;
            players = std::move(newPlayers);
            hasSnapshot = true;
            error.clear();
        }
    }
}
