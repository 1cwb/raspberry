#ifndef MSOCKET_H
#define MSOCKET_H
#include <netinet/in.h>
#include <sys/socket.h>
#include "common.h"
namespace NetTool
{
    bool CinitSockAddr(struct sockaddr_in* servaddr, INT32 family, INT32 addr, UINT16 port);
    bool CinitSockAddr(struct sockaddr_in* servaddr, INT32 family, CHAR* addr, UINT16 port);
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
    bool CsetfdUnblock(INT32 sockfd);
    bool Csetfdblock(INT32 sockfd);
    
    VOID testxx();
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

class TcpServer : public Socket
{
public:
    TcpServer(INT32 family = AF_INET, INT32 type = SOCK_STREAM, INT32 protocol = IPPROTO_TCP);
    ~TcpServer();
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

class TcpClient : public Socket
{
public:
    TcpClient(INT32 family = AF_INET, INT32 type = SOCK_STREAM, INT32 protocol = IPPROTO_TCP);
    ~TcpClient();
    bool initAddr(UINT16 port, const CHAR* addr);
    bool startConnect();
    INT32 CgetFamily();
private:
    INT32 ClientFamily;
    struct sockaddr* servaddr;
    struct sockaddr_in ipv4addr;
    struct sockaddr_in6 ipv6addr;
    socklen_t addrlen;
};

class Udp
{
public:
    Udp();
    ~Udp();
    ssize_t Crecvfrom(VOID* buff, size_t nbytes, INT32 flags, struct sockaddr *from, socklen_t* addrlen);
    ssize_t Csendto(const VOID* buff, size_t nbytes, INT32 flags, const struct sockaddr* to, socklen_t* addrlen);
private:
    INT32 sockfd;
};

#endif