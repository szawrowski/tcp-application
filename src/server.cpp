#include "server.h"

#include <ctime>
#include <cstring>
#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <mutex>

#include <netinet/in.h>
#include <unistd.h>

TcpServer::TcpServer(const int port) : port_(port)
{
    server_socket_ = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket_ < 0)
    {
        std::cerr << "Error opening socket" << std::endl;
        exit(1);
    }

    sockaddr_in server_addr{};
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket_, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) < 0)
    {
        std::cerr << "Error binding socket" << std::endl;
        exit(1);
    }

    listen(server_socket_, 5);
    log_file_.open("log.txt", std::ios::out | std::ios::app);

    if (!log_file_.is_open())
    {
        std::cerr << "Error opening log file" << std::endl;
        exit(1);
    }
}

TcpServer::~TcpServer()
{
    close(server_socket_);

    if (log_file_.is_open())
    {
        log_file_.close();
    }
}

void TcpServer::Start()
{
    std::cout << "Server started on port " << port_ << std::endl;

    while (true)
    {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);

        int client_socket = accept(server_socket_, reinterpret_cast<sockaddr*>(&client_addr), &client_len);

        if (client_socket < 0)
        {
            std::cerr << "Error accepting client" << std::endl;
            continue;
        }

        std::cout << "Client connected" << std::endl;

        threads_.emplace_back(&TcpServer::HandleClient, this, client_socket);
        threads_.back().detach();
    }
}

void TcpServer::HandleClient(const int client_socket)
{
    while (true)
    {
        char buffer[256] = {};
        const auto n = read(client_socket, buffer, 255);

        if (n < 0)
        {
            std::cerr << "Error reading from socket" << std::endl;
            break;
        }

        if (n == 0)
        {
            std::cout << "Client disconnected" << std::endl;
            break;
        }

        std::lock_guard guard(log_mutex_);
        log_file_ << buffer << std::endl;
        log_file_.flush();
    }

    close(client_socket);
}

int main(const int argc, const char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: server <port>" << std::endl;
        return 1;
    }

    const int port = std::stoi(argv[1]);
    TcpServer server(port);
    server.Start();
}