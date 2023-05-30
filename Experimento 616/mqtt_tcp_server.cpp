//NOTA:
//g++ -pthread mqtt_tcp_server.cpp -o server ----> compilador

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <algorithm>
#include <thread>
#include <bitset>
#include <atomic>
#include <mqtt/client.h>
#include <unordered_map>
#include "constans/mqtt_packet_types.hpp"
#include "constans/mqtt_packet_flags.hpp"
#include "structs/mqtt_fixed_header.hpp"
#include "structs/mqtt_connect_packet.hpp"
#include "structs/mqtt_disconnect_packet.hpp"
#include "structs/mqtt_publish_packet.hpp"
#include "structs/mqtt_subscribe_packet.hpp"
#include "classes/mqtt_broker.hpp"
#include <cstring>

std::atomic<bool> is_running(true);

mqtt_broker broker(1883);

class mqtt_subscribe_ack_packet {
public:
    uint8_t header;
    uint16_t packet_id;
    std::vector<uint8_t> return_codes;

    mqtt_subscribe_ack_packet() : header(0x90), packet_id(0) {}

    mqtt_subscribe_ack_packet(uint16_t id, const std::vector<uint8_t>& codes)
        : header(0x90), packet_id(id), return_codes(codes) {}

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> buffer;
        buffer.push_back(header);
        buffer.push_back(2 + return_codes.size());
        buffer.push_back(packet_id >> 8);
        buffer.push_back(packet_id & 0xFF);
        buffer.insert(buffer.end(), return_codes.begin(), return_codes.end());
        return buffer;
    }
};
// Parse incoming data as an MQTT packet
size_t write_packet_header(uint8_t* buffer, uint8_t packet_type, uint8_t remaining_length) {
    // Write the packet type to the buffer
    buffer[0] = packet_type;

    // Write the remaining length to the buffer
    size_t offset = 1;
    do {
        uint8_t digit = remaining_length % 128;
        remaining_length /= 128;
        if (remaining_length > 0) {
            digit |= 0x80;
        }
        buffer[offset++] = digit;
    } while (remaining_length > 0);

    return offset;
}
size_t write_uint16(uint8_t* buffer, uint16_t value) {
    buffer[0] = (value >> 8) & 0xFF;
    buffer[1] = value & 0xFF;
    return 2;
}
void parse_mqtt_packet(const char* data, size_t len,int client_socket) {

     if (len < 2) {
        std::cerr << "Error: Data too short to contain MQTT packet header" << std::endl;
        std::cout << "len "<< len << std::endl;
        return;
    }

     // Extract the packet type from the header
    uint8_t packet_type = (data[0] & 0xF0) >> 4;
    
    switch (packet_type) {
        case MQTT_CONNECT: {
            if (len < 12) {
                std::cerr << "Error: Data too short to contain MQTT CONNECT packet" << std::endl;
                std::cout << "len " << len << std::endl;
                return;
            }

            // Extraer los campos del paquete CONNECT
            mqtt_fixed_header header;
            header.control_packet_type =  (data[0] & 0xF0) >> 4;
            header.dup_flag = (data[0] & 0x08) >> 3;
            header.qos_level = (data[0] >> 5) & 3;
            header.retain = (data[0]) & 1;

            int pos = 1;
            unsigned char* buffer = reinterpret_cast<unsigned char*>(const_cast<char*>(data));
            header.remaining_length = read_length(buffer,pos);
            //Primero extraemos el header que es equivalente a 2 bytes
            //Los primero 4 bits del primer byte corresponde al tipo de mensaje el cual ya fue extraido arriba
            //Luego el 5 bit corresponde a la bandera DUP        
            //Luego el 6-7 bit corresponde a la bandera QOS
            //Luego el 8 bit corresponde a la bandera RETAIN
            //Luego el segundo byte corresponde al tamano del mensaje
            std::cout << (int)header.control_packet_type<< std::endl;
            std::cout << (int)header.dup_flag << std::endl;
            std::cout << (int)header.qos_level << std::endl;
            std::cout << (int)header.retain << std::endl;
            std::cout << (int)header.remaining_length << std::endl;
            
            mqtt_connect_packet connect;
            //Luego procedemos a extraer los datos del MQTT CONNECT MESSAGE
            // Obtenemos la longitud del nombre de protocolo
            // Extraer los campos del mensaje MQTT CONNECT
            // Protocol name:

            int16_t protocol_name_len = (data[pos] << 8) + data[pos+1];
            pos += 2;
            if (protocol_name_len != 4 || strncmp((char*)&data[pos], "MQTT", 4) != 0) {
                std::cerr << "Error: Incorrect protocol name or version" << std::endl;
                return;
            }
            pos += protocol_name_len;

            // Protocol level
            connect.protocol_version = data[pos];
            pos++;

            // Connect flags
            uint8_t connect_flags = data[pos];
            pos++;

            // Keep-alive timer
            uint16_t keep_alive_timer = (data[pos] << 8) + data[pos+1];
            pos += 2;

            // Client ID
            uint16_t client_id_len = (data[pos] << 8) + data[pos+1];
            pos += 2;
            connect.client_id = std::string((char*)&data[pos], client_id_len);
            pos += client_id_len;

            // Will topic and message
            if (connect_flags & 0x04) {
                uint16_t will_topic_len = (data[pos] << 8) + data[pos+1];
                pos += 2;
                pos += will_topic_len;
                uint16_t will_message_len = (data[pos] << 8) + data[pos+1];
                pos += 2;
            }

            // Username
            if (connect_flags & 0x80) {
                uint16_t username_len = (data[pos] << 8) + data[pos+1];
                pos += 2;
                pos += username_len;
            }

            // Password
            if (connect_flags & 0x40) {
                uint16_t password_len = (data[pos] << 8) + data[pos+1];
                pos += 2;
                pos += password_len;
            }

            // Imprimir los datos del mensaje MQTT CONNECT
            std::cout << "Protocol version: " << (int)connect.protocol_version << std::endl;
            std::cout << "Client ID: " << connect.client_id << std::endl;


            mqtt_client* new_client = new mqtt_client(client_socket,connect.client_id,true);

            broker.add_client(*new_client);

            char connack_packet[4] = {0x20, 0x02, 0x00, 0x00};
            send(client_socket, connack_packet, 4, 0);
            
            std::cout << "Received MQTT CONNECT packet" << std::endl;
            break;
        }
        case MQTT_PUBLISH: {
            std::cout << "Entrando al Publish\n";

            if (len < 5) {
                std::cerr << "Error: Data too short to contain MQTT PUBLISH packet" << std::endl;
                return;
            }
            
            mqtt_publish_packet *publish_packet = new mqtt_publish_packet();
            publish_packet->header = data[0];

            uint8_t qos = (publish_packet->header & 0x06) >> 1;
            bool retain = (publish_packet->header & 0x01) != 0;

            // Extract the topic from the PUBLISH packet variable header
            publish_packet->topic_len = (data[2] << 8) | data[3];
            publish_packet->topic.assign(&data[4], publish_packet->topic_len);

            // Extract the payload from the PUBLISH packet payload
            size_t payload_len = len - 4 - publish_packet->topic_len;
            publish_packet->payload.assign(&data[4+publish_packet->topic_len], payload_len);

            uint16_t* message_id = new uint16_t(0);
            if ((data[0] & 0x06) == MQTT_FLAG_QOS_1) {
                size_t header_len = 4 + publish_packet->topic_len;
                *message_id = (data[header_len] << 8) | data[header_len + 1];
            }

            std::cout << "Received MQTT PUBLISH packet" << std::endl;
            std::cout << "Topic: " << publish_packet->topic << std::endl;
            std::cout << "Payload: " << publish_packet->payload << std::endl;

            if ((data[0] & 0x06) == MQTT_FLAG_QOS_1) {
                std::cout << "Message_id: " << (int)*message_id << std::endl;
            }

            const std::string* topic_to_subscribe = new std::string(publish_packet->topic);
            const std::string* payload_to_send= new std::string(publish_packet->payload);
            for(auto client : broker.get_clients()){
                for(auto sub : client.get_subscriptions()){
                        if(sub == *topic_to_subscribe){
                            std::cout << "Hay un cliente suscrito" << std::endl;
                            std::cout << "Socket a enviar -> " << client.get_socket_fd() << std::endl;
                            std::cout << "Hay un cliente suscrito" << std::endl;
                            send(client.get_socket_fd(),data,len,0);
                        }
                }
            }
            std::cout << "Imprimimos topicos: " << std::endl;

            for(auto &client : broker.get_clients()){
                if(client.get_socket_fd() == client_socket){
                    std::cout << "SE ENCONTRO EL CLIENTE EN EL BROKER" << client.get_socket_fd() << std::endl;
                    const_cast<mqtt_client&>(client).add_subscription(*topic_to_subscribe);
                }
                
            }

            // Set the values for the PUBACK packet
            uint8_t packet_type = MQTT_PUBACK;
            uint8_t remaining_length = 2;
            uint16_t packet_id = 0;

            // Allocate memory for the PUBACK packet buffer
            size_t packet_size = remaining_length + 2;
            uint8_t* packet_buffer = new uint8_t[packet_size];

            size_t offset = 0;
            offset += write_packet_header(packet_buffer, packet_type, remaining_length);
            offset += write_uint16(packet_buffer + offset, packet_id);

            // Send the PUBACK packet over the socket
            size_t bytes_sent = send(client_socket, packet_buffer, packet_size, 0);
            
            if (bytes_sent < 0) {
                std::cout << "ERROR" << std::endl;
            } else {
                std::cout << "PUBACK sent succesfully" << std::endl;
            }
            
            // Free the memory allocated for the packet buffer
            delete[] packet_buffer;

            break;
        }
        case MQTT_SUBSCRIBE: {
            // Parse the SUBSCRIBE packet data here
            if (len < 3) {
                std::cerr << "Error: Data too short to contain MQTT SUBSCRIBE packet" << std::endl;
                return;
            }

            //Extraer los acmpos del paquete SUBSCRIBE
            mqtt_subscribe_packet* subscribe_packet = new mqtt_subscribe_packet();
            subscribe_packet->header = data[0];


            // Extract packet identifier
            subscribe_packet->packet_id = (data[2] << 8) | data[3];

            // Extract topics to subscribe
            size_t pos = 4;
            while (pos < len) {
                uint16_t topic_len = (data[pos] << 8) | data[pos + 1];
                std::string topic(reinterpret_cast<const char*>(data + pos + 2), topic_len);
                pos += 2 + topic_len;
                uint8_t qos = data[pos];
                subscribe_packet->topics.emplace_back(topic, qos);
                ++pos;
            }

            std::cout << "imprimiendo topico\n";
            for(auto& topic : subscribe_packet->topics){
                std::cout << topic.topic << std::endl;
            }

            std::string* topic_to_subscribe = new std::string(subscribe_packet->topics[0].topic);

            for(auto &client : broker.get_clients()){
                if(client.get_socket_fd() == client_socket){ 
                    std::cout << client.get_client_id();              
                    const_cast<mqtt_client&>(client).add_subscription(*topic_to_subscribe);
                    for(auto& topic : client.get_subscriptions()){
                        std::cout << topic << std::endl;
                    }
                }
            }
            
            std::cout << "Received MQTT SUBSCRIBE packet" << std::endl;
            std::cout << "Packet ID: " << subscribe_packet->packet_id << std::endl;
            for (const auto& topic : subscribe_packet->topics) {
                std::cout << "Topic: " << topic.topic << ", QoS: " << topic.qos << std::endl;
            }
            // Subscribe to the requested topics
            mqtt_subscribe_ack_packet* ack_packet = new mqtt_subscribe_ack_packet();
            ack_packet->header = MQTT_SUBACK << 4;
            ack_packet->packet_id = subscribe_packet->packet_id;

            for (const auto& topic : subscribe_packet->topics) {
                // TODO: Subscribe to the topic with the desired QoS level
                // ...

                // Add the topic to the ACK packet
                ack_packet->return_codes.emplace_back(0x00);
            }

            std::vector<uint8_t> buffer = ack_packet->serialize();

            uint8_t header = MQTT_SUBACK << 4;
            uint8_t remaining_length = buffer.size();

            // Construct the fixed header
            std::vector<uint8_t> fixed_header = {header, remaining_length};
            
            // Send the SUBSCRIBE ACK packet
            size_t bytes_fixed_header = send(client_socket, fixed_header.data(), fixed_header.size(), 0);
            size_t bytes_buffer = send(client_socket, buffer.data(), buffer.size(), 0);

            if (bytes_fixed_header == -1 || bytes_buffer == -1) {
                std::cerr << "Error: Failed to send SUBACK packet" << std::endl;
            } else {
                std::cout << "SUBACK packet sent successfully" << std::endl;
            }

            /*delete[] subscribe_packet;
            delete[] topic_to_subscribe;
            delete[] ack_packet;*/
            
            break;
        }
        case MQTT_DISCONNECT: {
            // Parse the DISCONNECT packet data here
            if (len < 2) {
                std::cerr << "Error: Data too short to contain MQTT DISCONNECT packet" << std::endl;
                return;
            }
            std::cout << "Received MQTT DISCONNECT packet" << std::endl;
            std::cout << "Cliente en el socket -> " << client_socket <<std::endl;

            char connackMessage[] = {0x20, 0x02, 0x00, 0x00};
            int numBytesSent = send(client_socket, connackMessage, sizeof(connackMessage), 0);

            broker.remove_client_bySocket(client_socket);
            std::cout << "-> -> -> CLIENTE REMOVIDO" << std::endl;
            broker.print_client();
            
            break;
        }
        // Add cases for other packet types here
        default:
            std::cerr << "Error: Unknown MQTT packet type" << std::endl;
            return;
    }

    memset((void *)data, 0, sizeof(data));
}

void handle_client(int client_socket) {
    std::cout << "Client connected in socket " << client_socket << std::endl;
    is_running.store(true);
    // Receive data from the client
    while (is_running) {
        char buffer[1024] = {0};    
        
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if(bytes_received != -1){
            std::cout << "Received " << bytes_received << " bytes: " << std::endl;
        }
        if (bytes_received < 0) {
            std::cerr << "Error receiving data or" << std::endl;
        } else if (bytes_received == 0) {
            std::cout << "Client disconnected" << std::endl;
            is_running.store(false);
        } else {
            parse_mqtt_packet(buffer,bytes_received,client_socket);
        }
        std::vector<mqtt_client> clients = broker.get_clients();
        std::cout << "Imprimiendo clientes" << std::endl;
        for(auto &client : clients){
            std::cout << "Client en el socket -> " << client.get_socket_fd() << std::endl;
            std::vector<std::string> topics = client.get_subscriptions();
            std::cout << "     topicos del cliente ->" << std::endl;
            for(auto &topic : topics){
                std::cout << "      "<<topic << std::endl;
            }
        }
        //std::cout << "Presione enter para seguir..." << std::endl;
        //std::cin.get();

    }
    std::cout << "saliendo de handle_client()" << std::endl;
    close(client_socket);
}

int main(int argc, char *argv[]) {
    
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    std::string server_ip = "127.0.0.1";
    int server_port = 1883;
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(server_ip.c_str());
    server_address.sin_port = htons(server_port);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Error binding socket - Free the port 1883 and wait 1 minute" << std::endl;
        return 1;
    }

    if (listen(server_socket, 5) < 0) {
        std::cerr << "Error listening on socket" << std::endl;
        return 1;
    }

    std::cout << "Server listening on " << server_ip << ":" << server_port << std::endl;

    while (true){
        int client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket < 0) {
            std::cerr << "Error accepting connection" << std::endl;
            break;
        }

        // Handle the client connection in a separate thread
        std::thread(handle_client, client_socket).detach();


    }

    close(server_socket);
    return 0;
}
