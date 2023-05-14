#include <iostream>
#include <zmq.hpp>

int main(){
    zmq::context_t context(1);
    zmq::socket_t receiver(context,ZMQ_SUB);

    receiver.connect("tcp://localhost:5556");

    //Inserte codigo de suscripcion
    receiver.setsockopt(ZMQ_SUBSCRIBE,"",0);

    while(true){
        zmq::message_t message;
        receiver.recv(&message);
        std::cout << "Mensaje recibido: " << std::string(static_cast<char*>(message.data()),message.size()) << std::endl;
    }
    return 0;
}