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

    redisReply* reply = (redisReply*)redisCommand(redis, "HMSET publisher Id %s Mensaje %s", publisher_id.c_str(), message.c_str()); // Insertar el publicador en la tabla "publisher"
    freeReplyObject(reply);

    redisFree(redis); // Cerrar la conexión a Redis
    
    publishers.push_back({ publisher_id, message });

    std::string json_string = publisher_id + "-" + message;

    //publisher_socket.send(zmq::buffer(json_string), zmq::send_flags::none);
}

void list_publishers()
{
    std::cout << "Publishers:" << std::endl;
    redisContext* redis = redisConnect("127.0.0.1", 6379); // Conectarse a Redis
    if (redis == NULL || redis->err) {
        std::cout << "Error al conectarse a Redis: " << redis->errstr << std::endl;
        return;
    }

    redisReply* reply = (redisReply*)redisCommand(redis, "HGETALL publisher"); // Obtener todos los campos y valores de la tabla "publisher"
    //redisReply* reply = (redisReply*)redisCommand(redis, "HVALS publisher"); // Obtener todos los valores de la tabla "publisher"
    if (reply == NULL || reply->type == REDIS_REPLY_ERROR) {
        std::cout << "Error al obtener los datos de la tabla: " << redis->errstr << std::endl;
        freeReplyObject(reply);
        redisFree(redis);
        return;
    }

    if (reply->type == REDIS_REPLY_ARRAY) {
        for (int i = 0; i < reply->elements; i += 2) {
            std::cout << reply->element[i]->str << ": " << reply->element[i+1]->str << std::endl; // Imprimirtodos los campos y valores de la tabla
        }
    }

    freeReplyObject(reply);
    redisFree(redis); // Cerrar la conexión a Redis
}

int main(){
    std::string publisher_id;
    std::string message;

    std::cout << "\n";
    publisher_id = to_string(generarIDUnico());

    std::cout << "Enter message: ";
    std::cin >> message;

    publish(publisher_id, message);

    list_publishers();
    return 0;
}