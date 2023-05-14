#include <iostream>
#include <string>
#include <zmq.hpp>
#include <unistd.h>

int main(){
    zmq::context_t context(1);
    zmq::socket_t publisher(context, ZMQ_PUB);

    publisher.bind("tcp://localhost:5556");

    int count = 0;
    while(true){
        std::string message = "Publicador -> " + std::to_string(count);

        zmq::message_t zmq_message(message.size());
        memcpy(zmq_message.data(), message.c_str(), message.size());

        publisher.send(zmq_message,zmq::send_flags::none);

        count++;
        sleep(1);
    }
    return 0;
}