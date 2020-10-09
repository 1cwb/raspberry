#include "socket.h"
#include "unistd.h"
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
/*for TCP*/
Tcp::Tcp(INT32 family = AF_INET, INT32 type = SOCK_STREAM, INT32 protocol = IPPROTO_TCP)
{
    sockfd = socket(family, type, protocol);
}
Tcp::~Tcp()
{
    if(sockfd >= 0)
    {
        close(sockfd);
    }
    sockfd = -1;
}
/*
INT32 Tcp::Csocket(INT32 family, INT32 type, INT32 protocol)
{
    return socket(family, type, protocol);
}*/
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