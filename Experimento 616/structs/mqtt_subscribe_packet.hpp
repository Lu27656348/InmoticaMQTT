#ifndef MQTT_SUBSCRIBE_PACKET_H
#define MQTT_SUBSCRIBE_PACKET_H

#include <cstdint>
#include <vector>

#include "mqtt_subscribe_topic.hpp"

struct mqtt_subscribe_packet {
    uint8_t header;
    uint16_t packet_id;
    std::vector<mqtt_subscribe_topic> topics;

    mqtt_subscribe_packet() : header(0x82), packet_id(0) {}

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> buffer;
        buffer.push_back(header);
        buffer.push_back(2 + get_variable_header_length() + get_payload_length());
        buffer.push_back(packet_id >> 8);
        buffer.push_back(packet_id & 0xFF);
        for (const auto& topic : topics) {
            buffer.push_back(topic.topic.length() >> 8);
            buffer.push_back(topic.topic.length() & 0xFF);
            buffer.insert(buffer.end(), topic.topic.begin(), topic.topic.end());
            buffer.push_back(topic.qos);
        }
        return buffer;
    }

    size_t get_variable_header_length() const {
        return 2; // packet identifier
    }

    size_t get_payload_length() const {
        size_t len = 0;
        for (const auto& topic : topics) {
        len += 2 + topic.topic.length() + 1; // 2 bytes for topic length, topic string, and 1 byte for QoS level
        }
        return len;
    }
};

#endif // !
