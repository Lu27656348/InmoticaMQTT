#include <iostream>
#include <cstring>
#include <cstdlib>
#include <mosquitto.h>

#define ADDRESS     "localhost"
#define PORT        1883
#define CLIENTID    "Publisher"
#define TOPIC       "test/topic"
#define QOS         1

int main(int argc, char* argv[]) {
    struct mosquitto* mosq = NULL;
    int rc;

    // Inicialización de Mosquitto
    mosquitto_lib_init();

    // Creación del cliente Mosquitto
    mosq = mosquitto_new(CLIENTID, true, NULL);
    if (!mosq) {
        std::cout << "Fallo al crear el cliente Mosquitto." << std::endl;
        return -1;
    }

    // Conexión al broker MQTT
    rc = mosquitto_connect(mosq, ADDRESS, PORT, 0);
    if (rc != MOSQ_ERR_SUCCESS) {
        std::cout << "Fallo al conectar al broker MQTT. Código de retorno: " << rc << std::endl;
        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
        return -1;
    }

    // Publicar un mensaje en el topic
    rc = mosquitto_publish(mosq, NULL, TOPIC, strlen("Mensaje de prueba"), "Mensaje de prueba", QOS, false);
    if (rc != MOSQ_ERR_SUCCESS) {
        std::cout << "Fallo al publicar el mensaje. Código de retorno: " << rc << std::endl;
        mosquitto_disconnect(mosq);
        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
        return -1;
    }

    std::cout << "Mensaje publicado en el topic: " << TOPIC << std::endl;

    // Esperar a que se envíe el mensaje
    mosquitto_loop_start(mosq);
    mosquitto_loop_stop(mosq, true);

    std::cout << "Mensaje entregado" << std::endl;

    // Desconexión del broker MQTT
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}