#ifndef MQTT_PACKET_FLAGS_H
#define MQTT_PACKET_FLAGS_H

// MQTT packet header flags
#define MQTT_FLAG_RETAIN 0x01
#define MQTT_FLAG_QOS_0 0x00
#define MQTT_FLAG_QOS_1 0x02
#define MQTT_FLAG_QOS_2 0x04
#define MQTT_FLAG_DUP 0x08

#endif // MQTT_PACKET_FLAGS_H