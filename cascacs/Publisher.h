#ifndef PUBLISHER_H
#define PUBLISHER_H
#include <string>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include <zmq.hpp>
#include "shared.h"
#include "messages.h"
#include "shared.h"
struct publisher_args {
    zmq::context_t* context;
    std::string publisher_id;
};


class Lightbul {
    private:
        std::string status;
    public:
        Lightbul(std::string status) : status(status){}
        std::string getStatus(){
            return status;
        }
        friend std::ostream& operator<<(std::ostream& os, const Lightbul& lightbul) {
            os << lightbul.status;
            return os;
        }

        // Operador de extracción >>
        friend std::istream& operator>>(std::istream& is, Lightbul& lightbul) {
            is >> lightbul.status;
            return is;
        }

};
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
void* sendMesage(zmq::socket_t &socket,std::string topic, std::string payload){
        zmq::message_t topic_msg(topic.size());
        memcpy(topic_msg.data(), topic.data(), topic.size());

        zmq::message_t payload_msg(payload.size());
        memcpy(payload_msg.data(), payload.c_str(), payload.size());

        socket.send(topic_msg, zmq::send_flags::sndmore);
        socket.send(payload_msg, zmq::send_flags::none);
}
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

    bool sendConnect(bool cleanSession, zmq::socket_t &socket){
        std::cout << "Enviando CONNECT" << std::endl;
        CONNECT connect(client_id,cleanSession,keep_alive_interval);

        std::string topic = "CONNECT";

        std::ostringstream oss;
        oss << connect;
        std::string payload = oss.str();

        zmq::message_t topic_msg(topic.size());
        memcpy(topic_msg.data(), topic.data(), topic.size());

        zmq::message_t payload_msg(payload.size());
        memcpy(payload_msg.data(), payload.c_str(), payload.size());

        socket.send(topic_msg, zmq::send_flags::sndmore);
        socket.send(payload_msg, zmq::send_flags::none);

        struct timespec currentTime;
        struct timespec limitTime;

        clock_gettime(CLOCK_REALTIME,&currentTime);
        limitTime.tv_sec = currentTime.tv_sec + 1;
        limitTime.tv_nsec = currentTime.tv_nsec;

        pthread_mutex_lock(&pthread_mutex);
        std::cout << "respuesta_recibida: " << respuesta_recibida <<std::endl;
        while(!respuesta_recibida){
            int resultado = pthread_cond_timedwait(&condition,&pthread_mutex,&limitTime);
            if(resultado == ETIMEDOUT){
                std::cout << "El broker no respondio en el keepAlive" << std::endl;
                pthread_mutex_unlock(&pthread_mutex);
                return false;
            }
        }

        respuesta_recibida = 0;
        pthread_mutex_unlock(&pthread_mutex);
        std::cout << "Publisher conectado al broker" << std::endl;
        pthread_exit(NULL);
        return true;

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
        os << publisher.client_id << " " << publisher.publish_topic << " " << publisher.keep_alive_interval;
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

void* Publisher_routine(void* arg)
{

    publisher_args* args = static_cast<publisher_args*>(arg);

    zmq::context_t* context = args->context;
    std::string publisher_id = args->publisher_id;

    std::cout << "Publisher iniciado " <<std::endl;
    zmq::socket_t socket(*context, ZMQ_PUB);

    socket.connect("tcp://localhost:5555");

    while (true) {
        std::string message = publisher_id + ": Hello, World!";
        zmq::message_t zmq_message(message.size());
        memcpy(zmq_message.data(), message.data(), message.size());
        socket.send(zmq_message);
        usleep(1000000); // Esperar un segundo
    }

    return NULL;


    //Creamos un publisher de prueba
    Publisher publisher("1","bombillo",60);

    bool publisherConnected = false;

    while(true){
        std::string message = "Mensaje " + std::to_string(generate_id());
        zmq::message_t zmq_message(message.size());
        memcpy(zmq_message.data(), message.c_str(), message.size());

        std::string topic = "CONNECT";
        zmq::message_t topic_msg(topic.size());
        memcpy(topic_msg.data(), topic.data(), topic.size());

        zmq::message_t payload_msg(message.size());
        memcpy(payload_msg.data(), message.c_str(), message.size());

        socket.send(topic_msg, zmq::send_flags::sndmore);
        socket.send(payload_msg, zmq::send_flags::none);
        //std::cout << "Publicador envió mensaje: " << message << std::endl;
        sleep(1);
        //writer_thread(NULL);
        /*
        if(!publisherConnected){
            if(publisher.sendConnect(true,socket)){
                publisherConnected = true;
                std::cout << "publisherConnected -> " << publisherConnected << std::endl;
            }
        }else{
            Lightbul lightbul("ON");
            std::cout << "El publisher esta conectado -> " << std::endl;
            std::cout << "topico -> " << publisher.getPublishTopic()<< std::endl;

            std::ostringstream oss;
            oss << lightbul;
            std::string payload = oss.str();

            sendMesage(socket,publisher.getPublishTopic(),payload);
            sleep(1);

        }
        */

    }
    //context.close();
    socket.close();
    return NULL;

}


#endif // PUBLISHER_H
