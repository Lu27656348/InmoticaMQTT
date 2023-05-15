#include <string>
#include <iostream>
#include <unistd.h>

#include <zmq.hpp>

int main()
{
    // initialize the zmq context with a single IO thread
    zmq::context_t context{1};

    // construct a PUB (request) socket and connect to interface
    zmq::socket_t socket{context, zmq::socket_type::pub};
    socket.bind("tcp://*:5555");
    int count = 0;
    while(true){
        //  Write three messages, each with an envelope and content
        socket.send(zmq::str_buffer("status"), zmq::send_flags::none);
        socket.send(zmq::str_buffer("Message in status"));
        sleep(1);
    }

}
