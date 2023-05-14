#include <iostream>
#include <string>
#include <zmq.hpp>

using namespace std;

int main() {
    zmq::context_t context(1);

    // Crear un socket de suscriptor
    zmq::socket_t subscriber(context, zmq::socket_type::sub);
    std::cout << "Conectando al servidor..." << std::endl;
    subscriber.connect("tcp://127.0.0.1:5557");

    // Suscribirse al topic "Bombillo"
    subscriber.setsockopt(ZMQ_SUBSCRIBE, "Bombillo", strlen("Bombillo"));

    std::cout << "Suscriptor MQTT iniciado." << std::endl;

    while (true) {
        zmq::message_t message;
        subscriber.recv(message, zmq::recv_flags::none);

        // Convertir el mensaje a una cadena de texto
        std::string payload(static_cast<char*>(message.data()), message.size());

        std::cout << "Mensaje recibido: " << payload << std::endl;
    }

    return 0;
}