#include <iostream>
#include <string>
#include <zmq.hpp>
#include <map>
#include <unordered_map>
#include <memory>

using namespace std;

map<string, shared_ptr<zmq::socket_t>> subscribers;

unordered_map<string, string> Cliente;

//Función que recibe el mensaje del cliente
void handle_message(zmq::socket_t& socket) {
    // Recibe el mensaje del cliente
    zmq::message_t request;
    socket.recv(request, zmq::recv_flags::none);

    // Convierte el mensaje a una cadena
    std::string msg(static_cast<char*>(request.data()), request.size());
    std::cout << "Mensaje recibido: " << msg << std::endl;

    //Separar el mensaje recibido en dos cadenas ()
    size_t posicion = msg.find_first_of('-');

    //Id Cliente
    std::string id = msg.substr(0, posicion);

    //Mensaje recibido
    std::string message = msg.substr(posicion + 1);

    //agregar al map de conexion de clientes al broker

    Cliente.emplace(id,message);

    //mostrar Clientes del map
    for (auto& i : Cliente) {
    cout << "Id: " << i.first << ", Mensaje: " << i.second << endl;
    }

    // Separa el tema y el mensaje del mensaje recibido
    std::string topic = "Bombillo";

    std::string payload = message;

    // Publica el mensaje en los sockets de los suscriptores
    if (subscribers.find(topic) != subscribers.end()) {
        zmq::message_t response(payload.size());
        memcpy(response.data(), payload.data(), payload.size());

        for (const auto& subscriber : subscribers) {
            try {
                if (subscriber.second) {
                    subscriber.second->send(response, zmq::send_flags::none);
                    std::cout << "Mensaje ZMQ enviado a suscriptor: " << payload << std::endl;
                } else {
                    std::cerr << "Error: socket nulo para el tema " << topic << std::endl;
                }
            } catch (zmq::error_t& ex) {
                std::cerr << "Error al enviar el mensaje: " << ex.what() << std::endl;
            }
        }
    } 

    // Enviar la respuesta al cliente
    zmq::message_t reply(5);
    memcpy(reply.data(), "OK", 2);
    socket.send(reply, zmq::send_flags::none);
}

//Funcion Main()
int main() {
    int major, minor, patch;
    zmq_version(&major,&minor,&patch);
    std::cout << "ZeroMQ version: " << major << " . " << minor << " . " << patch << std::endl;

    zmq::context_t context(1);

    // Socket REP para recibir mensajes del cliente
    zmq::socket_t receiver(context, zmq::socket_type::rep);
    receiver.bind("tcp://*:5556");

    // Socket PUB para publicar mensajes en los temas suscritos por los clientes
    //zmq::socket_t publisher(context, zmq::socket_type::pub);
    //publisher.bind("tcp://*:5557");

    // Crear un socket de publicación para el tema "Bombillo"
    auto socket_ptr = std::make_shared<zmq::socket_t>(context, zmq::socket_type::pub);
    socket_ptr->bind("tcp://*:5557");
    subscribers.insert(make_pair("Bombillo", std::move(socket_ptr)));

    std::cout << "Broker MQTT iniciado." << std::endl;

    while (true) {
        zmq::pollitem_t items[] = {
            {receiver, 0, ZMQ_POLLIN, 0},
        };

        zmq::poll(items, 1, -1);

        if (items[0].revents & ZMQ_POLLIN) {
            handle_message(receiver);
        }
    }

    return 0;
}
