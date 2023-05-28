#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_ADDRESS "localhost"
#define SERVER_PORT 1883

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }
    
    struct hostent* server = gethostbyname(SERVER_ADDRESS);
    if (server == nullptr)
    {
        std::cerr << "Error resolving server address" << std::endl;
        return 1;
    }
    
    struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = *(unsigned long*)server->h_addr;
    serverAddr.sin_port = htons(SERVER_PORT);
    
    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) != 0)
    {
        std::cerr << "Error connecting to server" << std::endl;
        return 1;
    }
    std::cout << "PASO EL CONNECT()" << std::endl;
    // Build the CONNECT message
    char connectMessage[] = {
    0x10, 0x26, // Fixed header: CONNECT message, no flags, message size = 38 bytes
    0x00, 0x04, 'M', 'Q', 'T', 'T', // Protocol name: MQTT, length = 4 bytes
    0x04, // Protocol level: 4
    0x02, // Connect flags: clean session
    0x00, 0x3C, // Keep-alive timer: 60 seconds
    0x00, 0x0A, 'm', 'y', '_', 'c', 'l', 'i','e', 'n', 't', '_', 'i', 'd', // Client ID: "my_client_id", length = 12 bytes
    0x00, 0x07, 't', 'o', 'p', 'i', 'c', // Will topic: "topic", length = 7 bytes
    0x00, 0x07, 'm', 'e', 's', 's', 'a', 'g', 'e', // Will message: "message", length = 7 bytes
    0x00, 0x04, 'u', 's', 'e', 'r', // Username: "user", length = 4 bytes
    0x00, 0x08, 'p', 'a', 's', 's', 'w', 'o', 'r', 'd' // Password: "password", length = 8 bytes
};
   std::cout << sizeof(connectMessage) << std::endl;
   if (send(sockfd, connectMessage, sizeof(connectMessage), 0) != sizeof(connectMessage))
    {
        std::cerr << "Error sending CONNECT message" << std::endl;
        return 1;
    }
    
    char connackBuffer[2];
    if (recv(sockfd, connackBuffer, sizeof(connackBuffer), 0) != sizeof(connackBuffer))
    {
        std::cerr << "Error receiving CONNACK message" << std::endl;
        return 1;
    }
    
    if (connackBuffer[0] != (char)0x20 || connackBuffer[1] != (char)0x02)
    {
        std::cerr << "Received message is not a CONNACK message" << std::endl;
        return 1;
    }
    
    std::cout << "Connection successful" << std::endl;

    // Send a disconnect message to the server
    std::string disconnect_message = "\xE0\x02\x00\x00";
    if (send(sockfd, disconnect_message.c_str(), disconnect_message.length(), 0) < 0) {
        std::cerr << "Error sending disconnect message" << std::endl;
        return 1;
    }

    // Wait for the disconnect message to be received
    char buffer[1024];
    while (true) {
        int bytes_received = recv(sockfd, buffer, sizeof(buffer), 0);
        if (bytes_received < 0) {
            std::cerr << "Error receiving data" << std::endl;
            break;
        } else if (bytes_received == 0) {
            std::cout << "Disconnected by server" << std::endl;
            break;
        } else {
            break;
        }
    }

    // Close the socket
    close(sockfd);

    std::cout << "Test complete" << std::endl;
    
    close(sockfd);
    
    return 0;
}