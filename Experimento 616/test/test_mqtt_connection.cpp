#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include "MQTTClient.h"

#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    ""
#define TOPIC       ""
#define QOS         0
#define TIMEOUT     10000L
std::string generateRandomID(int length)
{
    const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    const int charsetSize = sizeof(charset) - 1; // The size of the charset, minus the null terminator
    std::string id;
    srand(time(0)); // Use the current time as the seed for the random number generator
    
    for (int i = 0; i < length; i++)
    {
        int index = rand() % charsetSize;
        id += charset[index];
    }
    
    return id;
}
int main(int argc, char* argv[])
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc;
    std::string id = generateRandomID(5);
    MQTTClient_create(&client, ADDRESS, id.c_str(), MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

    printf("Connected to MQTT server\n");

    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}