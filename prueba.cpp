#include <iostream>
#include <mosquitto.h>

int main()
{
    struct mosquitto *mosq = NULL;
    int rc;
    
    mosquitto_lib_init();

    mosq = mosquitto_new(NULL, true, NULL);

    rc = mosquitto_connect(mosq, "localhost", 1883, 60);

    if(rc != 0)
    {
        std::cout << "Error al conectarse al broker: " << mosquitto_strerror(rc) << std::endl;
        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
        return -1;
    }

    rc = mosquitto_publish(mosq, NULL, "test", 5, "Hola Mundo", 0, false);

    if(rc != MOSQ_ERR_SUCCESS)
    {
        std::cout << "Error al publicar el mensaje: " << mosquitto_strerror(rc) << std::endl;
        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
        return 1;
    }

    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}