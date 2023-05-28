#ifndef MQTT_CONNECT_PACKET_H
#define MQTT_CONNECT_PACKET_H

#include <string>
#include "mqtt_fixed_header.hpp"

// MQTT connect packet structure
struct mqtt_connect_packet {
    mqtt_fixed_header header;
    uint8_t protocol_version : 4;
    uint8_t protocol_name_len;
    char protocol_name[6];
    uint8_t protocol_level;
    uint8_t connect_flags;
    uint16_t keep_alive;
    std::string client_id;
};

#endif // MQTT_CONNECT_PACKET_H