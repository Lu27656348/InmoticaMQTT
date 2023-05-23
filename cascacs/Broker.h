#include <string>
#include <iostream>
#include <vector>

#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <pthread.h>
#include "shared.h"
#include "messages.h"
#include "Publisher.h"
#include "Redis.h"

pthread_mutex_t pthread_mutex;
pthread_cond_t condition;
int respuesta_recibida = 0;

std::vector<Publisher> publisher_list;

void* reader_thread(void*){
    //Debo cerrar la variable para mantener la consistencia
    pthread_mutex_lock(&pthread_mutex);
    std::cout << "Broker lee: " << shared_variable << std::endl;
    pthread_mutex_unlock(&pthread_mutex);
    //Termino con la variable y la libero
    return NULL;

}

void* print_vector(std::vector<Publisher> publisher_list){
    std::cout << "**PRINT** " << std::endl;
    std::cout << "-----------------------" << std::endl;
    if(publisher_list.size()<1){
         std::cout << "No hay publisher" << std::endl;

    }else{
         for(int i = 0; i < publisher_list.size();i++){
             std::cout << "Publisher " << publisher_list[i].getClientId() << " " << publisher_list[i].getPublishTopic() <<std::endl;
         }
    }
    std::cout << "-----------------------" << std::endl;

}

int topic_handler(std::string topic, std::string payload){
    /*
    switch(topic){
        case "PUBLISH":
        break;

    }
    */

}

 void* Broker(void* arg)
{
    zmq::context_t* context = static_cast<zmq::context_t*>(arg);

    std::cout << "Broker iniciado" << std::endl;
    std::cout << "Contexto de ZeroMQ: " << *context << std::endl;

    // construct a REQ (request) socket and connect to interface
    zmq::socket_t subscriber{*context, zmq::socket_type::sub};
    zmq::socket_t publisher{*context, zmq::socket_type::pub};

    subscriber.bind("tcp://*:5555");
    subscriber.setsockopt(ZMQ_SUBSCRIBE,"",0);

    publisher.bind("tcp://*:5556");

    //zmq::proxy(subscriber, publisher);

    while(true){

            zmq::message_t topic;
            zmq::message_t content;
            bool received = subscriber.recv(topic, zmq::recv_flags::none) &&
                            subscriber.recv(content, zmq::recv_flags::none);

            if (received) {

                std::string topic_str(static_cast<char*>(topic.data()), topic.size());
                std::string content_str(static_cast<char*>(content.data()), content.size());

                //El cliente quiere establecer una conexion
                if(topic_str == "CONNECT"){
                    std::cout << "Se recibio una peticion CONNECT con las caracteristicas: " << content_str << std::endl;

                    std::istringstream iss(content_str);
                    CONNECT deserialized;
                    iss >> deserialized;


                    if(deserialized.getCleanSession() == 0){
                        std::cout << "Redis -> " << std::endl;
                        add_pub(deserialized.getClientId());
                        list_publishers();
                    }



                    //Enviar CONNACK de vuelta y habilitar conexion (Nota: Actualmente por los mecanismos de ZeroMQ
                    //Los publicadores no pueden recibir mensajes, es necesario cambiar los sockets a tipo reply

                    /*
                    auto iter = std::find_if(publisher_list.begin(), publisher_list.end(), [&](const CONNECT& obj) {
                        return obj.getClientId() == deserialized.getClientId();
                    });

                    if (iter != publisher_list.end()) {
                        std::cout << "Objeto encontrado: " << iter->clientId << std::endl;
                    } else {
                        std::cout << "Objeto no encontrado." << std::endl;
                    }
*/
                    //publisher_list.push_back(deserialized.clientId);

                    pthread_mutex_lock(&pthread_mutex);
                    respuesta_recibida = 1;
                    pthread_cond_signal(&condition);
                    pthread_mutex_unlock(&pthread_mutex);


                    //pthread_exit(NULL);

                }else if (topic_str == "PUBLISH"){

                    std::istringstream iss(content_str);
                    Publisher deserialized;
                    iss >> deserialized;

                    publisher_list.push_back(deserialized);
                    std::cout << "Se ha hecho una publicacion " << std::endl;
                    publisher.send(zmq::str_buffer("status"), zmq::send_flags::none);
                    publisher.send(zmq::str_buffer("Mensaje desde broker"));
                }

                std::cout << "Received message on topic \"" << topic_str << "\": " << content_str << std::endl;

            } else {
                // handle receive errors
                std::cerr << "Error receiving message: " << zmq_strerror(errno) << std::endl;
            }
            print_vector(publisher_list);
             /*
            std::string message_subscribers = ": Hello, World!";
            zmq::message_t zmq_message(message_subscribers.size());
            memcpy(zmq_message.data(), message_subscribers.data(), message_subscribers.size());
            publisher.send(zmq_message);
            usleep(1000000); // Esperar un segundo
            */
    }

    //router.close();
    //context.close();
    //subscriber.close();
    //publisher.close();
    return NULL;
}
