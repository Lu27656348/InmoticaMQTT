#ifndef PUBLISHER_CLASS_H
#define PUBLISHER_CLASS_H

#include <zmq.hpp>
#include <iostream>
#include <string>
#include <pthread.h>
#include "messages.h"
#include "shared.h"

class Publisher {
private:
    std::string client_id;
    std::string publish_topic;
    int keep_alive_interval;
    bool connected;
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
        keep_alive_interval(keep_alive_interval),
        connected(false){}
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
        std::cout << "Publisher " << client_id << " conectado al broker" << std::endl;
        connected = true;
        //pthread_exit(NULL);
        return true;

    }

    std::string getClientId(){
        return client_id;
    }
    bool getConnected(){
        return connected;
    }
    void setConnected(bool connected_){
        connected = connected_;
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
#endif // PUBLISHER_CLASS_H
