#include <string>
#include <iostream>

#include <zmq.hpp>
#include <zmq_addon.hpp>

int Broker()
{
    // initialize the zmq context with a single IO thread
    zmq::context_t context{1};

    // construct a REQ (request) socket and connect to interface
    zmq::socket_t subscriber{context, zmq::socket_type::sub};
    zmq::socket_t publisher{context, zmq::socket_type::pub};
    subscriber.connect("tcp://localhost:5555");
    publisher.bind("tcp://*:5556");

    // set topics you want to sub
    subscriber.setsockopt(ZMQ_SUBSCRIBE,"",0);

    while(true){
        // receive a message
        zmq::message_t topic;
        zmq::message_t content;
        bool received = subscriber.recv(topic, zmq::recv_flags::none) &&
                        subscriber.recv(content, zmq::recv_flags::none);
        if (received) {
            std::string topic_str(static_cast<char*>(topic.data()), topic.size());
            std::string content_str(static_cast<char*>(content.data()), content.size());
            if(topic_str == "status"){
                //Buscar todos los subscriptores asociados
                //al topico y mandar el contenido
                publisher.send(zmq::str_buffer("status"), zmq::send_flags::none);
                publisher.send(zmq::str_buffer("Mensaje desde broker"));
            }
            std::cout << "Received message on topic \"" << topic_str << "\": " << content_str << std::endl;
        } else {
            // handle receive errors
            std::cerr << "Error receiving message: " << zmq_strerror(errno) << std::endl;
        }
    }

    return 0;
}
