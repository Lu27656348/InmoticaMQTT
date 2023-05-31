#include <iostream>
#include <cstring>
#include <cstdlib>
#include <mqtt/async_client.h>
#include <chrono>
#include <thread>

//g++ -o indicador indicador_sub.cpp -lpaho-mqttpp3 -lpaho-mqtt3as -lpthread

#define ADDRESS "tcp://localhost:1883"
#define CLIENTID "TemperatureSubscriber"
#define QOS 0

class TemperatureSubscriber : public virtual mqtt::callback {
private:
    int id;
    std::string topic;

    mqtt::async_client client;
    mqtt::connect_options conn_opts;

public:
    TemperatureSubscriber(int id, const std::string& topic) : id(id), topic(topic), client(ADDRESS, CLIENTID) {
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

        // Convertir el payload a temperatura en Celsius
        double temperaturaCelsius = std::stod(payload);

        // Convertir la temperatura de Celsius a Fahrenheit
        double temperaturaFahrenheit = (temperaturaCelsius * 9 / 5) + 32;

        std::cout << "ID: " << id << std::endl;
        std::cout << "Temperatura (Celsius): " << temperaturaCelsius << std::endl;
        std::cout << "Temperatura (Fahrenheit): " << temperaturaFahrenheit << std::endl;
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
    int id = generate_id();  // Generar un ID único para el Subscriber
    TemperatureSubscriber subscriber(id, "test/temperature");

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    return 0;
}
