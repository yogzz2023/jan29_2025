To achieve this, I'll provide you with a sample `.h` and `.cpp` file for a class that contains the member variables `MSG_ID`, `Seq_no`, `range`, `azimuth`, and `elevation`. Then, I'll show you how to use this class in a separate program to send the data in JSON format over UDP. Finally, I'll provide a simple UDP receiver to demonstrate receiving the data.

### Sample Header File (`DataPacket.h`)

```cpp
#ifndef DATAPACKET_H
#define DATAPACKET_H

#include <string>

class DataPacket {
public:
    int MSG_ID;
    int Seq_no;
    double range;
    double azimuth;
    double elevation;

    DataPacket(int msgId, int seqNo, double rng, double az, double el)
        : MSG_ID(msgId), Seq_no(seqNo), range(rng), azimuth(az), elevation(el) {}

    std::string toJson() const;
};

#endif // DATAPACKET_H
```

### Sample Implementation File (`DataPacket.cpp`)

```cpp
#include "DataPacket.h"
#include <sstream>

std::string DataPacket::toJson() const {
    std::ostringstream oss;
    oss << "{"
        << "\"MSG_ID\":" << MSG_ID << ","
        << "\"Seq_no\":" << Seq_no << ","
        << "\"range\":" << range << ","
        << "\"azimuth\":" << azimuth << ","
        << "\"elevation\":" << elevation
        << "}";
    return oss.str();
}
```

### UDP Sender (`UdpSender.cpp`)

```cpp
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include "DataPacket.h"

int main() {
    DataPacket packet(1, 100, 50.5, 30.0, 10.0);
    std::string jsonData = packet.toJson();

    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    sendto(sockfd, jsonData.c_str(), jsonData.size(), 0, (const struct sockaddr*)&servaddr, sizeof(servaddr));
    std::cout << "Data sent: " << jsonData << std::endl;

    close(sockfd);
    return 0;
}
```

### UDP Receiver (`UdpReceiver.cpp`)

```cpp
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int sockfd;
    char buffer[1024];
    struct sockaddr_in servaddr, cliaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(8080);

    if (bind(sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        close(sockfd);
        return -1;
    }

    socklen_t len = sizeof(cliaddr);
    int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr, &len);
    buffer[n] = '\0';
    std::cout << "Received data: " << buffer << std::endl;

    close(sockfd);
    return 0;
}
```

### Explanation

1. **DataPacket Class**: This class holds the member variables and provides a method `toJson()` to convert the data to JSON format.

2. **UDP Sender**: This program creates a UDP socket, converts the `DataPacket` to JSON, and sends it to a specified address and port.

3. **UDP Receiver**: This program listens on a specified port for incoming UDP packets and prints the received JSON data.

Compile these files using a C++ compiler and run the receiver first, followed by the sender, to see the data being sent and received.