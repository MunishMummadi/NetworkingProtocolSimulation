#include <iostream>
#include <thread>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

void server() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Creating socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    // Receive data from client
    read(new_socket, buffer, 1024);
    std::cout << "Received message: " << buffer << std::endl;

    // Send acknowledgment
    const char *ack = "Packet received";
    send(new_socket, ack, strlen(ack), 0);
    std::cout << "Acknowledgment sent" << std::endl;

    close(new_socket);
    close(server_fd);
}

void client() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    const char *message = "Hello, Server";
    char buffer[1024] = {0};

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cout << "\nSocket creation error\n";
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cout << "\nInvalid address / Address not supported\n";
        return;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "\nConnection Failed\n";
        return;
    }

    // Send message to server
    send(sock, message, strlen(message), 0);
    std::cout << "Message sent" << std::endl;

    // Receive acknowledgment
    read(sock, buffer, 1024);
    std::cout << "Acknowledgment received: " << buffer << std::endl;

    close(sock);
}

int main() {
    std::thread server_thread(server);
    std::this_thread::sleep_for(std::chrono::seconds(1));  // Delay to let the server start
    std::thread client_thread(client);

    server_thread.join();
    client_thread.join();
    
    return 0;
}
