#include <iostream>
#include <string>
#include <zmq.hpp>
#include <functional>
#include <ctime>
#include <vector>

using namespace std;

const std::string BROKER_ADDRESS { "tcp://localhost:1883" };
const std::string SUBSCRIBER_SOCKET_ADDRESS { "tcp://*:5555" };
const std::string PUBLISHER_SOCKET_ADDRESS { "tcp://*:6666" };
const int QOS = 1;

zmq::context_t context(1);
zmq::socket_t subscriber_socket(context, zmq::socket_type::sub);
zmq::socket_t publisher_socket(context, zmq::socket_type::pub);

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

void subscribe(const std::string& subscriber_id, const std::string& service)
{
    auto it = subscribers.begin();
    while (it != subscribers.end())
    {
        if (it->id == subscriber_id)
        {
            auto& services = it->services;
            bool found = false;
            for (const auto& s : services)
            {
                if (s == service)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                services.push_back(service);
            }
            break;
        }
        ++it;
    }
    if (it == subscribers.end())
    {
        subscribers.push_back({ subscriber_id, { service } });
    }

    subscriber_socket.setsockopt(ZMQ_SUBSCRIBE, service.c_str(), service.length());
}

void publish(const std::string& publisher_id, const std::string& message)
{
    publishers.push_back({ publisher_id, message });

    std::string json_string = publisher_id + "-" + message;

    publisher_socket.send(zmq::buffer(json_string), zmq::send_flags::none);
}

std::vector<std::string> get_subscriber_services(const std::string& subscriber_id)
{
    std::vector<std::string> services;
    for (const auto& subscriber : subscribers)
    {
        if (subscriber.id == subscriber_id)
        {
            services = subscriber.services;
            break;
        }
    }
    return services;
}

void list_subscribers()
{
    std::cout << "Subscribers:" << std::endl;
    for (const auto& subscriber : subscribers)
    {
        std::cout << "- " << subscriber.id << ": ";
        std::cout << "[ ";
        for (size_t i = 0; i < subscriber.services.size(); ++i)
        {
            std::cout << subscriber.services[i];
            if (i + 1 < subscriber.services.size())
            {
                std::cout << ", ";
            }
        }
        std::cout << " ]" << std::endl;
    }
}

void list_publishers()
{
    std::cout << "Publishers:" << std::endl;
    for (const auto& publisher : publishers)
    {
        std::cout << "- " << publisher.id << ": " << publisher.message << std::endl;
    }
}

int main()
{
    subscriber_socket.bind(SUBSCRIBER_SOCKET_ADDRESS);
    publisher_socket.bind(PUBLISHER_SOCKET_ADDRESS);

    subscriber_socket.connect(BROKER_ADDRESS);

    zmq::pollitem_t items [] = {
        { subscriber_socket, 0, ZMQ_POLLIN, 0 },
    };

    while (true)
    {
        std::cout << "Menu:" << std::endl;
        std::cout << "1. List subscribers" << std::endl;
        std::cout << "2. Get subscriber services" << std::endl;
        std::cout << "3. Subscribe" << std::endl;
        std::cout << "4. List publishers" << std::endl;
        std::cout << "5. Publish" << std::endl;
        std::cout << "6. Subscribe" << std::endl;
        std::cout << "7. Exit" << std::endl;

        int choice;
        std::string subscriber_id;
        std::vector<std::string> services;
        std::string service;
        std::string publisher_id;
        std::string message;
        std::cin >> choice;

        switch (choice)
        {
            case 1:
                list_subscribers();
                break;

            case 2:
                std::cout << "subscriber id: ";
                std::cin >> subscriber_id;

                services = get_subscriber_services(subscriber_id);
                std::cout << "Subscriber " << subscriber_id << " services: ";
                std::cout << "[ ";
                for (size_t i = 0; i < services.size(); ++i)
                {
                    std::cout << services[i];
                    if (i + 1 < services.size())
                    {
                        std::cout << ", ";
                    }
                }
                std::cout << " ]" << std::endl;
                break;

            case 3:
                std::cout << "Enter subscriber id: ";
                std::cin >> subscriber_id;

                std::cout << "Enter service: ";
                std::cin >> service;

                subscribe(subscriber_id, service);
                break;

            case 4:
                list_publishers();
                break;

            case 5:
                std::cout << "Publisher id: ";
                publisher_id = to_string(generarIDUnico());

                std::cout << "Enter message: ";
                std::cin.ignore();
                std::getline(std::cin, message);

                publish(publisher_id, message);
                break;
            
            case 6:
                std::cout << "Subscriber id: ";
                subscriber_id = to_string(generarIDUnico());

                // Verificar que el identificador del nuevo suscriptor no esté en uso
                for (const auto& subscriber : subscribers)
                {
                    if (subscriber.id == subscriber_id)
                    {
                        std::cout << "Error: subscriber with id " << subscriber_id << " already exists" << std::endl;
                        break;
                    }
                }

                if (std::find_if(subscribers.begin(), subscribers.end(),
                    [&](const Subscriber& s) { return s.id == subscriber_id; }) == subscribers.end())
                {
                    std::cout << "Enter service to subscribe: ";
                    std::cin >> service;

                    subscribe(subscriber_id, service);
                }
                break;

            case 7:
                return 0;

            default:
                std::cout << "Invalid choice" << std::endl;
                break;
        }

        zmq::poll(items, 1, 0);

        if (items[0].revents & ZMQ_POLLIN)
        {
            zmq::message_t topic;
            zmq::message_t payload;

            subscriber_socket.recv(topic, zmq::recv_flags::none);
            subscriber_socket.recv(payload, zmq::recv_flags::none);

            std::string topic_str(static_cast<char*>(topic.data()), topic.size());
            std::string payload_str(static_cast<char*>(payload.data()), payload.size());

            size_t pos = payload_str.find(":");
            std::string publisher_id = payload_str.substr(2, pos - 3);
            std::string message = payload_str.substr(pos + 3, payload_str.length() - pos - 5);

            std::cout << "Received message from " << topic_str << ": ";
            std::cout << "id = " << publisher_id << ", ";
            std::cout << "message = " << message << std::endl;
        }
    }

    return 0;
}