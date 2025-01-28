Here’s the sample implementation based on your requirements:

### Header File: `MessageData.h`

This defines the structure and member variables.

```cpp
#ifndef MESSAGE_DATA_H
#define MESSAGE_DATA_H

#include <string>

class MessageData {
public:
    int MSG_ID;
    int Seq_no;
    double range;
    double azimuth;
    double elevation;

    // Constructor
    MessageData(int msg_id, int seq_no, double range, double azimuth, double elevation);

    // Method to convert object to JSON string
    std::string toJson() const;
};

#endif // MESSAGE_DATA_H
```

---

### Source File: `MessageData.cpp`

This implements the methods of the `MessageData` class.

```cpp
#include "MessageData.h"
#include <sstream>
#include <iomanip>

// Constructor
MessageData::MessageData(int msg_id, int seq_no, double range, double azimuth, double elevation)
    : MSG_ID(msg_id), Seq_no(seq_no), range(range), azimuth(azimuth), elevation(elevation) {}

// Method to convert object to JSON string
std::string MessageData::toJson() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "{"
        << "\"MSG_ID\": " << MSG_ID << ", "
        << "\"Seq_no\": " << Seq_no << ", "
        << "\"range\": " << range << ", "
        << "\"azimuth\": " << azimuth << ", "
        << "\"elevation\": " << elevation
        << "}";
    return oss.str();
}
```

---

### UDP Sender Code: `UdpSender.cpp`

This code sends the data in JSON format to a UDP receiver.

```cpp
#include "MessageData.h"
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

int main() {
    // Create a MessageData object
    MessageData message(1, 1001, 500.5, 1.234, 0.567);

    // Convert the message to JSON format
    std::string jsonMessage = message.toJson();

    // Create a UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }

    // Server address setup
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    // Send the JSON message
    int sendResult = sendto(sock, jsonMessage.c_str(), jsonMessage.size(), 0,
                            (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (sendResult < 0) {
        std::cerr << "Error sending data" << std::endl;
    } else {
        std::cout << "Message sent: " << jsonMessage << std::endl;
    }

    // Close the socket
    close(sock);
    return 0;
}
```

---

### UDP Receiver Code: `UdpReceiver.cpp`

This code receives the JSON message sent by the UDP sender.

```cpp
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 8080

int main() {
    // Create a UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }

    // Server address setup
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the port
    if (bind(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Error binding socket" << std::endl;
        close(sock);
        return -1;
    }

    // Buffer to store received data
    char buffer[1024];
    sockaddr_in clientAddr{};
    socklen_t clientAddrLen = sizeof(clientAddr);

    std::cout << "UDP receiver is running on port " << SERVER_PORT << "..." << std::endl;

    // Receive data
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int recvLen = recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
                               (sockaddr*)&clientAddr, &clientAddrLen);
        if (recvLen > 0) {
            buffer[recvLen] = '\0'; // Null-terminate the received data
            std::cout << "Received message: " << buffer << std::endl;
        }
    }

    // Close the socket
    close(sock);
    return 0;
}
```

---

### Compilation and Execution

1. Compile the files:
   ```bash
   g++ -o sender UdpSender.cpp MessageData.cpp
   g++ -o receiver UdpReceiver.cpp
   ```

2. Run the receiver in one terminal:
   ```bash
   ./receiver
   ```

3. Run the sender in another terminal:
   ```bash
   ./sender
   ```

---

### Expected Output

#### Sender Terminal:
```
Message sent: {"MSG_ID": 1, "Seq_no": 1001, "range": 500.50, "azimuth": 1.23, "elevation": 0.57}
```

#### Receiver Terminal:
```
UDP receiver is running on port 8080...
Received message: {"MSG_ID": 1, "Seq_no": 1001, "range": 500.50, "azimuth": 1.23, "elevation": 0.57}
```