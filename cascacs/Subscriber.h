#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

#include <zmq.hpp>
#include <string>
#include <iostream>
struct subscriber_args {
    zmq::context_t* context;
    std::string publisher_id;
};
void susbscribe_to_topic(zmq::socket_t &socket, const std::string &topic){
    socket.setsockopt(ZMQ_SUBSCRIBE, topic.c_str(), topic.length());
}


void* Subscriber_routine(void* arg)
{

    subscriber_args* args = static_cast<subscriber_args*>(arg);

    zmq::context_t* context = args->context;
    std::string subscriber_id = args->publisher_id;

    std::cout << "Subscriber iniciado " <<std::endl;
    zmq::socket_t socket(*context, ZMQ_SUB);
    socket.connect("tcp://localhost:5556");
    susbscribe_to_topic(socket,"sensor/bombillo");
    //socket.setsockopt(ZMQ_SUBSCRIBE, "", 0);


    while (true) {
        zmq::message_t message;
        socket.recv(&message);

        std::string message_str(static_cast<char*>(message.data()), message.size());
        std::cout << "Mensaje recibido: " << message_str << std::endl;
    }

    return NULL;
}
#endif // SUBSCRIBER_H
