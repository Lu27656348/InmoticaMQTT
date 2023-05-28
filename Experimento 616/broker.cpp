#include <iostream>
#include <map>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

#define MAX_CLIENTS 10
#define MAX_PACKET_SIZE 1024

enum class MessageType {
    CONNECT,
    CONNACK,
    PUBLISH,
    PUBACK,
    SUBSCRIBE,
    SUBACK,
    UNSUBSCRIBE,
    UNSUBACK,
    PINGREQ,
    PINGRESP,
    DISCONNECT
};

struct Message {
    MessageType type;
    uint8_t* data;
    size_t length;
};

struct Client {
    int socket;
    uint16_t keep_alive;
    bool connected;
    uint32_t last_ping;
    uint8_t* buffer;
    size_t buffer_length;
    map<uint16_t, uint8_t> subscriptions;
};

int create_socket(const char* address, int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        cerr << "Error creating socket" << endl;
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(address);
    serv_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        cerr << "Error binding socket to address" << endl;
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, MAX_CLIENTS) < 0) {
        cerr << "Error listening on socket" << endl;
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

void send_message(int socket, const Message& message) {
    uint8_t header = static_cast<uint8_t>(message.type);
    uint8_t byte;
    size_t remaining_length = message.length;
    vector<uint8_t> remaining_bytes;

    do {
        byte = remaining_length % 128;
        remaining_length /= 128;
        if (remaining_length > 0) {
            byte |= 0x80;
        }
        remaining_bytes.push_back(byte);
    } while (remaining_length > 0);

    uint8_t packet[MAX_PACKET_SIZE];
    size_t packet_length = 0;

    packet[packet_length++] = header;
    packet_length += remaining_bytes.size();
    for (auto it = remaining_bytes.rbegin(); it != remaining_bytes.rend(); ++it) {
        packet[packet_length++] = *it;
    }
    memcpy(packet + packet_length, message.data, message.length);
    packet_length += message.length;

    ssize_t sent = send(socket, packet, packet_length, 0);
}

void handle_connect(Client& client, const uint8_t* data, size_t length) {
    // Parse variable header
    size_t offset = 0;
    uint16_t protocol_name_length = (data[offset] << 8) | data[offset + 1];
    offset += 2;
    string protocol_name(reinterpret_cast<const char*>(data + offset), protocol_name_length);
    offset += protocol_name_length;
    uint8_t protocol_version = data[offset++];
    uint8_t connect_flags = data[offset++];
    uint16_t keep_alive = (data[offset] << 8) | data[offset + 1];
    offset += 2;

    // Parse payload
    uint16_t client_id_length = (data[offset] << 8) | data[offset + 1];
    offset += 2;
    string client_id(reinterpret_cast<const char*>(data + offset), client_id_length);

    // TODO: Validate CONNECT packet fields

    // Add client ID to map
    client.connected = true;
    client.keep_alive = keep_alive;
    client.last_ping = time(nullptr);
    client.subscriptions.clear();
    cout << "Client " << client_id << " connected" << endl;

    // Send CONNACK packet
    uint8_t connack_packet[] = {0x20, 0x02,0x00, 0x00}; // Fixed header + variable header + payload
    send_message(client.socket, {MessageType::CONNACK, connack_packet, sizeof(connack_packet)});
}
int main() {
    int sockfd = create_socket("127.0.0.1", 1883);
    vector<Client> clients;

    while (true) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);
        int max_fd = sockfd;

        for (auto& client : clients) {
            FD_SET(client.socket, &read_fds);
            if (client.socket > max_fd) {
                max_fd = client.socket;
            }
        }

        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int activity = select(max_fd + 1, &read_fds, nullptr, nullptr, &tv);
        if (activity < 0) {
            cerr << "Error in select" << endl;
            exit(EXIT_FAILURE);
        }

        if (activity == 0) {
            // Handle timeouts
            for (auto& client : clients) {
                if (client.connected && (time(nullptr) - client.last_ping > client.keep_alive)) {
                    cerr << "Client timed out" << endl;
                    close(client.socket);
                    client.connected = false;
                }
            }
            continue;
        }

        if (FD_ISSET(sockfd, &read_fds)) {
            // Handle new connections
            struct sockaddr_in client_addr;
            socklen_t addrlen = sizeof(client_addr);
            int newsockfd = accept(sockfd, (struct sockaddr*)&client_addr, &addrlen);
            if (newsockfd < 0) {
                cerr << "Error accepting new connection" << endl;
                continue;
            }

            if (clients.size() >= MAX_CLIENTS) {
                cerr << "Too many clients, rejecting connection" << endl;
                close(newsockfd);
                continue;
            }

            clients.push_back({newsockfd, 0, false, 0, new uint8_t[MAX_PACKET_SIZE], 0});
            cout << "Client connected" << endl;
        }

        for (auto& client : clients) {
            if (FD_ISSET(client.socket, &read_fds)) {
                // Handle data from client
                ssize_t received = recv(client.socket, client.buffer + client.buffer_length, MAX_PACKET_SIZE - client.buffer_length, 0);
                if (received <= 0) {
                    cerr << "Error receiving data from client" << endl;
                    close(client.socket);
                    client.connected = false;
                    continue;
                }

                client.buffer_length += received;

                // Parse packets from buffer
                size_t offset = 0;
                while (offset < client.buffer_length) {
                    uint8_t header = client.buffer[offset++];
                    MessageType type = static_cast<MessageType>(header & 0xF0);
                    size_t remaining_length = 0;
                    uint8_t byte;

                    do {
                        byte = client.buffer[offset++];
                        remaining_length += (byte & 0x7F) << (7 * (offset - 2));
                    } while ((byte & 0x80) != 0);

                    if (remaining_length + offset > client.buffer_length) {
                        // Not enough data in buffer, wait for more
                        break;
                    }

                    Message message = {type, client.buffer + offset, remaining_length};
                    offset += remaining_length;

                    switch (message.type) {
                        case MessageType::CONNECT: {
                            std::cout << "CONNECT" << std::endl;
                            // Handle CONNECT packet
                            handle_connect(client, message.data, message.length);
                            // TODO: Implement CONNECT packet handling
                            break;
                        }
                        case MessageType::PUBLISH: {
                            // Handle PUBLISH packet
                            std::cout << "PUBLISH" << std::endl;
                            // TODO: Implement PUBLISH packet handling
                            break;
                        }
                        case MessageType::SUBSCRIBE: {
                            // Handle SUBSCRIBE packet
                            std::cout << "SUBSCRIBE" << std::endl;
                            // TODO: Implement SUBSCRIBE packet handling
                            break;
                        }
                        case MessageType::UNSUBSCRIBE: {
                            // Handle UNSUBSCRIBE packet
                            std::cout << "UNSUSCRIBE" << std::endl;
                            // TODO: Implement UNSUBSCRIBE packet handling
                            break;
                        }
                        case MessageType::PINGREQ: {
                            // Handle PINGREQ packet
                            // TODO: Implement PINGREQ packet handling
                            break;
                        }
                        case MessageType::DISCONNECT: {
                            // Handle DISCONNECT packet
                            std::cout << "DISCONNECT" << std::endl;
                            // TODO: Implement DISCONNECT packet handling
                            break;
                        }
                        default: {
                            cerr << "Unsupported message type: " << static_cast<int>(message.type) << endl;
                            break;
                        }
                                   }

                // Shift buffer contents and update buffer length
                size_t remaining_bytes = client.buffer_length - offset;
                if (remaining_bytes > 0) {
                    memmove(client.buffer, client.buffer + offset, remaining_bytes);
                }
                client.buffer_length = remaining_bytes;

                // Update last ping time for connected clients
                if (client.connected && message.type == MessageType::PINGREQ) {
                    client.last_ping = time(nullptr);
                }
            }
        }
    }

    return 0;
}
}