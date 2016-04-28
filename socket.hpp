
#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "ipaddr.hpp"

class Socket : public IPAddress
{
public:
    Socket(bool isIPv6, int type);
    ~Socket();

    int Connect(const char *ipAddr, int port);
    int Bind(const char *ipAddr, int port);
    int Bind(int port);
    int Listen(int backlog);
    int Accept(Socket &remoteHost);

    int Recv(void *buff, int len, uint32_t flags);
    int Recv(void *buff, int len, uint32_t flags, int timeout);
    int RecvFrom(void *buff, int len, uint32_t flags, Socket &sock);
    int RecvFrom(void *buff, int len, uint32_t flags, Socket &sock, int timeout);
    int Send(const void *buff, int len, uint32_t flags);
    int SendTo(const void *buff, int len, uint32_t flags, Socket &sock);

    int GetSockOpt(int level, int optName, void *optVal, socklen_t *optLen);
    int	SetSockOpt(int level, int optName, const void *optVal, socklen_t optLen);

    int GetSockName(const char* &ipAddr, int &port);
    int Fcntl(int cmd, int arg);

protected:
    int m_sockfd;
};

#endif
