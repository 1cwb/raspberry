#include "msocket.h"
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
Socket::Socket()
{

}
Socket::~Socket()
{

}
UINT16 Socket::Chtons(UINT16 host16bitvalue)
{
    return htons(host16bitvalue);
}
UINT32 Socket::Chtonl(UINT32 host32bitvalue)
{
    return htonl(host32bitvalue);
}
UINT64  Socket::Chtonll(UINT64 host64bitvalue)
{
    if(__BYTE_ORDER == __LITTLE_ENDIAN)
    {
        return (((UINT64)htonl((UINT32)(host64bitvalue & 0x00000000FFFFFFFFULL)) << 32) | ((UINT64)htonl((UINT32)(host64bitvalue >> 32)))); 
    }
    return host64bitvalue;
}
UINT16 Socket::Cntohs(UINT16 net16bitvalue)
{
    return ntohs(net16bitvalue);
}
UINT32 Socket::Cntohl(UINT32 net32bitvalue)
{
    return ntohl(net32bitvalue);
}
UINT64  Socket::Cntohll(UINT64 net64bitvalue)
{
    if(__BYTE_ORDER == __LITTLE_ENDIAN)
    {
        return ((((UINT64)htonl((UINT32)(net64bitvalue & 0x00000000FFFFFFFFULL))) << 32) | ((UINT64)htonl((UINT32)(net64bitvalue >> 32)))); 
    }
    return net64bitvalue;
}
INT32  Socket::CinetPton(INT32 family, const char* strptr, VOID* addrptr)
{
    return inet_pton(family, strptr, addrptr);
}
const CHAR* Socket::CinetNtop(INT32 family, const VOID* addrptr, CHAR* strptr, size_t len)
{
    return inet_ntop(family, addrptr, strptr, len);
}
/*for TCP*/
Tcp::Tcp() : sockfd(-1)
{

}
Tcp::~Tcp()
{
    if(sockfd >= 0)
    {
        close(sockfd);
    }
    sockfd = -1;
}

INT32 Tcp::CreateSocket(INT32 family, INT32 type, INT32 protocol)
{
    sockfd = socket(family, type, protocol);
    return sockfd;
}
INT32 Tcp::Cconnect(const struct sockaddr* servaddr, socklen_t addrlen)
{
    return connect(sockfd ,servaddr, addrlen);
}
INT32 Tcp::Cbind(const struct sockaddr* myaddr, socklen_t addrlen)
{
    return bind(sockfd, myaddr, addrlen);
}
INT32 Tcp::Clisten(INT32 backlog)
{
    return listen(sockfd, backlog);
}
INT32 Tcp::Caccept(struct sockaddr* cliaddr, socklen_t *addrlen)
{
    return accept(sockfd, cliaddr, addrlen);
}
INT32 Tcp::getsockFD()
{
    return sockfd;
}

/*tcp server*/
TcpServer::TcpServer()
{
    len = sizeof(struct sockaddr_in);
    memset(&servaddr, 0, len);

}
TcpServer::~TcpServer()
{

}
VOID TcpServer::setFamily(INT32 family)
{
    servaddr.sin_family = family;
}
VOID TcpServer::setAddr(UINT32 addr)
{
    servaddr.sin_addr.s_addr = Socket::Chtonl(addr);
}
VOID TcpServer::setAddr(const CHAR* addr)
{
    Socket::CinetPton(AF_INET, addr, &servaddr.sin_addr);
}
VOID TcpServer::setPort(UINT16 port)
{
    servaddr.sin_port = Socket::Chtons(port);
}
bool TcpServer::start()
{
    if(tcpserver.CreateSocket() < 0)
    {
        return false;
    }
    if(tcpserver.Cbind((struct sockaddr*)&servaddr, len) < 0)
    {
        return false;
    }
    if(tcpserver.Clisten(128) < 0)
    {
        return false;
    }
    return true;
}
INT32 TcpServer::accept(struct sockaddr* cliaddr, socklen_t *addrlen)
{
    return tcpserver.Caccept(cliaddr, addrlen);
}
INT32 TcpServer::getSockfd()
{
    return tcpserver.getsockFD();
}
/*for client*/
TcpClient::TcpClient()
{
    len = sizeof(struct sockaddr_in);
    memset(&servaddr, 0, len);
}
TcpClient::~TcpClient()
{

}
VOID TcpClient::setFamily(INT32 family)
{
    servaddr.sin_family = family;
}
VOID TcpClient::setAddr(UINT32 addr)
{
    servaddr.sin_addr.s_addr = Socket::Chtonl(addr);
}
VOID TcpClient::setAddr(const CHAR* addr)
{
    Socket::CinetPton(AF_INET, addr, &servaddr.sin_addr);
}
VOID TcpClient::setPort(UINT16 port)
{
    servaddr.sin_port = Socket::Chtons(port);
}
bool TcpClient::startConnect()
{
    if(tcpclient.CreateSocket() < 0)
    {
        return false;
    }
    if(tcpclient.Cconnect((const struct sockaddr*)&servaddr, len) < 0)
    {
        return false;
    }
    return true;
}
INT32 TcpClient::getSockfd()
{
    return tcpclient.getsockFD();
}
//Tcp tcpclient;
//socklen_t len;
//struct sockaddr_in servaddr;
