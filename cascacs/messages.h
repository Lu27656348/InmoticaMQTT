#include <string>
#include <vector>

#ifndef CONNECT_H
#define CONNECT_H

class CONNECT {
    private:
        std::string clientId;
        bool cleanSession;
        int keepAlive;

        std::string username;
        std::string password;
        std::string lastWillTopic;
        int lastWillQos;
        std::string lastWillMessage;
        bool lastWillRetain;

    public:
        CONNECT(std::string clientId,bool cleanSession,int keepAlive)
        :
        clientId(clientId),
        cleanSession(cleanSession),
        keepAlive(keepAlive){}

     //Recordar meter SETTERS Y GETTERS AQUI
};

#endif // CONNECT_H

#ifndef CONNACK_H
#define CONNACK_H

/*
returnCode ->
    0: Accepted
    1: refused - unacceptable protocol
    2: refused - identifier rejected
    3: refused - server unavailable
    4: refused - bad username or password
    5: refused - not authorized
*/

class CONNACK {
    private:
        bool sessionPresent;
        int returnCode;
    public:
        CONNACK(bool sessionPresent, int returnCode)
            :
            sessionPresent(sessionPresent),
            returnCode(returnCode){}
};

#endif // CONNACK_H

#ifndef PUBLISH_H
#define PUBLISH_H

class PUBLISH {
        int packetId; //Always 0 for qos 0
        std::string topicName;
        int qos;
        bool retainFlag;
        std::string payload;
        bool dupFlag;
};

#endif // PUBLISH_H

#ifndef SUBSCRIBE_H
#define SUBSCRIBE_H
class SUBSCRIBE {
    int packetId; //Always 0 for qos 0
    std::vector<std::string> topicList;

};
#endif // SUBSCRIBE_H
class UNSUBCRIBE {


};

class SUBACK {


};

class UNSUBACK {


};


