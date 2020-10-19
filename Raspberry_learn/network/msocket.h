#ifndef MSOCKET_H
#define MSOCKET_H
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include "common.h"
namespace NetTool
{
    UINT16 Chtons(UINT16 host16bitvalue);
    UINT32 Chtonl(UINT32 host32bitvalue);
    UINT64  Chtonll(UINT64 host64bitvalue);
    UINT16 Cntohs(UINT16 net16bitvalue);
    UINT32 Cntohl(UINT32 net32bitvalue);
    UINT64  Cntohll(UINT64 net64bitvalue);
    INT32  CinetPton(INT32 family, const char* strptr, VOID* addrptr);
    const CHAR* CinetNtop(INT32 family, const VOID* addrptr, CHAR* strptr, size_t len);
    INT32 Cgetsockname(INT32 sockfd, struct sockaddr *localaddr, socklen_t *addrlen);
    INT32 Cgetpeername(INT32 sockfd, struct sockaddr *peeraddr, socklen_t *addrlen);
    INT32 Cshutdown(INT32 sockfd, INT32 howto /*SHUT_RD/SHUT_WR/SHUT_RDWR*/);
    INT32 Cgetsockopt(INT32 sockfd, INT32 level, INT32 optname, VOID* optval, socklen_t* optlen);
    INT32 Csetsockopt(INT32 sockfd, INT32 level, INT32 optname, const VOID* optval, socklen_t optlen);
    bool Csetfdblock(INT32 sockfd, const bool block);
    bool CsetTCPSockNodelay(INT32 sockfd, const bool nodelay);//just for tcp
    bool CsetReuseAddr(INT32 sockfd, const bool reuseaddr);
    bool CsetReusePort(INT32 sockfd, const bool reuseport);
    INT32 CgetSockFamily(INT32 sockfd);
/*
    struct addrinfo
    {
    int ai_flags;			//Input flags.  
    int ai_family;		//Protocol family for socket.  
    int ai_socktype;		/Socket type.  
    int ai_protocol;		// Protocol for socket.  
    socklen_t ai_addrlen;		//Length of socket address.  
    struct sockaddr *ai_addr;	//Socket address for socket.  
    char *ai_canonname;		// Canonical name for service location.  
    struct addrinfo *ai_next;	// Pointer to next in list.  
    };
    hostname is www.xxx 
    service is port,if service not be NULL && ai_flags = AI_NUMERICSERV, hostname must be x.x.x.x/ipv6 addr
    hints just use ai_flags ai_family ai_socktype ai_protocol,hints can be NULL
        ai_family:AF_INET/AF_INET6/AF_UNSPEC(ipv4 and ipv6)
        ai_socktype: SOCK_STREAM/SOCK_DGRAME/SOCK_RAW（0 means can use all type）
        ai_protocol: IPPROTO_TCP IPPROTO_UDP （0 means can use all type）
        ai_flags: AI_PASSIVE（ifset it, and hostname is NULL, return sockaddr can be used by bind, return INADDR_ANY/IN6ADDR_ANY_INIT）
                  if hostname not NULL, AI_PASSIVE is invalid, if not set AI_PASSIVE, return sockaddr can be used in connect/sendto/sendmsg
                  AI_CANNONNAME if it be set, will return point in ai_canonname
                  AI_NUMERICHOST if set,hostname must be x.x.x.x/ipv6 addr
                  AI_NUMERICSERV if set,port must be num like "8080"
                  AI_V4MAPPED if set AF_INET6,ad it is be set, it will return a address ipv4 mapped ipv6
                  AI_ALL find ipv4 and ipv6 address 
                  AI_ADDRCONFIG if set ipv4, find ipv4 addr, if set ipv6, find ipv6 addr

*/
    INT32 Cgetaddrinfo(const CHAR* hostname, const char* service, const struct addrinfo *hints, struct addrinfo **result);
    VOID Cfreeaddrinfo(struct addrinfo *ai);
    VOID getAddrTest();
    //for udp
    ssize_t Crecvfrom(INT32 sockfd, VOID* buff, size_t nbytes, INT32 flags, struct sockaddr *from, socklen_t* addrlen);
    ssize_t Csendto(INT32 sockfd, const VOID* buff, size_t nbytes, INT32 flags, const struct sockaddr* to, socklen_t addrlen);
}
/*
FAMILY:AF_INET AF_INET6 AF_LOCAL AF_ROUTE AF_KEY
TYPE:SOCK_STREAM SOCK_DGRAM SOCK_SEQPACKET SOCK_RAW
PROTOCOL:IPPROTO_TCP IPPROTO_UDP IPPROTO_SCTP
*/
//INADDR_ANY
//<netinet/in.h> in6addr_any
//bind return fail: EADDRINUSE
class Socket
{
public:
    Socket();
    ~Socket();
    INT32 CreateSocket(INT32 family, INT32 type, INT32 protocol);
    INT32 Cconnect(const struct sockaddr* servaddr, socklen_t addrlen);
    INT32 Cbind(const struct sockaddr* myaddr, socklen_t addrlen);
    INT32 Clisten( INT32 backlog);
    INT32 Caccept(struct sockaddr* cliaddr, socklen_t *addrlen);
    INT32 getsockFD();
    INT32 CcloseSockfd();
private:
    INT32 sockfd;
};

class NetServer : public Socket
{
public:
    NetServer(INT32 family = AF_INET, INT32 type = SOCK_STREAM, INT32 protocol = IPPROTO_TCP);
    ~NetServer();
    bool initAddr(UINT16 port, const CHAR* addr = NULL);
    bool start();
    INT32 CgetFamily();
private:
    INT32 ServerFamily;
    struct sockaddr* servaddr;
    struct sockaddr_in ipv4addr;
    struct sockaddr_in6 ipv6addr;
    socklen_t addrlen;
};

#define HOSTNAMELEN 128
#define PORTLEN      8

class NetClient : public Socket
{
public:
    NetClient(const CHAR* hostName, const CHAR* port);
    ~NetClient();
    bool initAddr(INT32 family = AF_UNSPEC, INT32 type = SOCK_STREAM, INT32 protocol = IPPROTO_IP, INT32 flags = AI_CANONNAME);
    bool startConnect();
    INT32 CgetFamily();
private:
    INT32 ClientFamily;
    INT32 ClientType;
    INT32 ClientProtocol;
    CHAR strhostname[HOSTNAMELEN];
    CHAR strport[PORTLEN];
    CHAR strcanonname[HOSTNAMELEN];
    struct sockaddr_storage ipaddr;
    socklen_t addrlen;
};
#endif