#include <iostream>
#include <cstring>
#include <cstdlib>
#include <mqtt/async_client.h>
#include <chrono>
#include <thread>

//#define ADDRESS     "tcp://10.68.17.52:1883"
#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "Subscriber"
#define TOPIC       "test/topic"
#define QOS         0

class MessageCallback : public virtual mqtt::callback
{
    void message_arrived(mqtt::const_message_ptr msg) override {
        std::cout << "Mensaje recibido en el topic: " << msg->get_topic() << std::endl;
        std::cout << "Contenido del mensaje: " << msg->to_string() << std::endl;
    }
};

int main(int argc, char* argv[]) {
    mqtt::async_client client(ADDRESS, CLIENTID);
    mqtt::connect_options conn_opts;
    conn_opts.set_clean_session(true);

    MessageCallback cb;
    client.set_callback(cb);

    try {
        mqtt::token_ptr conntok = client.connect(conn_opts);
        std::cout << "Conectado al broker MQTT" << std::endl;
        conntok->wait();
    }
    catch (const mqtt::exception&exc) {
        std::cerr << "Error al conectar al broker MQTT: " << exc.what() << std::endl;
        return 1;
    }

    mqtt::token_ptr subtok = client.subscribe(TOPIC, QOS);
    std::cout << "Suscrito al topic: " << TOPIC << std::endl;
    subtok->wait();

    while (true) {
        std::cout << "Esperando mensajes..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(10));
        std::cout << "Realizando tarea..." << std::endl;
        // Realizar tarea aquí
    }

    return 0;
}
