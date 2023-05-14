#include <iostream>
#include <string>
#include <zmq.hpp>
#include <functional>
#include <ctime>

using namespace std;

// Función para generar un ID único
int generarIDUnico() {
  // Obtener el tiempo actual en segundos
  time_t tiempoActual = time(nullptr);

  // Convertir el tiempo a una cadena de caracteres
  string tiempoActualStr = to_string(tiempoActual);

  // Generar un hash único para la cadena de tiempo
  size_t hashedTiempoActual = hash<string>{}(tiempoActualStr);

  // Mostrar solo la sección del ID que se desea
  int id = hashedTiempoActual % 1000000;

  return id;
}

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

    //Crear el Hash unico para el cliente
    int id=generarIDUnico();

    //Convierto el Id a una cadena
    std::string id_str = to_string(id);

    // Define el mensaje a enviar al broker
    std::string msg = "\0";
    std::cout << "Ingresa mensaje al broker: " << std::endl;
    std::cin >> msg;
    std::cout << "\n";

    // Unimos id_str y msg en una sola cadena separada por un "-"
    std::string mensaje = id_str + "-" + msg;

    // Envía el mensaje
    zmq::message_t zmq_message(mensaje.data(), mensaje.size());

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
    }catch (zmq::error_t& ex){
        std::cerr << "Error al recibir la respuesta: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}