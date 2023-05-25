///NOTA:
//sudo apt-get install libmqttclient-dev  ---> Instalador
//g++ mqtt_publisher.cpp -lpaho-mqtt3c -o mqtt_publisher --> correr

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <MQTTClient.h>

#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "SimpleBroker"
#define QOS         1

// Estructura para almacenar la información del cliente
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <MQTTClient.h>

#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "SimpleBroker"
#define QOS         1

// Estructura para almacenar la información del cliente
struct ClientInfo {
    std::string clientId;
    std::string subscribedTopic;
};

// Vector para almacenar la información de los clientes conectados
std::vector<ClientInfo> clients;

// Función de callback para recibir mensajes publicados
int messageArrived(void* context, char* topicName, int topicLen, MQTTClient_message* message) {
    std::string topic(topicName, topicLen);
    std::string payload(static_cast<char*>(message->payload), message->payloadlen);
    
    std::cout << "Mensaje recibido en el topic: " << topic << std::endl;
    std::cout << "Contenido del mensaje: " << payload << std::endl;

    // Enviar el mensaje a los clientes suscritos al topic
    for (const auto& client : clients) {
        if (client.subscribedTopic == topic) {
            MQTTClient_publish(context, client.clientId.c_str(), payload.size(), payload.c_str(), QOS, 0, NULL);
        }
    }

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);

    return 1;
}

int main(int argc, char* argv[]) {
    MQTTClient client;
    MQTTClient_connectOptions connOpts = MQTTClient_connectOptions_initializer;
    int rc;

    // Inicialización del cliente MQTT
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    connOpts.keepAliveInterval = 20;
    connOpts.cleansession = 1;

    // Conexión al broker MQTT
    if ((rc = MQTTClient_connect(client, &connOpts)) != MQTTCLIENT_SUCCESS) {
        std::cout << "Fallo al conectar al broker. Código de retorno: " << rc << std::endl;
        return -1;
    }

    // Suscripción a todos los clientes conectados
   for (const auto& clientInfo : clients) {
        MQTTClient_subscribe(client, clientInfo.subscribedTopic.c_str(), QOS);
    }


    // Configuración de la función de callback para recibir mensajes
    MQTTClient_setCallbacks(client, NULL, NULL, messageArrived, NULL);

    // Ciclo principal para recibir mensajes
    while (true) {
        MQTTClient_yield();
    }

    // Desconexión del broker MQTT
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);

    return 0;
}
