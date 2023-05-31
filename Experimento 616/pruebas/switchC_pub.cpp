///NOTA:
//sudo apt-get install libmqttclient-dev  ---> Instalador
//Primero, en experimento 616 hacer en el terminal: cd PubSub ---> direccion
//g++ switchC_pub.cpp -lpaho-mqttpp3 -lpaho-mqtt3a -o sc_pub --> correr

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <mqtt/async_client.h>
#include <string>

class SwitchPublisher {
private:
    int id;
    std::string topic;
    mqtt::async_client client;
    mqtt::connect_options conn_opts;

public:
    SwitchPublisher(int id, const std::string& topic) : id(id), topic(topic), client("tcp://localhost:1883", "Publisher") {
        conn_opts.set_clean_session(true);

        try {
            mqtt::token_ptr conntok = client.connect(conn_opts);
            std::cerr << "Se ingresó al broker con exito" << std::endl;
            conntok->wait();
        }
        catch (const mqtt::exception& exc) {
            std::cerr << "Error al conectar al broker MQTT: " << exc.what() << std::endl;
            std::exit(-1);
        }
    }

    void enviarMensaje(const std::string& mensaje) {
        mqtt::message_ptr pubmsg = mqtt::make_message(topic, mensaje);

        try {
            mqtt::token_ptr pubtok = client.publish(pubmsg);
            std::cerr << "Mensaje Enviado con exito" << std::endl;
            pubtok->wait();
        }
        catch (const mqtt::exception& exc) {
            std::cerr << "Error al publicar el mensaje: " << exc.what() << std::endl;
        }
    }

    ~SwitchPublisher() {
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

int main(int argc, char* argv[]) {
    int id = generate_id();

    std::cout << "¿Desea prender o pagara los Bombillos?\n";
    std::cout << "Escriba [ON] (en mayusculas) para encender\n";
    std::cout << "Escriba [OFF] (en mayusculas) para apagar\n";
    std::cout << "En caso de escribir otra cosa, no habrá accion en el bombillo\n";
    std::string msg;
    std::cin >> msg;

    //Manda mensaje al Bombillo A
    SwitchPublisher publisher(id, "test/bombillo/1/");
    publisher.enviarMensaje(msg);

    //Manda mensaje al Bombillo B
    SwitchPublisher publisher2(id, "test/bombillo/2/");
    publisher2.enviarMensaje(msg);

    return 0;
}
