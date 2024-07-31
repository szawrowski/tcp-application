#ifndef SERVER_H_
#define SERVER_H_

#include <fstream>
#include <thread>
#include <vector>
#include <mutex>

class TcpServer
{
public:
    TcpServer() = delete;
    explicit TcpServer(int port);

    ~TcpServer();

public:
    void Start();

private:
    void HandleClient(int client_socket);

private:
    int server_socket_;
    int port_;
    std::vector<std::thread> threads_;
    std::ofstream log_file_;
    std::mutex log_mutex_;
};

#endif // SERVER_H_
