#ifndef MQTT_FIXED_HEADER_H
#define MQTT_FIXED_HEADER_H
#include <cstdint>
struct mqtt_fixed_header {
    uint8_t control_packet_type : 4;
    bool dup_flag;
    uint8_t qos_level : 2;
    uint8_t retain : 1;
    uint8_t remaining_length;
};
int read_length(unsigned char* buffer, int& pos) {
    int multiplier = 1;
    int value = 0;
    unsigned char encoded_byte;
    do {
        encoded_byte = buffer[pos];
        value += (encoded_byte & 127) * multiplier;
        multiplier *= 128;
        pos++;
    } while ((encoded_byte & 128) != 0);
    return value;
}
#endif // !MQTT_FIXED_HEADER_H