#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <json/json.h>

int main() {
    // Create UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return -1;
    }

    sockaddr_in receiverAddr{}, senderAddr{};
    receiverAddr.sin_family = AF_INET;
    receiverAddr.sin_port = htons(8080); // Listening port
    receiverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind(sock, (struct sockaddr*)&receiverAddr, sizeof(receiverAddr)) < 0) {
        perror("Bind failed");
        return -1;
    }

    char buffer[1024];
    socklen_t senderAddrLen = sizeof(senderAddr);

    std::cout << "Waiting for data..." << std::endl;
    int recvBytes = recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
                             (struct sockaddr*)&senderAddr, &senderAddrLen);
    if (recvBytes < 0) {
        perror("Receive failed");
        return -1;
    }

    buffer[recvBytes] = '\0';
    std::string jsonString(buffer);

    // Parse JSON data
    Json::CharReaderBuilder readerBuilder;
    Json::Value jsonData;
    std::string errors;
    std::istringstream jsonStream(jsonString);
    if (!Json::parseFromStream(readerBuilder, jsonStream, &jsonData, &errors)) {
        std::cerr << "Failed to parse JSON: " << errors << std::endl;
        return -1;
    }

    std::cout << "Received JSON data: " << jsonData << std::endl;
    close(sock);
    return 0;
}
