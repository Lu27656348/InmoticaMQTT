#include <iostream>
#include <string>
#include <zmq.hpp>
#include <functional>
#include <ctime>
#include <vector>
#include <hiredis/hiredis.h>

using namespace std;

struct Subscriber
{
    std::string id;
    std::vector<std::string> services;
};

struct Publisher
{
    std::string id;
    std::string message;
};

std::vector<Subscriber> subscribers;
std::vector<Publisher> publishers;

int generarIDUnico() {
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

void publish(const std::string& publisher_id, const std::string& message)
{
    redisContext* redis = redisConnect("127.0.0.1", 6379); // Conectarse a Redis
    if (redis == NULL || redis->err) {
        std::cout << "Error al conectarse a Redis: " << redis->errstr << std::endl;
        return;
    }

    // Verificar hasta donde hay datos en la tabla "publisher"
    int i = 0;
    redisReply* ver = nullptr;
    std::string str_i;
    do {
        str_i = std::to_string(i);
        ver = (redisReply*)redisCommand(redis, "EXISTS publisher:%s", str_i.c_str());
        if (ver == nullptr) {
            std::cout << "Error al consultar Redis: " << redis->errstr << std::endl;
            redisFree(redis);
            return;
        }
        if (ver->integer == 0) {
            freeReplyObject(ver);
            break;
        }
        freeReplyObject(ver);
        i++;
    } while (true);

    redisReply* reply = (redisReply*)redisCommand(redis, "HMSET publisher:%s Id %s Mensaje %s", str_i.c_str(), publisher_id.c_str(), message.c_str()); // Insertar el publicador en la tabla "publisher"
    freeReplyObject(reply);

    redisFree(redis); // Cerrar la conexión a Redis
    
    publishers.push_back({ publisher_id, message });

    std::string json_string = publisher_id + "-" + message;

    //publisher_socket.send(zmq::buffer(json_string), zmq::send_flags::none);
}

void list_publishers()
{
    std::cout << "services:" << std::endl;
    redisContext* redis = redisConnect("127.0.0.1", 6379); // Conectarse a Redis
    if (redis == NULL || redis->err) {
        std::cout << "Error al conectarse a Redis: " << redis->errstr << std::endl;
        return;
    }

    // Verificar hasta donde hay datos en la tabla "publisher"
    int i = 0;
    redisReply* ver = nullptr;
    std::string str_i;
    do {
        str_i = std::to_string(i);

        // Recuperar los datos del publicador
        redisReply* reply = (redisReply*)redisCommand(redis,"HMGET publisher:%s Id Mensaje", str_i.c_str());
        if (reply == nullptr) {
            std::cout << "Error al consultar Redis: " << redis->errstr << std::endl;
            redisFree(redis);
            return;
        }

        // Mostrar los datos del publicador en la pantalla
        std::cout << "Id: " << reply->element[0]->str << ", Mensaje: " << reply->element[1]->str << std::endl;

        freeReplyObject(reply);

        i++;
    } while (true);

    redisFree(redis); // Cerrar la conexión a Redis
}

void services_for_publisher(int id)
{
    std::cout << "Publisher:" << id << std::endl;
    redisContext* redis = redisConnect("127.0.0.1", 6379); // Conectarse a Redis
    if (redis == NULL || redis->err) {
        std::cout << "Error al conectarse a Redis: " << redis->errstr << std::endl;
        return;
    }

    // Verificar hasta donde hay datos en la tabla "publisher"
    redisReply* ver = nullptr;
    std::string str_i = to_string(id);

    // Recuperar los datos del publicador
    redisReply* reply = (redisReply*)redisCommand(redis,"HVALS publisher:%s", str_i.c_str());
    if (reply == nullptr) {
        std::cout << "Error al consultar Redis: " << redis->errstr << std::endl;
        redisFree(redis);
        return;
    }

    // Mostrar los servicios del publicador en la pantalla
    if(reply->type == REDIS_REPLY_ARRAY){
        std::cout << "Servicios: ";
        cout << reply->element[2]->str << " ";
        std::cout << std::endl;
    }

    freeReplyObject(reply);


    redisFree(redis); // Cerrar la conexión a Redis
}


//Crear servicio
void create_service(){
    std::string topic;
    std::cout << "Enter message: ";
    std::cin >> topic;

    redisContext* redis = redisConnect("127.0.0.1", 6379); // Conectarse a Redis
    if (redis == NULL || redis->err) {
        std::cout << "Error al conectarse a Redis: " << redis->errstr << std::endl;
        return;
    }

    // Verificar hasta donde hay datos en la tabla "service"
    int i = 0;
    std::string str_i;
    do {
        str_i = std::to_string(i);

        // Recuperar los datos del publicador
        redisReply* reply = (redisReply*)redisCommand(redis,"HMGET service:%s topic", str_i.c_str());
        if (reply == nullptr) {
            std::cout << "Error al consultar Redis: " << redis->errstr << std::endl;
            redisFree(redis);
            return;
        }

        //si ya está repetido salir de la función
        if (strcmp(reply->element[0]->str, topic.c_str()) == 0) {
            // Mostrar los datos del publicador en la pantalla
            std::cout << "Topic ya ingresado" << std::endl;
            freeReplyObject(reply);
            return;
        }
        
        freeReplyObject(reply);

        i++;
    } while (true);

    // Verificar hasta donde hay datos en la tabla "service"
    i = 0;
    redisReply* ver = nullptr;
    do {
        str_i = std::to_string(i);
        ver = (redisReply*)redisCommand(redis, "EXISTS service:%s", str_i.c_str());
        if (ver == nullptr) {
            std::cout << "Error al consultar Redis: " << redis->errstr << std::endl;
            redisFree(redis);
            return;
        }
        if (ver->integer == 0) {
            freeReplyObject(ver);
            break;
        }
        freeReplyObject(ver);
        i++;
    } while (true);

    redisReply* reply = (redisReply*)redisCommand(redis, "HMSET service:%s topic %s", str_i.c_str(), topic.c_str()); // Insertar el publicador en la tabla "publisher"
    freeReplyObject(reply);

    redisFree(redis); // Cerrar la conexión a Redis
}


//Crea un suscriptor
void create_subscriber(){

}

int main(){
    std::string publisher_id;
    std::string message;

    std::cout << "\n";
    publisher_id = to_string(generarIDUnico());

    //std::cout << "Enter message: ";
    //std::cin >> message;

    //publish(publisher_id, message);

    //list_publishers();

    //services_for_publisher(92878);

    //services_for_publisher(2);

    create_service();

    return 0;
}