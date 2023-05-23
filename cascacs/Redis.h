#ifndef REDIS_H
#define REDIS_H
#include <string>
#include <iostream>
#include <hiredis/hiredis.h>

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

void add_pub(std::string publisher_id){
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

    redisReply* reply = (redisReply*)redisCommand(redis, "HSET publisher:%s clientId %s", str_i.c_str(), publisher_id.c_str()); // Insertar el publicador en la tabla "publisher"
    freeReplyObject(reply);

    redisFree(redis); // Cerrar la conexión a Redis
}

//Contar PUBS
int count_pubs() {
    redisContext* redis = redisConnect("127.0.0.1", 6379); // Conectarse a Redis
    if (redis == NULL || redis->err) {
        std::cout << "Error al conectarse a Redis: " << redis->errstr << std::endl;
        return -1;
    }

    int count = 0;
    int i = 0;
    std::string str_i;
    bool done = false;
    do {
        str_i = std::to_string(i);

        // Verificar si el suscriptor existe
        redisReply* reply = (redisReply*)redisCommand(redis,"EXISTS publisher:%s", str_i.c_str());
        if (reply == nullptr) {
            std::cout << "Error al consultar Redis: " << redis->errstr << std::endl;
            redisFree(redis);
            return -1;
        }

        // Si el objeto reply es 1, incrementar el contador
        if (reply->integer == 1) {
            count++;
        }

        // Liberar el objeto reply
        freeReplyObject(reply);

        i++;

        // Comprobar si se han procesado todos los suscriptores
        if (reply->integer == 0) {
            done = true;
        }

    } while (!done);

    redisFree(redis); // Cerrar la conexión a Redis
    return count;

}



//Verificar posicion de un publishe
int verify_publisher(std::string topic){
    redisContext* redis = redisConnect("127.0.0.1", 6379); // Conectarse a Redis
    if (redis == NULL || redis->err) {
        std::cout << "Error al conectarse a Redis: " << redis->errstr << std::endl;
        return -1;
    }

    int i = 0;
    std::string str_i;
    do {
        str_i = std::to_string(i);

        // Recuperar los datos del publicador
        redisReply* reply = (redisReply*)redisCommand(redis,"HMGET publisher:%s clientId", str_i.c_str());
        if (reply == nullptr) {
            std::cout << "Error al consultar Redis: " << redis->errstr << std::endl;
            redisFree(redis);
            return -1;
        }

        // Si el objeto reply tiene elementos, comprobar si el topic ya existe
        if (reply->elements > 0 && strcmp(reply->element[0]->str, topic.c_str()) == 0) {
            // Mostrar los datos del publicador en la pantalla
            std::cout << "Existe el publisher" << std::endl;
            freeReplyObject(reply);
            return i;
        }

        // Liberar el objeto reply
        freeReplyObject(reply);

        i++;
    } while (i < count_pubs());

    std::cout << "No existe el publisher" << std::endl;
    return -1;
}



//Listar Publishers
void list_publishers()
{
    std::cout << "publshers:" << std::endl;
    redisContext* redis = redisConnect("127.0.0.1", 6379); // Conectarse a Redis
    if (redis == NULL || redis->err) {
        std::cout << "Error al conectarse a Redis: " << redis->errstr << std::endl;
        return;
    }

    // Verificar hasta donde hay datos en la tabla "publisher"
    int i = 0;
    redisReply* ver = nullptr;
    std::string str_i;
    bool done = true;
    do {
        str_i = std::to_string(i);

        // Recuperar los datos del publicador
        redisReply* reply = (redisReply*)redisCommand(redis,"HMGET publisher:%s clientId", str_i.c_str());
        if (reply == nullptr) {
            std::cout << "Error al consultar Redis: " << redis->errstr << std::endl;
            redisFree(redis);
            return;
        }

        // Mostrar los datos del publicador en la pantalla
        std::cout << "Id: " << reply->element[0]->str << std::endl;

        freeReplyObject(reply);

        i++;

    } while (i < count_pubs());

    redisFree(redis); // Cerrar la conexión a Redis
}




//Contar SERV
int count_serv() {
    redisContext* redis = redisConnect("127.0.0.1", 6379); // Conectarse a Redis
    if (redis == NULL || redis->err) {
        std::cout << "Error al conectarse a Redis: " << redis->errstr << std::endl;
        return -1;
    }

    int count = 0;
    int i = 0;
    std::string str_i;
    bool done = false;
    do {
        str_i = std::to_string(i);

        // Verificar si el suscriptor existe
        redisReply* reply = (redisReply*)redisCommand(redis,"EXISTS service:%s", str_i.c_str());
        if (reply == nullptr) {
            std::cout << "Error al consultar Redis: " << redis->errstr << std::endl;
            redisFree(redis);
            return -1;
        }

        // Si el objeto reply es 1, incrementar el contador
        if (reply->integer == 1) {
            count++;
        }

        // Liberar el objeto reply
        freeReplyObject(reply);

        i++;

        // Comprobar si se han procesado todos los suscriptores
        if (reply->integer == 0) {
            done = true;
        }

    } while (!done);

    redisFree(redis); // Cerrar la conexión a Redis
    return count;

}



//Verificar si ya existe el servicio para crearlo
bool verify_service(std::string topic){
    redisContext* redis = redisConnect("127.0.0.1", 6379); // Conectarse a Redis
    if (redis == NULL || redis->err) {
        std::cout << "Error al conectarse a Redis: " << redis->errstr << std::endl;
        return false;
    }

    int i = 0;
    std::string str_i;
    do {
        str_i = std::to_string(i);

        // Recuperar los datos del publicador
        redisReply* reply = (redisReply*)redisCommand(redis,"HMGET service:%s topic", str_i.c_str());
        if (reply == nullptr) {
            std::cout << "Error al consultar Redis: " << redis->errstr << std::endl;
            redisFree(redis);
            return false;
        }

        // Si el objeto reply tiene elementos, comprobar si el topic ya existe
        if (reply->elements > 0 && strcmp(reply->element[0]->str, topic.c_str()) == 0) {
            // Mostrar los datos del publicador en la pantalla
            std::cout << "Topic ya ingresado" << std::endl;
            freeReplyObject(reply);
            return false;
        }

        // Liberar el objeto reply
        freeReplyObject(reply);

        i++;
    } while (i < count_serv());

    return true;
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

    // Verificar si el servicio existe
    if (!verify_service(topic)){
        return;
    }


    // Verificar hasta donde hay datos en la tabla "service"
    int i = 0;
    std::string str_i;
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
    std::string subscriber_id;

    std::cout << "\n";
    subscriber_id = std::to_string(generarIDUnico());

    redisContext* redis = redisConnect("127.0.0.1", 6379); // Conectarse a Redis
    if (redis == NULL || redis->err) {
        std::cout << "Error al conectarse a Redis: " << redis->errstr << std::endl;
        return;
    }

    // Verificar hasta donde hay datos en la tabla "subscriber"
    int i = 0;
    redisReply* ver = nullptr;
    std::string str_i;
    do {
        str_i = std::to_string(i);
        ver = (redisReply*)redisCommand(redis, "EXISTS subscriber:%s", str_i.c_str());
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

    redisReply* reply = (redisReply*)redisCommand(redis, "HMSET subscriber:%s Id %s", str_i.c_str(), subscriber_id.c_str()); // Insertar el publicador en la tabla "publisher"
    freeReplyObject(reply);

    redisFree(redis); // Cerrar la conexión a Redis
}



//Contar SUBS
int count_subs() {
    redisContext* redis = redisConnect("127.0.0.1", 6379); // Conectarse a Redis
    if (redis == NULL || redis->err) {
        std::cout << "Error al conectarse a Redis: " << redis->errstr << std::endl;
        return -1;
    }

    int count = 0;
    int i = 0;
    std::string str_i;
    bool done = false;
    do {
        str_i = std::to_string(i);

        // Verificar si el suscriptor existe
        redisReply* reply = (redisReply*)redisCommand(redis,"EXISTS subscriber:%s", str_i.c_str());
        if (reply == nullptr) {
            std::cout << "Error al consultar Redis: " << redis->errstr << std::endl;
            redisFree(redis);
            return -1;
        }

        // Si el objeto reply es 1, incrementar el contador
        if (reply->integer == 1) {
            count++;
        }

        // Liberar el objeto reply
        freeReplyObject(reply);

        i++;

        // Comprobar si se han procesado todos los suscriptores
        if (reply->integer == 0) {
            done = true;
        }

    } while (!done);

    redisFree(redis); // Cerrar la conexión a Redis
    return count;

}



//verificar si existe el subscriptor
int verify_sub(std::string id){
    redisContext* redis = redisConnect("127.0.0.1", 6379); // Conectarse a Redis
    if (redis == NULL || redis->err) {
        std::cout << "Error al conectarse a Redis: " << redis->errstr << std::endl;
        return -1;
    }

    int i = 0;
    std::string str_i;
    do {
        str_i = std::to_string(i);

        // Recuperar los datos del publicador
        redisReply* reply = (redisReply*)redisCommand(redis,"HMGET subscriber:%s Id", str_i.c_str());
        if (reply == nullptr) {
            std::cout << "Error al consultar Redis: " << redis->errstr << std::endl;
            redisFree(redis);
            return -1;
        }

        // Comprobar si no hay más suscriptores en la tabla
        if (reply->elements == 0) {
            std::cout << "No existe subscriptor\n";
            redisFree(redis); // Cerrar la conexión a Redis
            return -1;
        }

        // Si el objeto reply tiene elementos, comprobar si el topic ya existe
        if (reply->elements > 0 && strcmp(reply->element[0]->str, id.c_str()) == 0) {
            // Mostrar los datos del publicador en la pantalla
            std::cout << "True" << std::endl;
            freeReplyObject(reply);
            redisFree(redis); // Cerrar la conexión a Redis
            return i;
        }

        // Liberar el objeto reply
        freeReplyObject(reply);

        i++;
    } while (i < count_subs());

    redisFree(redis); // Cerrar la conexión a Redis
    std::cout << "no consegui al pana\n";
    return -1;
}



//Verificar si ya existe el servicio
bool verify_sub_service(std::string topic){
    redisContext* redis = redisConnect("127.0.0.1", 6379); // Conectarse a Redis
    if (redis == NULL || redis->err) {
        std::cout << "Error al conectarse a Redis: " << redis->errstr << std::endl;
        return false;
    }

    int i = 0;
    std::string str_i;
    do {
        str_i = std::to_string(i);

        // Recuperar los datos del publicador
        redisReply* reply = (redisReply*)redisCommand(redis,"HMGET service:%s topic", str_i.c_str());
        if (reply == nullptr) {
            std::cout << "Error al consultar Redis: " << redis->errstr << std::endl;
            redisFree(redis);
            return false;
        }

        // Si el objeto reply tiene elementos, comprobar si el topic ya existe
        if (reply->elements > 0 && strcmp(reply->element[0]->str, topic.c_str()) == 0) {
            // Mostrar los datos del publicador en la pantalla
            std::cout << "El servicio existe" << std::endl;
            freeReplyObject(reply);
            return true;
        }

        // Liberar el objeto reply
        freeReplyObject(reply);

        i++;
    } while (i < count_serv());

    return false;
}





//Servicios a los que esta suscrito un Publisher
void services_for_subscriber(int id)
{
    std::cout << "Subscriber:" << id << std::endl;
    redisContext* redis = redisConnect("127.0.0.1", 6379); // Conectarse a Redis
    if (redis == NULL || redis->err) {
        std::cout << "Error al conectarse a Redis: " << redis->errstr << std::endl;
        return;
    }

    // Verificar hasta donde hay datos en la tabla "publisher"
    redisReply* ver = nullptr;
    std::string str_i = std::to_string(verify_sub(std::to_string(id)));

    // Recuperar los datos del publicador
    redisReply* reply = (redisReply*)redisCommand(redis,"HVALS subscriber:%s", str_i.c_str());
    if (reply == nullptr) {
        std::cout << "Error al consultar Redis: " << redis->errstr << std::endl;
        redisFree(redis);
        return;
    }

    // Mostrar los servicios del publicador en la pantalla
    if (reply->type == REDIS_REPLY_ARRAY) {
        std::cout << "Servicios: ";
        for (size_t i = 0; i < reply->elements; i++) {
            std::cout << reply->element[i]->str << " ";
        }
        std::cout << std::endl;
    }

    freeReplyObject(reply);


    redisFree(redis); // Cerrar la conexión a Redis
}





//Suscriptor se suscribe a servicio
void sub_subscribe(int id)
{
    std::string str_id = std::to_string(id);

    // Verificar si existe el suscriptor
    if (verify_sub(str_id) == -1) {
        std::cout << "No existe el siguiente suscriptor\n";
        return;
    }

    // Ingresar a qué servicio se quiere suscribir
    std::cout << "¿A qué servicio se quiere suscribir?\n";
    std::string serv;
    std::cin >> serv;

    // Verificar si el servicio existe
    if (!verify_sub_service(serv)) {
        std::cout << "No existe el servicio\n";
        return;
    }

    // Conectar a Redis
    redisContext* redis = redisConnect("127.0.0.1", 6379); // Conectarse a Redis
    if (redis == NULL || redis->err) {
        std::cout << "Error al conectarse a Redis: " << redis->errstr << std::endl;
        return;
    }

    // Agregar servicio al suscriptor
    std::string str_i = std::to_string(verify_sub(str_id));

    redisReply* reply = (redisReply*)redisCommand(redis, "HGET subscriber:%s services", str_i.c_str());
    if (reply == nullptr || reply->type == REDIS_REPLY_ERROR) {
        std::cout << "Error al consultar los servicios suscritos del suscriptor" << std::endl;
        freeReplyObject(reply);
        redisFree(redis);
        return;
    }

    if (reply->type == REDIS_REPLY_NIL) {
        // Suscriptor no suscrito a ningún servicio
        redisReply* reply2 = (redisReply*)redisCommand(redis, "HSET subscriber:%s services %s", str_i.c_str(), serv.c_str());
        if (reply2 == nullptr || reply2->type == REDIS_REPLY_ERROR) {
            std::cout << "Error al agregar el servicio al suscriptor" << std::endl;
            freeReplyObject(reply2);
            freeReplyObject(reply);
            redisFree(redis);
            return;
        } else {
            std::cout << "Suscriptor suscrito a su primer servicio" << std::endl;
            freeReplyObject(reply2);
            freeReplyObject(reply);
            redisFree(redis);
            return;
        }
    } else if (reply->type == REDIS_REPLY_STRING) {
        // El suscriptor ya está suscrito a algún servicio, agregar el nuevo servicio a la cadena
        std::string current_services = reply->str;
        if (current_services.find(serv) != std::string::npos) {
            std::cout << "El suscriptor ya está suscrito a este servicio" << std::endl;
                    freeReplyObject(reply);
            redisFree(redis);
            return;
        } else {
            current_services += " " + serv;
            redisReply* reply2 = (redisReply*)redisCommand(redis, "HSET subscriber:%s services %s", str_i.c_str(), current_services.c_str());
            if (reply2 == nullptr || reply2->type == REDIS_REPLY_ERROR) {
                std::cout << "Error al agregar el servicio al suscriptor" << std::endl;
                freeReplyObject(reply2);
                freeReplyObject(reply);
                redisFree(redis);
                return;
            } else {
                std::cout << "El suscriptor ha sido suscrito al servicio exitosamente" << std::endl;
                freeReplyObject(reply2);
                freeReplyObject(reply);
                redisFree(redis);
                return;
            }
        }
    } else {
        std::cout << "Error al consultar los servicios suscritos del suscriptor" << std::endl;
        freeReplyObject(reply);
        redisFree(redis);
        return;
    }

    // Cerrar la conexión a Redis
    freeReplyObject(reply);
    redisFree(redis);
}



void subs_for_services(std::string service) {
    redisContext* redis = redisConnect("127.0.0.1", 6379); // Conectarse a Redis
    if (redis == NULL || redis->err) {
        std::cout << "Error al conectarse a Redis: " << redis->errstr << std::endl;
        return;
    }

    int i = 0;
    std::string str_i;

    std::cout << "Lista de Suscriptores suscritos a " << service << std::endl;
    do {
        str_i = std::to_string(i);
        // Recuperar los datos del suscriptor
        redisReply* reply = (redisReply*)redisCommand(redis, "HMGET subscriber:%s Id services", str_i.c_str());
        if (reply == nullptr) {
            std::cout << "Error al consultar Redis: " << redis->errstr << std::endl;
            redisFree(redis);
            return;
        }

        // Mostrar los subscriptores que están suscritos al servicio específico
        if (reply->type == REDIS_REPLY_ARRAY && reply->elements >= 2) {
            if (reply->element[1]->str != nullptr){
                std::string current_services = reply->element[1]->str;
                if (current_services.find(service) != std::string::npos) {
                    std::cout << "Suscriptor ID: " << reply->element[0]->str << std::endl;
                }
            }
        }
        freeReplyObject(reply);

        i++;
    } while (i < count_subs());

    redisFree(redis); // Cerrar la conexión a Redis
}

#endif // REDIS_H
