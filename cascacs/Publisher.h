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

int generate_id_() {
        // Obtener el tiempo actual en segundos
        time_t tiempoActual = time(nullptr);

        // Convertir el tiempo a una cadena de caracteres
        std::string tiempoActualStr = std::to_string(tiempoActual);

        // Generar un hash único para la cadena de tiempo
        size_t hashedTiempoActual = std::hash<std::string>{}(tiempoActualStr);

        // Mostrar solo la sección del ID que se desea
        int id = hashedTiempoActual % 1000000;

        std::cout << "ID generado -> " << id << std::endl;

        return id;
}

void* sendMesage_(zmq::socket_t &socket,std::string topic, std::string payload){
        zmq::message_t topic_msg(topic.size());
        memcpy(topic_msg.data(), topic.data(), topic.size());

        zmq::message_t payload_msg(payload.size());
        memcpy(payload_msg.data(), payload.c_str(), payload.size());

        socket.send(topic_msg, zmq::send_flags::sndmore);
        socket.send(payload_msg, zmq::send_flags::none);
}

void* writer_thread_(void*){
    //Debo cerrar la variable para mantener la consistencia
    pthread_mutex_lock(&pthread_mutex);
    std::cout << "Publisher escribiendo" << shared_variable << std::endl;
    shared_variable = shared_variable + 1;
    std::cout << "Publisher escribiendo" << shared_variable << std::endl;
    pthread_mutex_unlock(&pthread_mutex);
    //Termino con la variable y la libero
    return NULL;
}


#endif // PUBLISHER_H
