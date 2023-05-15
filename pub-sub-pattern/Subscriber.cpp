#include <string>
#include <iostream>

#include <zmq.hpp>
#include <zmq_addon.hpp>

int main()
{
    // initialize the zmq context with a single IO thread
    zmq::context_t context{1};

    // construct a REQ (request) socket and connect to interface
    zmq::socket_t subscriber{context, zmq::socket_type::sub};
    subscriber.connect("tcp://localhost:5556");

 // set topics you want to sub
    subscriber.setsockopt(ZMQ_SUBSCRIBE,"status",strlen("status"));

    while(true){
        // receive a message
        zmq::message_t topic;
        zmq::message_t content;
        bool received = subscriber.recv(topic, zmq::recv_flags::none) &&
                        subscriber.recv(content, zmq::recv_flags::none);
        if (received) {
            std::string topic_str(static_cast<char*>(topic.data()), topic.size());
            std::string content_str(static_cast<char*>(content.data()), content.size());
            std::cout << "Received message on topic \"" << topic_str << "\": " << content_str << std::endl;
        } else {
            // handle receive errors
            std::cerr << "Error receiving message: " << zmq_strerror(errno) << std::endl;
        }
    }

    return 0;
}
