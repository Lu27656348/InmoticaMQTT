#include <iostream>
#include <string>
#include <zmq.hpp>

int main() {

    // Crea un contexto ZeroMQ
    zmq::context_t context(1);

    // Crea un socket de tipo PUB que envía mensajes
    zmq::socket_t socket(context, zmq::socket_type::pub);

    // Configura el socket para permitir conexiones entrantes
    int hwm = 0;
    socket.setsockopt(ZMQ_SNDHWM, &hwm, sizeof(hwm));

    // Conéctate al socket en la dirección "tcp://localhost:5555"
    std::cout << "Conectando al servidor..." << std::endl;
    socket.connect("tcp://localhost:5555");

    // Define el mensaje a enviar al broker
    std::string mensaje = "Hola mundo";
    
    // Envía el mensaje
    zmq::message_t zmq_message(mensaje.size());
    memcpy(zmq_message.data(), mensaje.c_str(), mensaje.size());

    try {
        socket.send(zmq_message, 0);
        std::cout << "Mensaje enviado: " << mensaje << std::endl;
    } catch (zmq::error_t& ex) {
        std::cerr << "Error al enviar el mensaje: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}