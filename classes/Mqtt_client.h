#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;

enum QosLevel {
    AT_MOST_ONCE = 0,
    AT_LEAST_ONCE = 1,
    EXACTLY_ONCE = 2
};

class MqttClient {
    private:
        std::string client_id;
        bool clean_session; //Dice si la conexion es permanente o no
                            //Si la clean_session = true -> no guarda nada
                            //de lo contrario, si el Qos = 1 o 2, guarda.
        std::string will_message; //Informa a los demas clientes si en cliente esta
                                  //desconectado por causa mayor
        std::string will_topic;
        QosLevel will_qos;
        bool will_retain;
        std::string username;
        std::string password;
        int keep_alive_interval;
    public:
        MqttClient(std::string client_id, bool clean_session, int keep_alive_interval)
         : client_id(client_id),
           clean_session(clean_session),
           keep_alive_interval(keep_alive_interval){};
        void hello_world(){
            cout << "Hello world" << endl;
        }
};

#endif //MQTT_CLIENT_H