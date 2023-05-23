#include <string>
#include <iostream>
#include <vector>
#include <hiredis/hiredis.h>
#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <pthread.h>
#include "shared.h"
#include "messages.h"
#include "Publisher.h"

using namespace std;

pthread_mutex_t pthread_mutex;
pthread_cond_t condition;
int respuesta_recibida = 0;

std::vector<Publisher> publisher_list;


void add_pub(string publisher_id){
    redisContext* redis = redisConnect("127.0.0.1", 6379); // Conectarse a Redis
    if (redis == NULL || redis->err) {
        std::cout << "Error al conectarse a Redis: " << redis->errstr << std::endl;
        return;
    }

    // Verificar hasta donde hay datos en la tabla "publisher"
    int i = 0;
    redisReply* ver = nullptr;
    std::string str_i;
    do {
        str_i = std::to_string(i);
        ver = (redisReply*)redisCommand(redis, "EXISTS publisher:%s", str_i.c_str());
        if (ver == nullptr) {
            std::cout << "Error al consultar Redis: " << redis->errstr << std::endl;
            redisFree(redis);
            return;
        }
        if (ver->integer == 0) {
            freeReplyObject(ver);
            break;
        }
        freeReplyObject(ver);
        i++;
    } while (true);

    redisReply* reply = (redisReply*)redisCommand(redis, "HSET publisher:%s Id %s", str_i.c_str(), publisher_id.c_str()); // Insertar el publicador en la tabla "publisher"
    freeReplyObject(reply);

    redisFree(redis); // Cerrar la conexión a Redis
}


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
    for(int i = 0; i < publisher_list.size();i++){
        std::cout << "Publisher " << publisher_list[i].getClientId() << std::endl;
    }
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

            zmq::message_t message;
            subscriber.recv(&message);
            std::string str_message = std::string(static_cast<char*>(message.data()), message.size());
            std::cout << "Received message: " << str_message << std::endl;

            // Busca la posición del ":"
            std::size_t posicion_guion = str_message.find(":");

            // Extrae la primera parte de la cadena
            std::string parte1 = str_message.substr(0, posicion_guion);

            // Extrae la segunda parte de la cadena (desde la posición siguiente al ":")
            std::string parte2 = str_message.substr(posicion_guion + 1);

            //Verificamos si el mensaje del publisher es 1 o 0
            //Si es 0 se guarda en el vector
            //Si el mensaje es 1 se guatrad en la bss y en el vector
            if(parte2 == "0"){
                //Se procede a guaradr en el vector
                std::cout << "Guarado en el vector\n";
            } else {
                //Se procede a guaradar en el vector y en la base de datos
                add_pub(parte1);
                std::cout << "Guarado en el vector y en la base de datos\n";
            }

            std::string message_subscribers = parte2;
            zmq::message_t zmq_message(message_subscribers.size());
            memcpy(zmq_message.data(), message_subscribers.data(), message_subscribers.size());
            publisher.send(zmq_message);
            usleep(1000000); // Esperar un segundo

        /*
        zmq::message_t topic;
        zmq::message_t content;
        bool received = subscriber.recv(topic, zmq::recv_flags::none) &&
                        subscriber.recv(content, zmq::recv_flags::none);
        if (received) {
            std::string topic_str(static_cast<char*>(topic.data()), topic.size());
            std::string content_str(static_cast<char*>(content.data()), content.size());

            std::istringstream iss(content_str);
            Publisher deserialized;
            iss >> deserialized;

            //El cliente quiere establecer una conexion
            if(topic_str == "CONNECT"){
                std::cout << "Se recibio una peticion CONNECT con las caracteristicas: " << content_str << std::endl;
                std::cout << "Publisher " << deserialized.getClientId() << std::endl;
                //Enviar CONNACK de vuelta y habilitar conexion (Nota: Actualmente por los mecanismos de ZeroMQ
                //Los publicadores no pueden recibir mensajes, es necesario cambiar los sockets a tipo reply

                publisher_list.push_back(deserialized);

                pthread_mutex_lock(&pthread_mutex);
                respuesta_recibida = 1;
                pthread_cond_signal(&condition);
                pthread_mutex_unlock(&pthread_mutex);

                publisher.send(zmq::str_buffer("status"), zmq::send_flags::none);
                publisher.send(zmq::str_buffer("Mensaje desde broker"));
                //pthread_exit(NULL);

            }

            std::cout << "Received message on topic \"" << topic_str << "\": " << content_str << std::endl;
            print_vector(publisher_list);
        } else {
            // handle receive errors
            std::cerr << "Error receiving message: " << zmq_strerror(errno) << std::endl;
        }
           */
    }

    //router.close();
    //context.close();
    //subscriber.close();
    //publisher.close();
    return NULL;
}
