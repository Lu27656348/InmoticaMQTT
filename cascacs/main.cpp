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

int shared_variable = 0;
pthread_mutex_t pthread_mutex;

int main(void)
{
    std::vector<Publisher> Publishers;
    pthread_mutex_init(&pthread_mutex, NULL); //Espacio donde se comparten datos del mutex

    std::vector<std::thread> threads; //Vector que almacena todos los procesos clientes (publicadores y suscriptores)
    pthread_t broker_thread;
    pthread_t publisher_thread;

    pthread_create(&broker_thread,NULL,&Broker,NULL);
    pthread_create(&publisher_thread,NULL,&Publisher_routine,NULL);

    pthread_join(broker_thread,NULL);
    pthread_join(publisher_thread,NULL);

    pthread_mutex_destroy(&pthread_mutex);


}
