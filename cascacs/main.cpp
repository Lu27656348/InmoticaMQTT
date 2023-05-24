#include "mainwindow.h"
#include "zmq.h"
#include "zmq.hpp"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <iostream>
#include "Broker.h"
#include "Publisher.h"
#include "Subscriber.h"
#include <thread>
#include <pthread.h>
#include <mutex> //Libreria para crear objetos compartidos entre hilos
#include <condition_variable> //Indica cambio entre hilos para compartir variable
#include "shared.h"
#include <vector>
#include "publisher_routine.h"

#include <QCoreApplication>
#include <QTextStream>
/*
struct broker_args {
    zmq::context_t* context;
    QVector<Publisher> publisher_list;
};
*/
/*
struct publisher_args {
    zmq::context_t* context;
    std::string publisher_id;
};
*/
using namespace std;
int shared_variable = 0;
//pthread_mutex_t pthread_mutex;
// main.cpp
int main(int argc, char *argv[])
{
    //Configuracion de prioridad de los hilos
    sched_param params;
    int priority = sched_get_priority_max(SCHED_FIFO);
    params.sched_priority = priority;

    //Arreglo de Publicadores conectados
    QVector<Publisher> Publishers;
    Publisher publisher();


    //std::vector<pthread_t> threads; //Vector que almacena todos los procesos clientes (publicadores y suscriptores)

    //Declaramos los hilos del proceso central
    pthread_t broker; //Un hilo para el manejo del broker
    pthread_t thread; //Un hilo para publicador
    pthread_t thread2;//Un hilo para publicador
    pthread_t thread3;//Un hilo para suscriptor

    //Definimos un contexto para conectar todos los sockets para un maximo de 5 hilos
    zmq::context_t context{5};

    //Iniciamos el hilo con los procesos del broker
    broker_args ba1 = {&context,Publishers};
    pthread_create(&broker,NULL,Broker,&ba1);

    //Establece la prioridad del hilo de alta prioridad
    pthread_setschedparam(broker, SCHED_FIFO, &params);

    //Se definen las clases del publicador

    publisher_args pa1 = {&context,"publisher1"};
    publisher_args pa2 = {&context,"publisher2"};
    publisher_args pa3 = {&context,"subscriber1"};



    //Se inician los huilos con los procesos del suscriptor
    pthread_create(&thread3,NULL,Subscriber_routine,&pa3);
    //Se inician los hilos con los procesos del publicador
    pthread_create(&thread,NULL,Publisher_routine,&pa1);
    pthread_create(&thread2,NULL,Publisher_routine,&pa2);

    //Iniciamos la aplicacion
    QApplication a(argc, argv);
    MainWindow w(NULL,Publishers);
    w.show();

    return a.exec();

    pthread_join(broker,NULL);
    //pthread_join(thread,NULL);
    //pthread_join(thread2,NULL);


}
