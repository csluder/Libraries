#include <cstdio>
#include <stdint.h>
#include <cerrno>
#ifdef _WIN32
#include <Winsock2.h>
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#include <poll.h>
#endif
#include <cstring>
#include <csignal>
#include <fcntl.h>
#include <system_error>

#include "socket.hpp"

Socket::Socket(bool isIpv6, int type) : IPAddress(isIpv6)
{
    if ((m_sockfd = socket((isIpv6)?AF_INET6:AF_INET, type, 0)) < 0)
    {
	throw std::system_error(errno, std::system_category());
	return;
    }
}

Socket::~Socket()
{
    closesocket(m_sockfd);
}

int
Socket::Connect(const char *ipAddr, int port)
{
    int rc;

    SetAddress(ipAddr);
    SetPortNumber(port);

    if ( (rc = connect(m_sockfd, this->m_pIpAddr, SizeOf())) < 0)
    {
	throw std::system_error(errno, std::system_category());
	return errno;
    }
    return rc;
}

int
Socket::Bind(const char *ipAddr, int port)
{
    int rc;

    SetAddress(ipAddr);
    SetPortNumber(port);

    if ( (rc = bind(m_sockfd, this->m_pIpAddr, SizeOf())) < 0 )
    {
	throw std::system_error(errno, std::system_category());
	return errno;
    }

    return rc;
}

int
Socket::Bind(int port)
{
    int rc;

    SetIPAddressAny();
    SetPortNumber(port);

    if ( (rc = bind(m_sockfd, this->m_pIpAddr, SizeOf())) < 0 )
    {
	throw std::system_error(errno, std::system_category());
	return errno;
    }

    return rc;
}

int
Socket::Listen(int backlog)
{
    int rc;

    if ( (rc = listen(m_sockfd, backlog)) < 0 )
    {
	throw std::system_error(errno, std::system_category());
	return errno;
    }

    return rc;
}


int
Socket::Accept(Socket &remoteHost)
{
    sockaddr *saRemote = remoteHost.m_pIpAddr;
    socklen_t len = remoteHost.SizeOf();

    close(remoteHost.m_sockfd);
    if ((remoteHost.m_sockfd = accept(m_sockfd, saRemote, &len)) < 0)
    {
	throw std::system_error(errno, std::system_category());
	return errno;
    }

    return remoteHost.m_sockfd;
}

int
Socket::Recv(void *pBuffer, int len, unsigned int flags)
{
    int bytes;

    if ( ( bytes = recv(m_sockfd, pBuffer, len, flags) ) < 0 )
    {
	throw std::system_error(errno, std::system_category());
	return errno;
    }
    return bytes;
}

int
Socket::Recv(void *pBuffer, int len, unsigned int flags, int timeout)
{
    int bytes;
    struct pollfd fds[2];
    int nfds = 1;

   memset(fds, 0 , sizeof(fds));
   fds[ 0 ].fd = m_sockfd;
   fds[ 0 ].events = POLLIN;

    int rc = poll( fds, nfds, timeout );
    if ( ( rc <= 0 ) || ( fds[0].revents != POLLIN ) ) return rc;

    if ( ( bytes = recv(m_sockfd, pBuffer, len, flags) ) < 0 )
    {
	throw std::system_error(errno, std::system_category());
	return errno;
    }
    return bytes;
}

int
Socket::RecvFrom(void *buff, int len, uint32_t flags, Socket &client)
{
    int             bytes;
    socklen_t saLen = client.SizeOf();

    if ( ( bytes = recvfrom(m_sockfd, buff, len, flags, client, &saLen) ) < 0 )
    {
	throw std::system_error(errno, std::system_category());
	return errno;
    }
    return bytes;
}

int
Socket::RecvFrom(void *buff, int len, uint32_t flags, Socket &client, int timeout)
{
    int             bytes;
    socklen_t saLen = client.SizeOf();
    struct pollfd fds[2];
    int nfds = 1;

   memset(fds, 0 , sizeof(fds));
   fds[ 0 ].fd = m_sockfd;
   fds[ 0 ].events = POLLIN;

    int rc = poll( fds, nfds, timeout );
    if ( ( rc <= 0 ) || ( fds[0].revents != POLLIN ) ) return rc;

    if ( ( bytes = recvfrom(m_sockfd, buff, len, flags, client, &saLen) ) < 0 )
    {
	throw std::system_error(errno, std::system_category());
	return errno;
    }
    return bytes;
}

int
Socket::Send(const void *buffer, int len, uint32_t flags)
{
    int bytes;

    if ( ( bytes = send(m_sockfd, buffer, len, flags) ) < 0 )
    {
	throw std::system_error(errno, std::system_category());
	return errno;
    }

    return bytes;
}

int
Socket::SendTo(const void *buffer, int len, uint32_t flags, Socket &client)
{
    int bytes;
static int cnt = 0;
    socklen_t saLen = client.SizeOf();
if ( saLen == 0 ) {
printf("Bad socket length\n");
}

/*
struct sockaddr* sa = client;
printf("Count = %d buffer %p len %d fd %d\n", ++cnt, buffer, len, m_sockfd );
printf("0x%x  %d ", sa->sa_family, saLen );
for (int i = 0; i < saLen - 2; i++ )
{
printf("%x", sa->sa_data[i]);
}
for (int i = 0; i < 40 - 2; i++ )
{
printf("%x", ((unsigned char*)buffer)[i]);
}
printf("\n");
*/
    if ( ( bytes = sendto(m_sockfd, buffer, len, flags, client, saLen) ) < 0 )
    {
	throw std::system_error(errno, std::system_category());
	return errno;
    }

    return bytes;
}

int
Socket::GetSockName(const char* &ipAddr, int &port)
{
    IPAddress addr;
    //SocketAddress   addr;
    socklen_t       len = addr.SizeOf();


    if (getsockname(m_sockfd, (sockaddr *)addr, &len) < 0)
    {
	throw std::system_error(errno, std::system_category());
	return errno;
    }

    ipAddr = addr.GetAddress();
    addr.GetPortNumber(port);
    return 0;
}

#ifdef notyet
void
Socket::GetPeerName(IPAddress &cRemoteAddr, int &iRemotePort)
{
    SocketAddress   cRemote;
    socklen_t       slDummy = cRemote.SizeOf();

    WaitMutex();

    if (getpeername(m_sockfd, (sockaddr *)cRemote, &slDummy) < 0)
    {
        ClearMutex();
        switch (errno)
        {
            case EBADF:
            case ENOTSOCK:  throw SocketException(SocketException::errBadDescriptor);
            case ENOBUFS:   throw SocketException(SocketException::errKernelMemory);
            case EFAULT:    throw SocketException(SocketException::errIllegalPointer);
            default:        throw SocketException(SocketException::errUnknown, errno);
        }
    }
    ClearMutex();

    cRemote.GetIPAddress(cRemoteAddr);
    cRemote.GetPortNumber(iRemotePort);
}
#endif

int
Socket::GetSockOpt(int level, int optName, void *optVal, socklen_t *optLen)
{
    int rc;

    if ( (rc = getsockopt(m_sockfd, level, optName, optVal, optLen)) < 0 )
    {
	throw std::system_error(errno, std::system_category());
	return errno;
    }

    return rc;
}

int
Socket::SetSockOpt(int level, int optName, const void *optVal, socklen_t optLen)
{
    int rc;

    if ( (rc = setsockopt(m_sockfd, level, optName, optVal, optLen)) < 0 )
    {
	throw std::system_error(errno, std::system_category());
	return errno;
    }

    return rc;
}

int
Socket::Fcntl(int cmd, int arg)
{
    int rc;

    if ( (rc = fcntl(m_sockfd, cmd, arg)) < 0 )
    {
	throw std::system_error(errno, std::system_category());
	return errno;
    }

    return rc;
}
