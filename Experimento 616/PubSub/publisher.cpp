///NOTA:
//sudo apt-get install libmqttclient-dev  ---> Instalador
//Primero, en experimento 616 hacer en el terminal: cd PubSub ---> direccion
//g++ publisher.cpp -lpaho-mqtt3c -o publisher --> correr

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <MQTTClient.h>

#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "Publisher"
#define TOPIC       "test/topic"
#define QOS         1
#define TIMEOUT     10000L

int main(int argc, char* argv[]) {
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc;

    // Conexión al broker MQTT
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        std::cout << "Fallo al conectar al broker. Código de retorno: " << rc << std::endl;
        return -1;
    }

    // Publicar un mensaje en el topic
    pubmsg.payload = (void*)"Mensaje de prueba";
    pubmsg.payloadlen = strlen((char*)pubmsg.payload);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
    std::cout << "Mensaje publicado en el topic: " << TOPIC << std::endl;

    // Esperar por la entrega del mensaje
    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    std::cout << "Mensaje entregado" << std::endl;

    // Desconexión del broker MQTT
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);

    return 0;
}
