#ifndef PUBLISHER_ROUTINE_H
#define PUBLISHER_ROUTINE_H

#include <zmq.hpp>
#include <iostream>

struct publisher_args {
    zmq::context_t* context;
    std::string publisher_id;
};

void* Publisher_routine(void* arg)
{
    //Primero tomamos el objeto pasado como parametro por el hilo
    publisher_args* args = static_cast<publisher_args*>(arg);

    //Extraemos las propiedades del objeto (id_publicador y el contexto de conexion)
    zmq::context_t* context = args->context;
    std::string publisher_id = args->publisher_id;

    //Iniciamos el publisher
    std::cout << "Publisher iniciado " <<std::endl;
    zmq::socket_t socket(*context, ZMQ_PUB);

    //Conectamos el publisher al puerto del broker
    socket.connect("tcp://localhost:5555");

    //Creamos un publisher de prueba
    Publisher publisher(publisher_id,"sensor/bombillo",60);



    while (true) {

        if(!publisher.getConnected()){
            std::cout << "El publicador no se ha conectado " << std::endl;
            if(publisher.sendConnect(true,socket)){
                std::cout << "CONNACK" << std::endl;

                std::ostringstream oss;
                oss << publisher;
                std::string payload = oss.str();

                sendMesage_(socket,"PUBLISH",payload);
            }
        }else{
            std::cout << "El publicador "<< publisher_id <<" se ha conectado " << std::endl;


        }

        std::ostringstream oss;
        oss << publisher;
        std::string payload = oss.str();

        usleep(3000000); // Esperar un segundo
    }

    return NULL;

    while(true){
        std::string message = "Mensaje " + std::to_string(generate_id_());
        zmq::message_t zmq_message(message.size());
        memcpy(zmq_message.data(), message.c_str(), message.size());




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
#endif // PUBLISHER_ROUTINE_H
