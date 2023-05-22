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

using namespace std;

int shared_variable = 0;
pthread_mutex_t pthread_mutex;

int main(void)
{

    pthread_mutex_init(&pthread_mutex, NULL); //Espacio donde se comparten datos del mutex

    std::vector<std::thread> threads; //Vector que almacena todos los procesos clientes (publicadores y suscriptores)
    pthread_t broker_thread;
    pthread_t client_thread;
    pthread_create(&broker_thread,NULL,&Broker,NULL);
    pthread_create(&client_thread,NULL,&Publisher,NULL);

    pthread_join(broker_thread,NULL);
    pthread_join(client_thread,NULL);

    pthread_mutex_destroy(&pthread_mutex);


}
