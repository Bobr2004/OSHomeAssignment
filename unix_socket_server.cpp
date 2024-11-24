#include <iostream>
#include <string>
#include <sstream>
#include <iterator>
#include <vector>
#include <map>
#include <future>
#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdlib>

class Task {
public:
    virtual std::string execute(int input) const = 0;
    virtual ~Task() = default;
};

class SquareTask : public Task {
public:
    std::string execute(int input) const override {
        return "Square: " + std::to_string(input * input);
    }
};

class CubeTask : public Task {
public:
    std::string execute(int input) const override {
        return "Cube: " + std::to_string(input * input * input);
    }
};

class ReverseTask : public Task {
public:
    std::string execute(int input) const override {
        std::string numStr = std::to_string(input);
        std::reverse(numStr.begin(), numStr.end());
        return "Reversed: " + numStr;
    }
};

class UnixSocketServer {
public:
    UnixSocketServer(const std::string& path) : socketPath(path), serverSocket(-1) {}

    ~UnixSocketServer() {
        if (serverSocket != -1) close(serverSocket);
        unlink(socketPath.c_str());
    }

    void start() {
        struct sockaddr_un addr;
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, socketPath.c_str(), sizeof(addr.sun_path) - 1);

        serverSocket = socket(AF_UNIX, SOCK_STREAM, 0);
        if (serverSocket < 0) {
            perror("socket");
            exit(EXIT_FAILURE);
        }

        unlink(socketPath.c_str());

        if (bind(serverSocket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            perror("bind");
            exit(EXIT_FAILURE);
        }

        if (listen(serverSocket, 5) < 0) {
            perror("listen");
            exit(EXIT_FAILURE);
        }

        std::cout << "Server started. Listening on " << socketPath << "\n";

        while (true) {
            int clientSocket = accept(serverSocket, nullptr, nullptr);
            if (clientSocket < 0) {
                perror("accept");
                continue;
            }

            std::async(std::launch::async, &UnixSocketServer::handleClient, this, clientSocket);
        }
    }

private:
    void handleClient(int clientSocket) {
        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        read(clientSocket, buffer, sizeof(buffer));

        std::string request(buffer);
        std::cout << "Received request: " << request << "\n";

        int number = std::stoi(request);
        std::string response = processTask(number);

        write(clientSocket, response.c_str(), response.size());
        close(clientSocket);
    }

    std::string processTask(int input) {
        auto task = tasks["default"];
        if (!task) return "Error: Task not found\n";
        return task->execute(input) + "\n";
    }

public:
    void addTask(const std::string& name, std::shared_ptr<Task> task) {
        tasks[name] = task;
    }

private:
    std::string socketPath;
    int serverSocket;
    std::map<std::string, std::shared_ptr<Task>> tasks;
};

int main() {
    UnixSocketServer server("/tmp/unix_socket_example");

    server.addTask("default", std::make_shared<SquareTask>());
    server.addTask("cube", std::make_shared<CubeTask>());
    server.addTask("reverse", std::make_shared<ReverseTask>());

    std::cout << "Available commands:\n";
    std::cout << "  start                - Start the server.\n";
    std::cout << "  exit                 - Exit the program.\n";

    std::string input;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input == "start") {
            server.start();
        } else if (input == "exit") {
            break;
        } else {
            std::cerr << "Unknown command: " << input << "\n";
        }
    }

    return 0;
}
