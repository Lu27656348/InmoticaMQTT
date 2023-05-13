#include <iostream>
#include <string>
#include <zmq.hpp>

int main() {
    int major, minor, patch;
    zmq_version(&major,&minor,&patch);
    std::cout << "ZeroMQ version: " << major << " . " << minor << " . " << patch << std::endl;
    // Crea un contexto ZeroMQ
    zmq::context_t context(1);

    // Crea un socket de tipo PUB que envía mensajes
    zmq::socket_t socket(context, zmq::socket_type::req);

    // Configura el socket para permitir conexiones entrantes
    int hwm = 0;
    socket.setsockopt(ZMQ_SNDHWM, &hwm, sizeof(hwm));

    // Conéctate al socket en la dirección "tcp://localhost:5555"
    std::cout << "Conectando al servidor..." << std::endl;
    socket.connect("tcp://localhost:5556");

    // Define el mensaje a enviar al broker
    std::string mensaje = "Hola mundo";
    std::cout << mensaje << std::endl;
    // Envía el mensaje
    //zmq::message_t zmq_message(mensaje.size());
    zmq::message_t zmq_message(mensaje.data(), mensaje.size());
    std::cout << mensaje << std::endl;
    try {
        socket.send(zmq_message, zmq::send_flags::none);
        std::cout << "Mensaje enviado: " << mensaje << std::endl;
    } catch (zmq::error_t& ex) {
        std::cerr << "Error al enviar el mensaje: " << ex.what() << std::endl;
        return 1;
    }

    //Esperando la respuesta del servidor
    zmq::message_t respuesta;
    
    try{  
        socket.recv(&respuesta,0);
        std::string respuesta_str(static_cast<char*>(respuesta.data()), respuesta.size());
        std::cout << "Respuesta recibida: " << respuesta_str << std::endl;
        printf("Enviando respuesta\n");
    }catch (zmq::error_t& ex){
        std::cerr << "Error al recibir la respuesta: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}