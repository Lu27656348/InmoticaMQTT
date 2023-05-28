//NOTA:
//sudo apt-get install libmqttclient-dev  ---> Instalador
//Primero, en experimento 616 hacer en el terminal: cd PubSub ---> direccion
//g++ subscriber.cpp -lpaho-mqtt3c -o subscriber --> correr

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <MQTTClient.h>

#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "Subscriber"
#define TOPIC       "test/topic"
#define QOS         1
#define TIMEOUT     10000L

int messageArrived(void* context, char* topicName, int topicLen, MQTTClient_message* message) {
    std::cout << "Mensaje recibido en el topic: " << topicName << std::endl;
    std::cout << "Contenido del mensaje: " << static_cast<char*>(message->payload) << std::endl;

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);

    return 1;
}

int main(int argc, char* argv[]) {
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;
    char* topicName = const_cast<char*>(TOPIC); // Definición de topicName

    // Conexión al broker MQTT
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        std::cout << "Fallo al conectar al broker. Código de retorno: " << rc << std::endl;
        return -1;
    }

    // Suscripción al topic
    MQTTClient_subscribe(client, topicName, QOS);

    // Bucle de recepción de mensajes
    while (1) {
        MQTTClient_message* message = NULL;
        int topicLen = 0; // Definición de topicLen
        rc = MQTTClient_receive(client, &topicName, &topicLen, &message, TIMEOUT);
        if (rc == MQTTCLIENT_SUCCESS) {
            messageArrived(NULL, topicName, topicLen, message);
            MQTTClient_freeMessage(&message);
            MQTTClient_free(topicName);
        } else if (rc == MQTTCLIENT_DISCONNECTED) {
            break;  // Salir del bucle si se pierde la conexión
        }
    }

    // Desconexión del broker MQTT
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);

    return 0;
}