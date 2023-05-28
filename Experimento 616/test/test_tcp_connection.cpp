#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>

std::string generateRandomID(int length)
{
    const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    const int charsetSize = sizeof(charset) - 1; // The size of the charset, minus the null terminator
    std::string id;
    srand(time(0)); // Use the current time as the seed for the random number generator
    
    for (int i = 0; i < length; i++)
    {
        int index = rand() % charsetSize;
        id += charset[index];
    }
    
    return id;
}

int main(int argc, char *argv[]) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cerr << "Error al crear el socket" << std::endl;
        return 1;
    }

    std::string server_ip = "127.0.0.1";
    int server_port = 1883;
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(server_ip.c_str());
    server_address.sin_port = htons(server_port);

    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Error al conectar al servidor" << std::endl;
        return 1;
    }


    while(true){
        char message[] = "Hola, servidor!";
        
        if (send(sock, message, strlen(message), 0) < 0) {
            std::cerr << "Errorsending message" << std::endl;
            break;
        }

        std::cout << "Message sent: " << message << std::endl;
        sleep(1);
        
    }
    close(sock);
    return 0;
}