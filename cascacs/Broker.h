#include <string>
#include <iostream>

#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <pthread.h>
#include "shared.h"
#include "messages.h"
#include "Publisher.h"

void* reader_thread(void*){
    //Debo cerrar la variable para mantener la consistencia
    pthread_mutex_lock(&pthread_mutex);
    std::cout << "Broker lee: " << shared_variable << std::endl;
    pthread_mutex_unlock(&pthread_mutex);
    //Termino con la variable y la libero
    return NULL;

}

int topic_handler(std::string topic, std::string payload){
    /*
    switch(topic){
        case "PUBLISH":
        break;

    }
    */

}

static void* Broker(void* nullarg)
{
    std::cout << "Broker iniciado" << std::endl;
    zmq::context_t context{1};
    // construct a REQ (request) socket and connect to interface
    zmq::socket_t subscriber{context, zmq::socket_type::sub};
    zmq::socket_t publisher{context, zmq::socket_type::pub};

    subscriber.connect("tcp://localhost:5555");
    publisher.bind("tcp://*:5556");

    // set topics you want to sub
    subscriber.setsockopt(ZMQ_SUBSCRIBE,"",0);

    while(true){
        //Lee variable compartida
        //reader_thread(NULL);
        // receive a message
        zmq::message_t topic;
        zmq::message_t content;
        bool received = subscriber.recv(topic, zmq::recv_flags::none) &&
                        subscriber.recv(content, zmq::recv_flags::none);
        if (received) {
            std::string topic_str(static_cast<char*>(topic.data()), topic.size());
            std::string content_str(static_cast<char*>(content.data()), content.size());

            if(topic_str == "PUBLISH"){
                std::istringstream iss(content_str);
                Publisher deserialized;
                iss >> deserialized;
                std::cout << "Objeto deserializado: " << deserialized.getPublishTopic() << std::endl;
                //Buscar todos los subscriptores asociados
                //al topico y mandar el contenido
                /*
                publisher.send(zmq::str_buffer("status"), zmq::send_flags::none);
                publisher.send(zmq::str_buffer("Mensaje desde broker"));
                */
            }

            std::cout << "Received message on topic \"" << topic_str << "\": " << content_str << std::endl;
        } else {
            // handle receive errors
            std::cerr << "Error receiving message: " << zmq_strerror(errno) << std::endl;
        }
    }
    subscriber.close();
    publisher.close();
    return NULL;
}
