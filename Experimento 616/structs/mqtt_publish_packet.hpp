#ifndef MQTT_PUBLISH_PACKET_H
#define MQTT_PUBLISH_PACKET_H
#include <cstdint>
#include <string>
// MQTT publish packet structure
struct mqtt_publish_packet {
    uint8_t header;
    uint8_t length;
    uint16_t topic_len;
    std::string topic;
    std::string payload;
};

#endif // !MQTT_PUBLISH_PACKET

