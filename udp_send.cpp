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
