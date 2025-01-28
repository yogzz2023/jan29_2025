g++ -c COMMAND_TARGET.cpp -o COMMAND_TARGET.o


ar rcs libcommand_target.a COMMAND_TARGET.o

//udp_send.cpp
#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <json/json.h> // Requires JSON for Modern C++

#include "COMMAND_TARGET.h"

int main() 
{
    // Initialize COMMAND_TARGET object
    COMMAND_TARGET target;
    target.msg.ccuTrackId = 1;
    target.msg.radTrackId = 101;
    target.msg.trackConfidence = 0.9;
    target.msg.speed = 50.5;
    target.msg.direction = 180;

    // Create JSON payload
    Json::Value jsonData;
    jsonData["ccuTrackId"] = target.msg.ccuTrackId;
    jsonData["radTrackId"] = target.msg.radTrackId;
    jsonData["trackConfidence"] = target.msg.trackConfidence;
    jsonData["speed"] = target.msg.speed;
    jsonData["direction"] = target.msg.direction;
    std::string jsonString = Json::writeString(Json::StreamWriterBuilder(), jsonData);

    // Create UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return -1;
    }

    sockaddr_in receiverAddr{};
    receiverAddr.sin_family = AF_INET;
    receiverAddr.sin_port = htons(8080); // Receiver port
    receiverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Receiver address

    // Send JSON data
    int sentBytes = sendto(sock, jsonString.c_str(), jsonString.size(), 0,
                           (struct sockaddr*)&receiverAddr, sizeof(receiverAddr));
    if (sentBytes < 0) {
        perror("Send failed");
        return -1;
    }

    std::cout << "Data sent: " << jsonString << std::endl;
    close(sock);
    return 0;
}


//udp_recieve.cpp
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


Compilation and Linking
g++ udp_send.cpp -o udp_send -L. -lcommand_target -ljsoncpp
g++ udp_receive.cpp -o udp_receive -ljsoncpp
