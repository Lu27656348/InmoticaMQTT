#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <string>
#include <vector>
#include <algorithm>

class mqtt_client {
private:
    int socket_fd;
    std::string client_id;
    bool clean_session;
    mutable std::vector<std::string> subscriptions;
public:
    mqtt_client(int socket_fd, const std::string& client_id, bool clean_session)
        : socket_fd(socket_fd), client_id(client_id), clean_session(clean_session) {}

    int get_socket_fd() const {
        return socket_fd;
    }

    const std::string& get_client_id() const {
        return client_id;
    }

    bool is_clean_session() const {
        return clean_session;
    }

    void add_subscription(const std::string& topic) {
        subscriptions.push_back(topic);
    }

    void remove_subscription(const std::string& topic) {
        subscriptions.erase(std::remove(subscriptions.begin(), subscriptions.end(), topic),
                            subscriptions.end());
    }

    const std::vector<std::string>& get_subscriptions() const {
        return subscriptions;
    }
};

#endif