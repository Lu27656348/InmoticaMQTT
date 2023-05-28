#ifndef MQTT_SUBSCRIBE_TOPIC_H
#define MQTT_SUBSCRIBE_TOPIC_H

#include <string>
#include <cstdint>

struct mqtt_subscribe_topic {
    std::string topic;
    uint8_t qos;

    mqtt_subscribe_topic(const std::string& t, uint8_t q) : topic(t), qos(q) {}
};

#endif // !

