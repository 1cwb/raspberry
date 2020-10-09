#ifndef MSOCKET_H
#define MSOCKET_H
#include <netinet/in.h>
#include <sys/socket.h>
#include "common.h"
class Socket
{
public:
    Socket();
    ~Socket();
    static UINT16 Chtons(UINT16 host16bitvalue);
    static UINT32 Chtonl(UINT32 host32bitvalue);
    static UINT64  Chtonll(UINT64 host64bitvalue);
    static UINT16 Cntohs(UINT16 net16bitvalue);
    static UINT32 Cntohl(UINT32 net32bitvalue);
    static UINT64  Cntohll(UINT64 net64bitvalue);
    static INT32  CinetPton(INT32 family, const char* strptr, VOID* addrptr);
    static const CHAR* CinetNtop(INT32 family, const VOID* addrptr, CHAR* strptr, size_t len);
private:

};
/*
FAMILY:AF_INET AF_INET6 AF_LOCAL AF_ROUTE AF_KEY
TYPE:SOCK_STREAM SOCK_DGRAM SOCK_SEQPACKET SOCK_RAW
PROTOCOL:IPPROTO_TCP IPPROTO_UDP IPPROTO_SCTP
*/
//INADDR_ANY
//<netinet/in.h> in6addr_any
//bind return fail: EADDRINUSE
class Tcp
{
public:
    Tcp();
    ~Tcp();
    INT32 CreateSocket(INT32 family = AF_INET, INT32 type = SOCK_STREAM, INT32 protocol = IPPROTO_TCP);
    INT32 Cconnect(const struct sockaddr* servaddr, socklen_t addrlen);
    INT32 Cbind(const struct sockaddr* myaddr, socklen_t addrlen);
    INT32 Clisten( INT32 backlog);
    INT32 Caccept(struct sockaddr* cliaddr, socklen_t *addrlen);
    INT32 getsockFD();
private:
    INT32 sockfd;
};
#endif

class TcpServer
{
public:
    TcpServer();
    ~TcpServer();
    VOID setFamily(INT32 family);
    VOID setAddr(UINT32 addr);
    VOID setAddr(const CHAR* addr);
    VOID setPort(UINT16 port);
    bool start();
    INT32 accept(struct sockaddr* cliaddr, socklen_t *addrlen);
    INT32 getSockfd();
private:
    Tcp tcpserver;
    socklen_t len;
    struct sockaddr_in servaddr;
};

class TcpClient
{
public:
    TcpClient();
    ~TcpClient();
    VOID setFamily(INT32 family);
    VOID setAddr(UINT32 addr);
    VOID setAddr(const CHAR* addr);
    VOID setPort(UINT16 port);
    bool startConnect();
    INT32 getSockfd();
private:
    Tcp tcpclient;
    socklen_t len;
    struct sockaddr_in servaddr;
};