#ifndef CLIENT_H_
#define CLIENT_H_

#include <string>

class TcpClient
{
public:
    TcpClient() = delete;
    TcpClient(std::string name, int port, int interval);

    ~TcpClient() = default;

public:
    void Start() const;

private:
    void SendMessage() const;

private:
    std::string name_;
    int port_;
    int interval_;
};

#endif // CLIENT_H_
