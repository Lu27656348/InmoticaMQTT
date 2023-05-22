#ifndef PUBLISHER_H
#define PUBLISHER_H
#include <string>
#include <iostream>
#include <unistd.h>
#include <pthread.h>

#include <zmq.hpp>
#include "shared.h"

class Publisher {
private:
    std::string client_id;
    std::string publish_topic;
    int keep_alive_interval;
    std::string port;
    std::string broker_address;
    std::string username;
    std::string password;

public:
    Publisher(){
        client_id = "0";
        publish_topic = "$SYS";
        keep_alive_interval = 60;
        broker_address = "localhost";
        port = "5555";
    }
    Publisher(std::string client_id, std::string publish_topic, int keep_alive_interval)
        : client_id(client_id),
        publish_topic(publish_topic),
        keep_alive_interval(keep_alive_interval){}
    void hello_world(){
        std::cout << "Hello world" << std::endl;
    }
    int generate_id() {
        // Obtener el tiempo actual en segundos
        time_t tiempoActual = time(nullptr);

        // Convertir el tiempo a una cadena de caracteres
        std::string tiempoActualStr = std::to_string(tiempoActual);

        // Generar un hash único para la cadena de tiempo
        size_t hashedTiempoActual = std::hash<std::string>{}(tiempoActualStr);

        // Mostrar solo la sección del ID que se desea
        int id = hashedTiempoActual % 1000000;

        return id;
    }
    std::string getClientId(){
        return client_id;
    }
    void setClientId(std::string client_id_){
        client_id = client_id_;
    }
    std::string getPublishTopic(){
        return publish_topic;
    }
    void setPublishTopic(std::string publish_topic_){
        publish_topic = publish_topic_;
    }
    // Operador de inserción <<
    friend std::ostream& operator<<(std::ostream& os, const Publisher& publisher) {
        os << publisher.client_id << " " << publisher.publish_topic << ", " << publisher.keep_alive_interval;
        return os;
    }

    // Operador de extracción >>
    friend std::istream& operator>>(std::istream& is, Publisher& publisher) {
        is >> publisher.client_id >> publisher.publish_topic >> publisher.keep_alive_interval;
        return is;
    }

};

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

static void* Publisher_routine(void* arg)
{
    // initialize the zmq context with a single IO thread
    std::cout << "Publisher iniciado" << std::endl;
    zmq::context_t context{1};

    // construct a PUB (request) socket and connect to interface
    zmq::socket_t socket{context, zmq::socket_type::pub};
    socket.bind("tcp://*:5555");

    //Creamos un nuevo publisher

    Publisher publisher("1","PUBLISH",60);
    std::cout << "Se ha generado -> " << publisher.getClientId() << std::endl;



    while(true){
        //writer_thread(NULL);
        //  Write three messages, each with an envelope and content
        std::string topic = publisher.getPublishTopic();
        std::ostringstream oss;
        oss << publisher;
        std::string payload = oss.str();

        zmq::message_t topic_msg(topic.size());
        memcpy(topic_msg.data(), topic.data(), topic.size());

        zmq::message_t payload_msg(payload.size());
        memcpy(payload_msg.data(), payload.c_str(), payload.size());

        socket.send(topic_msg, zmq::send_flags::sndmore);
        socket.send(payload_msg, zmq::send_flags::none);

        sleep(1);
    }

    socket.close();
    return NULL;

}

#endif // PUBLISHER_H
