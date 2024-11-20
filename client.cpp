#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

constexpr int PORT = 8080;

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address or address not supported" << std::endl;
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        return -1;
    }

    int x;
    double result;

    std::cout << "Enter an integer: ";
    std::cin >> x;

    send(sock, &x, sizeof(x), 0);
    std::cout << "Sent: " << x << std::endl;

    if (read(sock, &result, sizeof(result)) > 0) {
        std::cout << "Result: " << result << std::endl;
    } else {
        std::cerr << "Failed to receive result" << std::endl;
    }

    close(sock);
    return 0;
}
