#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>

#define MQTT_PUBLISH 3
#define MQTT_PUBACK 4

int main() {
    // Create a TCP socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "Error: Failed to create socket" << std::endl;
        return 1;
    }

    // Connect to the MQTT broker
    sockaddr_in broker_address{};
    broker_address.sin_family = AF_INET;
    broker_address.sin_port = htons(1883);
    inet_pton(AF_INET, "127.0.0.1", &broker_address.sin_addr);

    if (connect(client_socket, reinterpret_cast<sockaddr*>(&broker_address), sizeof(broker_address)) == -1) {
        std::cerr << "Error: Failed to connect to MQTT broker" << std::endl;
        close(client_socket);
        return 1;
    }

    // Construct a PUBLISH packet
    std::string topic = "test/topic";
    std::string payload = "Hello, world!";
    uint8_t qos = 1;
    bool retain = false;

    // Construct the variable header
    std::vector<uint8_t> variable_header;
    variable_header.emplace_back(0x00); // Length MSB
    variable_header.emplace_back(topic.size()); // Length LSB
    variable_header.insert(variable_header.end(), topic.begin(), topic.end()); // Topic name
    if (qos > 0) {
        // Add the packet ID for QoS 1 or 2
        uint16_t packet_id = 1234; // Replace with a unique packet ID
        variable_header.emplace_back(packet_id >> 8); // Packet ID MSB
        variable_header.emplace_back(packet_id & 0xFF); // Packet ID LSB
    }

    // Construct the payload
    std::vector<uint8_t> payload_data(payload.begin(), payload.end());

    // Construct the fixed header
    uint8_t fixed_header = static_cast<uint8_t>(MQTT_PUBLISH << 4) | (qos << 1) | retain;
    std::vector<uint8_t> fixed_header_data = { fixed_header };

    // Construct the entire PUBLISH packet
    std::vector<uint8_t> publish_packet;
    publish_packet.reserve(fixed_header_data.size() + variable_header.size() + payload_data.size());
    publish_packet.insert(publish_packet.end(), fixed_header_data.begin(), fixed_header_data.end());
    publish_packet.insert(publish_packet.end(), variable_header.begin(), variable_header.end());
    publish_packet.insert(publish_packet.end(), payload_data.begin(), payload_data.end());

    // Send the PUBLISH packet

    if (send(client_socket, publish_packet.data(), publish_packet.size(), 0) == -1) {
        std::cerr << "Error: Failed to send PUBLISH packet" << std::endl;
        close(client_socket);
        return 1;
    }

    // Receive the PUBACK packet
    std::vector<uint8_t> puback_packet(2);
    ssize_t bytes_received = recv(client_socket, puback_packet.data(), puback_packet.size(), 0);
    if (bytes_received == -1) {
        std::cerr << "Error: Failed to receive PUBACK packet" << std::endl;
        close(client_socket);
        return 1;
    } else if (bytes_received != 2) {
        std::cerr << "Error: Received invalid PUBACK packet" << std::endl;
        close(client_socket);
        return 1;
    }

    // Verify that the PUBACK packet matches the PUBLISH packet
    if (puback_packet[0] != static_cast<uint8_t>(MQTT_PUBACK << 4) || puback_packet[1] != 2 ||
        puback_packet[2] != publish_packet[2] || puback_packet[3] != publish_packet[3]) {
        std::cerr << "Error: Received incorrect PUBACK packet" << std::endl;
        close(client_socket);
        return 1;
    }

    // Display a success message
    std::cout << "Message published successfully" << std::endl;

    // Close the socket
    close(client_socket);

    return 0;
}