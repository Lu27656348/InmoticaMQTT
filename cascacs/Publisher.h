#ifndef PUBLISHER_H
#define PUBLISHER_H
#include <string>
#include <iostream>
#include <unistd.h>
#include <pthread.h>

#include <zmq.hpp>
#include "shared.h"

void* writer_thread(void*){
    //Debo cerrar la variable para mantener la consistencia
    pthread_mutex_lock(&pthread_mutex);
    std::cout << "Publisher escribiendo" << shared_variable << std::endl;
    shared_variable = shared_variable + 1;
    std::cout << "Publisher escribiendo" << shared_variable << std::endl;
    pthread_mutex_unlock(&pthread_mutex);
    //Termino con la variable y la libero
    return NULL;
}
static void* Publisher(void* client_id)
{
    // initialize the zmq context with a single IO thread
    std::cout << "Publisher iniciado" << std::endl;
    zmq::context_t context{1};

    // construct a PUB (request) socket and connect to interface
    zmq::socket_t socket{context, zmq::socket_type::pub};
    socket.bind("tcp://*:5555");

    while(true){
        writer_thread(NULL);
        //  Write three messages, each with an envelope and content
        socket.send(zmq::str_buffer("topico/bombillo"), zmq::send_flags::none);
        socket.send(zmq::str_buffer("ON"));
        sleep(1);
    }

    socket.close();
    return NULL;

}

#endif // PUBLISHER_H
