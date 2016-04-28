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

#ifndef IPADDR_HPP
#define IPADDR_HPP

#include <netdb.h>
#include <arpa/inet.h>
#include <string>

using namespace std;

/***
 * @class The class manages sockaddr structures for IPV6 and IPV4 address families.
 */
class SocketAddress
{
    public:
	/***
	 * Constructor for class. 
	 *
	 * @param[IN] isIpv6 - Indicates to the class whether it is managing ipv6 or ipv4
	 *                    structures.
	 */
                        SocketAddress(bool isIpv6 = false);

	/***
	 * Set the sockaddr structure to the wild card address.
	 */
	void	        SetIPAddressAny();

	/***
	 * Initialize the socaddr structure with the address in the in_addr structure.
	 *
	 * param[IN] ipAddr - in_addr structure containing an ip address
	 */
        void            SetIPAddress(const struct in6_addr &ipAddr);
	/***
	 * @overload
	 */
        void            SetIPAddress(const in_addr &ipAddr);
	/***
	 * @overload
	 */
        void            SetIPAddress(const in_addr_t &ipAddr);

	/***
	 * Returns the ip address associated with the class in passed argument.
	 *
	 * @param[OUT] ipAddr - in_addr structure that will contain the ip address
	 *                      associated with the class.
	 */
        void            GetIPAddress(struct in6_addr &ipAddr);

	/***
	 * @overload
	 */
        void            GetIPAddress(in_addr &ipAddr);

	/***
	 * Retun the port number associated with the class
	 *
	 * param[OUT] portNumber - Int to hold the returned port number
	 */
        void            GetPortNumber(int &portNumber);

	/***
	 * Assign a port number to the class.
	 *
	 * @param[IN] portNUmber - port number to be assigned to the class.
	 */
        void            SetPortNumber(int portNumber);

	/***
	 * Returns the address family associated with the class
	 *
	 * @return AF_INET6 or AF_INET
	 */
	int             GetAddrFamily();

        socklen_t       SizeOf();

	operator        sockaddr *() { return m_pIpAddr; }
        operator        sockaddr_in  *() { return m_pIpv4Addr; }
        operator        sockaddr_in6 *() { return m_pIpv6Addr; }

    protected:
        struct sockaddr_storage  m_ipAddr;
        struct sockaddr          *m_pIpAddr;
        struct sockaddr_in       *m_pIpv4Addr;
        struct sockaddr_in6      *m_pIpv6Addr;
};

/***
 * @class The class manages character IP addresses and converts them to and from
 *        binary sock_addr structures. The class inherits from the SocketAddress
 *        class
 */
class IPAddress:public SocketAddress
{
    public:
	/***
	 * Class constructor.
	 *
	 * @param[IN] isIpv6 - Indicates whether the class is managing ipv4 or ipv6
	 *                     addresses. The class is initialized to the loopback
	 *                     address for address family.
	 */
                      IPAddress(bool isIpv6 = false);
	/***
	 * Class deconstructor.
	 */
                      ~IPAddress() {}

	/***
	 * Takes a URI for a host and resolves it to the host IP address if possible.
	 *
	 * @param[IN] hostName - Pointer to a character string containing a URI.
	 */
	
        void          SetHostName(const char *hostName);
	/***
	 * Retrieves a previously stored hostname or resolves the hostname of a previously
	 * stored IP address.
	 */
        const char    *GetHostName();

	/***
	 * Saves the IP address and converts it to a binary sock_addr structure.
	 *
	 * @param[IN] ipAddr - pointer to a character string containing an IPV4 or IPV6 
	 *                     address in standard dot or colon notaion.
	 */
        void          SetAddress(const char *ipAddr);

	/***
	 * Returns a previously stored or resolved IP address.
	 * 
	 * @return A pointer to a character sting containg the IP address in standard
	 *         dot or colon notation.
	 */
        const char    *GetAddress();

    private:
        std::string   m_sHostName;
        std::string   m_cFormattedAddr;
};


#endif
