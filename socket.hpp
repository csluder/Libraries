/*
Copyright (C) 2012 Charles E Sluder
Class for hashing and signing files
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

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
