///NOTA:
//sudo apt-get install libmqttclient-dev  ---> Instalador
//Primero, en experimento 616 hacer en el terminal: cd PubSub ---> direccion
//g++ termometro_pub.cpp -lpaho-mqttpp3 -lpaho-mqtt3a -o t_pub --> correr

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <mqtt/async_client.h>
#include <random>
#include <chrono>
#include <thread>

//#define ADDRESS "tcp://localhost:1883"
#define ADDRESS "tcp://10.68.17.52:1883"

class TemperaturePublisher {
private:
    int id;
    std::string topic;
    mqtt::async_client client;
    mqtt::connect_options conn_opts;

public:
    TemperaturePublisher(int id, const std::string& topic) : id(id), topic(topic), client(ADDRESS, "Publisher") {
        conn_opts.set_clean_session(true);

        try {
            mqtt::token_ptr conntok = client.connect(conn_opts);
            std::cerr << "Conectado al broker MQTT" << std::endl;
            conntok->wait();
        }
        catch (const mqtt::exception& exc) {
            std::cerr << "Error al conectar al broker MQTT: " << exc.what() << std::endl;
            std::exit(-1);
        }
    }

    void publicarTemperatura(double temperatura) {
        std::string mensaje = std::to_string(temperatura);
        mqtt::message_ptr pubmsg = mqtt::make_message(topic, mensaje);

        try {
            mqtt::token_ptr pubtok = client.publish(pubmsg);
            std::cerr << "Temperatura publicada: " << temperatura << std::endl;
            pubtok->wait();
        }
        catch (const mqtt::exception& exc) {
            std::cerr << "Error al publicar la temperatura: " << exc.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1)); // Esperar 1 segundo antes de reintentar
            try {
                client.reconnect();
            } catch (const mqtt::exception& exc) {
                std::cerr << "Error al reconectar: " << exc.what() << std::endl;
            }
        }
    }

    ~TemperaturePublisher() {
        try {
            client.disconnect()->wait();
        }
        catch (const mqtt::exception& exc) {
            std::cerr << "Error al desconectar del broker MQTT: " << exc.what() << std::endl;
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
    int id = generate_id();  // Generar un ID único para el Publisher de temperatura
    TemperaturePublisher publisher(id, "test/temperature");

    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<double> distribution(23.0, 25.0);

    while (true) {
        double temperatura = distribution(generator);
        publisher.publicarTemperatura(temperatura);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
