#include "client.h"

#include <iostream>
#include <fstream>
#include <thread>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <utility>

#include <netinet/in.h>
#include <unistd.h>

#include <arpa/inet.h>

TcpClient::TcpClient(std::string name, const int port, const int interval)
    : name_(std::move(name)), port_(port), interval_(interval)
{
}

void TcpClient::Start() const
{
    while (true)
    {
        SendMessage();
        std::this_thread::sleep_for(std::chrono::seconds(interval_));
    }
}

void TcpClient::SendMessage() const
{
    const int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
    {
        std::cerr << "Error opening socket" << std::endl;
        return;
    }

    sockaddr_in serv_addr{};
    std::memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(port_);

    if (connect(sockfd, reinterpret_cast<sockaddr*>(&serv_addr), sizeof(serv_addr)) < 0)
    {
        std::cerr << "Error connecting to server" << std::endl;
        close(sockfd);
        return;
    }

    const auto now = std::chrono::system_clock::now();
    const auto in_time = std::chrono::system_clock::to_time_t(now);
    const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    const auto time_format = "%Y-%m-%d %H:%M:%S";

    std::stringstream ss;

    ss << '[' << std::put_time(std::localtime(&in_time), time_format)
       << '.' << std::setw(3) << std::setfill('0') << millis.count() << "] " << name_;

    const std::string message = ss.str();
    const auto n = write(sockfd, message.c_str(), message.length());

    if (n < 0)
    {
        std::cerr << "Error writing to socket" << std::endl;
    }
    else
    {
        std::cout << "Sent message: " << message << std::endl;
    }

    close(sockfd);
}

int main(const int argc, const char** argv)
{
    if (argc != 4)
    {
        std::cerr << "Usage: client <name> <port> <interval>" << std::endl;
        return 1;
    }

    const std::string name = argv[1];
    const int port = std::stoi(argv[2]);
    const int interval = std::stoi(argv[3]);

    const TcpClient client(name, port, interval);
    client.Start();
}