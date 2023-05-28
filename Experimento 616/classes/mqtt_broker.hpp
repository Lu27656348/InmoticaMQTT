#ifndef MQTT_BROKER_H
#define MQTT_BROKER_H

#include "mqtt_client.hpp"
#include <iostream>

class mqtt_broker {
public:
    mqtt_broker(int port_): clients(){
        port = port_;
    }
    void add_client(mqtt_client &client) {
        clients.push_back(client);
    }

    void remove_client(const std::string& client_id) {
        clients.erase(std::remove_if(clients.begin(), clients.end(),
                                     [&](const mqtt_client& client) {
                                         return client.get_client_id() == client_id;
                                     }),
                       clients.end());
    }

    void remove_client_bySocket(const int sockedfd) {
        clients.erase(std::remove_if(clients.begin(), clients.end(),
                                     [&](const mqtt_client& client) {
                                         return client.get_socket_fd() == sockedfd;
                                     }),
                       clients.end());
    }
    void print_client(){
        std::cout << "PRINT" << std::endl;
        std::cout << "Estan conectados -> " << clients.size() << std::endl;
        for( auto&Client : clients){
            if(Client.get_client_id() == ""){
                std::cout << " -> -> Cliente sin ID" << std::endl;
            }else{
                std::cout << Client.get_client_id() << std::endl;
            }
            
        }
        std::cout << "END-PRINT" << std::endl;
    }

    const std::vector<mqtt_client>& get_clients() const {
        return clients;
    }

private:
    int port;
    std::vector<mqtt_client> clients;
};

#endif