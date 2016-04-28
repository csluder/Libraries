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

#include <cstdio>
#include <cerrno>

#include <sys/socket.h>
#include <cstring>
#include <csignal>
#include <netdb.h>
#include <arpa/inet.h>


#include "ipaddr.hpp"

SocketAddress::SocketAddress(bool isIpv6)
{
    bzero(&m_ipAddr, sizeof(m_ipAddr));

    m_pIpAddr = (sockaddr *)&m_ipAddr;

    if ( isIpv6 ) {
        m_pIpv4Addr = (struct sockaddr_in *)NULL;
        m_pIpv6Addr = (struct sockaddr_in6*)&m_ipAddr;
        m_pIpv6Addr->sin6_addr   = IN6ADDR_LOOPBACK_INIT;
        m_pIpv6Addr->sin6_family = AF_INET6;
        m_pIpv6Addr->sin6_port = htons(0);
    } else {
        m_pIpv6Addr = NULL;
        m_pIpv4Addr = (struct sockaddr_in *)&m_ipAddr;
        m_pIpv4Addr->sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        m_pIpv4Addr->sin_family      = AF_INET;
        m_pIpv4Addr->sin_port = htons(0);
    }
}

void
SocketAddress::SetIPAddressAny()
{
    if (m_pIpAddr->sa_family == AF_INET6)
    {
        m_pIpv6Addr->sin6_addr   = IN6ADDR_ANY_INIT;
        m_pIpv6Addr->sin6_family = AF_INET6;
    } else {
        m_pIpv4Addr->sin_addr.s_addr = htonl(INADDR_ANY);
        m_pIpv4Addr->sin_family      = AF_INET;
    }
}

void
SocketAddress::SetIPAddress(const in6_addr &ipAddr)
{
    m_pIpv6Addr->sin6_addr = ipAddr;
}

void
SocketAddress::SetIPAddress(const in_addr &ipAddr)
{
    m_pIpv4Addr->sin_addr.s_addr = htonl(ipAddr.s_addr);
}

void
SocketAddress::SetIPAddress(const in_addr_t &ipAddr)
{
    m_pIpv4Addr->sin_addr.s_addr = htonl(ipAddr);
}

void
SocketAddress::GetIPAddress(in6_addr &ipAddr)
{
    ipAddr = m_pIpv6Addr->sin6_addr;
}

void
SocketAddress::GetIPAddress(in_addr &ipAddr)
{
    ipAddr.s_addr = ntohl(m_pIpv4Addr->sin_addr.s_addr);
}

void
SocketAddress::SetPortNumber(int portNumber)
{
    if (m_pIpAddr->sa_family == AF_INET6)
    {
        m_pIpv6Addr->sin6_port = htons(portNumber);
    } else {
        m_pIpv4Addr->sin_port = htons(portNumber);
    }
}

void
SocketAddress::GetPortNumber(int &portNumber)
{
    if (m_pIpAddr->sa_family == AF_INET6)
    {
        portNumber = ntohs(m_pIpv6Addr->sin6_port);
    } else {
        portNumber = ntohs(m_pIpv4Addr->sin_port);
    }
}

int
SocketAddress::GetAddrFamily()
{
    return (m_pIpAddr->sa_family);
}

socklen_t
SocketAddress::SizeOf()
{
    if (m_pIpAddr->sa_family == AF_INET6) {
      return(sizeof(struct sockaddr_in6));
    } else {
      return(sizeof(struct sockaddr_in));
    }
}

