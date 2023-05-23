#include "mainwindow.h"
#include "zmq.h"
#include "zmq.hpp"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <iostream>
#include "Broker.h"
#include "Publisher.h"
#include <thread>
#include <pthread.h>
#include <mutex> //Libreria para crear objetos compartidos entre hilos
#include <condition_variable> //Indica cambio entre hilos para compartir variable
#include "shared.h"
#include <vector>

using namespace std;
void *broker_thread(void *) {
    zmq::context_t context(1);
    zmq::socket_t frontend(context, ZMQ_SUB);
    frontend.bind("tcp://*:5555");
    frontend.setsockopt(ZMQ_SUBSCRIBE, "", 0);

    while (true) {
        zmq::message_t message;
        frontend.recv(&message);
        std::string str_message = std::string(static_cast<char*>(message.data()), message.size());
        std::cout << "Received message: " << str_message << std::endl;
    }

    return NULL;
}
int shared_variable = 0;
//pthread_mutex_t pthread_mutex;
// Función del hilo del Publicador
void *publisher_thread(void *args) {
    std::cout << "Iniciando hilo de publisher" << std::endl;
    publisher_args *pa = (publisher_args *)args;

    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_PUB);
    socket.connect("tcp://localhost:5555");

    while (true) {
        std::string message = pa->name + ": Hello, World!";
        zmq::message_t zmq_message(message.size());
        memcpy(zmq_message.data(), message.data(), message.size());
        socket.send(zmq_message);
        sleep(1);
    }
}


int main(void)
{

    sched_param params;
    int priority = sched_get_priority_max(SCHED_FIFO);
    params.sched_priority = priority;

    std::vector<Publisher> Publishers;
    Publisher publisher();

    pthread_mutex_init(&pthread_mutex, NULL); //Espacio donde se comparten datos del mutex

    //std::vector<pthread_t> threads; //Vector que almacena todos los procesos clientes (publicadores y suscriptores)

    pthread_t broker;
    pthread_t thread;
    pthread_t thread2;
    pthread_t thread3;
    pthread_t thread4;

    zmq::context_t context{5};

    pthread_create(&broker,NULL,broker_thread,NULL);

    //Establece la prioridad del hilo de alta prioridad
    pthread_setschedparam(broker, SCHED_FIFO, &params);

    publisher_args pa1 = {"publisher1"};
    publisher_args pa2 = {"publisher2"};
    publisher_args pa3 = {"publisher3"};

    pthread_create(&thread,NULL,publisher_thread,&pa1);
    pthread_create(&thread2,NULL,publisher_thread,&pa2);

    pthread_join(broker,NULL);
    pthread_join(thread,NULL);
    pthread_join(thread2,NULL);



    pthread_mutex_destroy(&pthread_mutex);


}
