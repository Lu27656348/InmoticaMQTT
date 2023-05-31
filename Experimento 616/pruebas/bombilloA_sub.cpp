#include <iostream>
#include <cstring>
#include <cstdlib>
#include <mqtt/async_client.h>
#include <chrono>
#include <thread>

//g++ -o bombilloA bombilloA_sub.cpp -lpaho-mqttpp3 -lpaho-mqtt3as -lpthread

//#define ADDRESS "tcp://localhost:1883"
#define ADDRESS "tcp://10.68.17.52:1883"
#define CLIENTID "Subscriber"
#define QOS 0

class BombilloSubscriber : public virtual mqtt::callback {
private:
    int id;
    std::string topic;
    std::string status;

    mqtt::async_client client;
    mqtt::connect_options conn_opts;

public:
    BombilloSubscriber(int id, const std::string& topic) : id(id), topic(topic), status("OFF"), client(ADDRESS, CLIENTID) {
        conn_opts.set_clean_session(true);
        client.set_callback(*this);

        try {
            mqtt::token_ptr conntok = client.connect(conn_opts);
            std::cout << "Conectado al broker MQTT" << std::endl;
            conntok->wait();
        }
        catch (const mqtt::exception& exc) {
            std::cerr << "Error al conectar al broker MQTT: " << exc.what() << std::endl;
            std::exit(1);
        }

        mqtt::token_ptr subtok = client.subscribe(topic, QOS);
        std::cout << "Suscrito al topic: " << topic << std::endl;
        subtok->wait();
    }

    void message_arrived(mqtt::const_message_ptr msg) override {
        std::string payload = msg->to_string();

        if (payload == "ON" && status == "OFF") {
            status = "ON";
            std::cout << "El bombillo con ID " << id << " ha cambiado su estado a ON" << std::endl;
        }
        else if (payload == "OFF" && status == "ON") {
            status = "OFF";
            std::cout << "El bombillo con ID " << id << " ha cambiado su estado a OFF" << std::endl;
        }
        else if (payload != "ON" && payload != "OFF") {
            std::cout << "El payload \"" << payload << "\" no es aceptado. El bombillo con ID " << id << " permanece en estado " << status << std::endl;
        }
    }
};

int generate_id() {
        // Obtener el tiempo actual en segundos
        time_t tiempoActual = time(nullptr);

        // Convertir el tiempo a una cadena de caracteres
        std::string tiempoActualStr = std::to_string(tiempoActual);

        // Generar un hash único para la cadena de tiempo
        size_t hashedTiempoActual = std::hash<std::string>{}(tiempoActualStr);

        // Mostrar solo la sección del ID que se desea
        int id = hashedTiempoActual % 1000000;

        std::cout << "ID generado -> " << id << std::endl;

        return id;
}

int main() {
    //int id = generate_id();
    BombilloSubscriber bombillo(1, "test/bombillo/1/");

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    return 0;
}
