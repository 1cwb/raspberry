#include "msocket.h"
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <errno.h>
#include "common.h"
#include "clog.h"

namespace NetTool 
{
    UINT16 Chtons(UINT16 host16bitvalue)
    {
        return htons(host16bitvalue);
    }
    UINT32 Chtonl(UINT32 host32bitvalue)
    {
        return htonl(host32bitvalue);
    }
    UINT64  Chtonll(UINT64 host64bitvalue)
    {
        if(__BYTE_ORDER == __LITTLE_ENDIAN)
        {
            return (((UINT64)htonl((UINT32)(host64bitvalue & 0x00000000FFFFFFFFULL)) << 32) | ((UINT64)htonl((UINT32)(host64bitvalue >> 32)))); 
        }
        return host64bitvalue;
    }
    UINT16 Cntohs(UINT16 net16bitvalue)
    {
        return ntohs(net16bitvalue);
    }
    UINT32 Cntohl(UINT32 net32bitvalue)
    {
        return ntohl(net32bitvalue);
    }
    UINT64  Cntohll(UINT64 net64bitvalue)
    {
        if(__BYTE_ORDER == __LITTLE_ENDIAN)
        {
            return ((((UINT64)htonl((UINT32)(net64bitvalue & 0x00000000FFFFFFFFULL))) << 32) | ((UINT64)htonl((UINT32)(net64bitvalue >> 32)))); 
        }
        return net64bitvalue;
    }
    INT32  CinetPton(INT32 family, const char* strptr, VOID* addrptr)
    {
        return inet_pton(family, strptr, addrptr);
    }
    const CHAR* CinetNtop(INT32 family, const VOID* addrptr, CHAR* strptr, size_t len)
    {
        return inet_ntop(family, addrptr, strptr, len);
    }
    INT32 Cgetsockname(INT32 sockfd, struct sockaddr *localaddr, socklen_t *addrlen)
    {
        return getsockname(sockfd, localaddr, addrlen);
    }
    INT32 Cgetpeername(INT32 sockfd, struct sockaddr *peeraddr, socklen_t *addrlen)
    {
        return getpeername(sockfd, peeraddr, addrlen);
    }
    INT32 Cshutdown(INT32 sockfd, INT32 howto /*SHUT_RD/SHUT_WR/SHUT_RDWR*/)
    {
        return shutdown(sockfd, howto);
    }
    INT32 Cgetsockopt(INT32 sockfd, INT32 level, INT32 optname, VOID* optval, socklen_t* optlen)
    {
        return getsockopt(sockfd, level, optname, optval, optlen);
    }
    INT32 Csetsockopt(INT32 sockfd, INT32 level, INT32 optname, const VOID* optval, socklen_t optlen)
    {
        return setsockopt(sockfd, level, optname, optval, optlen);
    }
    bool Csetfdblock(INT32 sockfd, const bool block)
    {
        INT32 flags;
        if(block)
        {
            if((flags = Cfcntl(sockfd, F_GETFL, 0)) < 0)
            {
                return false;
            }
            flags &= ~O_NONBLOCK;
            if(Cfcntl(sockfd, F_SETFL,flags) < 0)
            {
                return false;
            }
        }
        else
        {
            if((flags = Cfcntl(sockfd, F_GETFL, 0)) < 0)
            {
                return false;
            }
            flags |= O_NONBLOCK;
            if(Cfcntl(sockfd, F_SETFL,flags) < 0)
            {
                return false;
            }
        }
        return true;
    }
    bool CsetTCPSockNodelay(INT32 sockfd, const bool nodelay)
    {
        INT32 flags = nodelay ? 1:0;
        if(Csetsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &flags, sizeof(flags)) < 0)
        {
            return false;
        }
        return true;
    }
    bool CsetReuseAddr(INT32 sockfd, const bool reuseaddr)
    {
        INT32 flags = reuseaddr ? 1:0;
        if(Csetsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flags, sizeof(flags)) < 0)
        {
            return false;
        }
        return true;
    }
    bool CsetReusePort(INT32 sockfd, const bool reuseport)
    {
        INT32 flags = reuseport ? 1:0;
        if(Csetsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &flags, sizeof(flags)) < 0)
        {
            return false;
        }
        return true;        
    }
    INT32 CgetSockFamily(INT32 sockfd)
    {
        struct sockaddr_storage addr;
        socklen_t addrlen = sizeof(struct sockaddr_storage);
        if(Cgetsockname(sockfd, (struct sockaddr*)&addr, &addrlen) < 0)
        {
            return -1;
        }
        return addr.ss_family;
    }
    INT32 Cgetaddrinfo(const CHAR* hostname, const char* service, const struct addrinfo *hints, struct addrinfo **result)
    {
        return getaddrinfo(hostname, service, hints, result);
    }
    VOID Cfreeaddrinfo(struct addrinfo *ai)
    {
        return freeaddrinfo(ai);
    }
    VOID getAddrTest()
    {
        struct addrinfo hints;
        struct addrinfo *result = NULL, *ptr = NULL;
        struct sockaddr_in* ipv4addr;
        struct sockaddr_in6* ipv6addr;
        //socklen_t addrlen;
        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_UNSPEC;
        hints.ai_flags = AI_ALL;
        CHAR strbuff[32];
        if(Cgetaddrinfo("www.google.com","http",&hints,&result) < 0)
        {
            printf("getaddrinfo failer! %s\n",Cstrerror(errno));
            return;
        }
        printf ("socket test=============start=============\n");
        for(ptr = result; ptr != NULL; ptr = ptr->ai_next)
        {
            memset(strbuff, 0, 32);
            if(ptr->ai_family ==AF_INET)
            {
                ipv4addr = (struct sockaddr_in*)ptr->ai_addr;
                printf("family is %d\n",ptr->ai_family);
                const CHAR *IP = CinetNtop(ptr->ai_family,&(ipv4addr->sin_addr),strbuff,32);
                if(IP == NULL)
                {
                    printf("error\n");
                }
                printf("IP is %s, port is %d\n", IP, Cntohs(ipv4addr->sin_port));
            }
            if(ptr->ai_family ==AF_INET6)
            {
                ipv6addr = (struct sockaddr_in6*)ptr->ai_addr;
                printf("family is %d\n",ptr->ai_family);
                const CHAR *IP = CinetNtop(ptr->ai_family,&(ipv6addr->sin6_addr),strbuff,32);
                if(IP == NULL)
                {
                    printf("error\n");
                }
                printf("IP is %s, port is %d\n", IP, Cntohs(ipv6addr->sin6_port));
            }
        }
        printf("socket test==========end================\n");
        Cfreeaddrinfo(result);
    }
    ssize_t Crecvfrom(INT32 sockfd, VOID* buff, size_t nbytes, INT32 flags, struct sockaddr *from, socklen_t* addrlen)
    {
        return recvfrom(sockfd, buff, nbytes, flags, from, addrlen);
    }
    ssize_t Csendto(INT32 sockfd, const VOID* buff, size_t nbytes, INT32 flags, const struct sockaddr* to, socklen_t addrlen)
    {
        return sendto(sockfd, buff, nbytes, flags, to, addrlen);
    }
}
/*for socket*/
Socket::Socket() : sockfd(-1)
{

}
Socket::~Socket()
{
    CcloseSockfd();
}

INT32 Socket::CreateSocket(INT32 family, INT32 type, INT32 protocol)
{
    sockfd = socket(family, type, protocol);
    return sockfd;
}
INT32 Socket::Cconnect(const struct sockaddr* servaddr, socklen_t addrlen)
{
    return connect(sockfd ,servaddr, addrlen);
}
INT32 Socket::Cbind(const struct sockaddr* myaddr, socklen_t addrlen)
{
    return bind(sockfd, myaddr, addrlen);
}
INT32 Socket::Clisten(INT32 backlog)
{
    return listen(sockfd, backlog);
}
INT32 Socket::Caccept(struct sockaddr* cliaddr, socklen_t *addrlen)
{
    return accept(sockfd, cliaddr, addrlen);
}
INT32 Socket::getsockFD()
{
    return sockfd;
}
INT32 Socket::CcloseSockfd()
{
    if(sockfd >= 0)
    {
        close(sockfd);
    }
    sockfd = -1;
    return 0;
}

//

NetServer::NetServer(INT32 family, INT32 type, INT32 protocol)
{
    memset(&ipv4addr, 0, sizeof(struct sockaddr_in));
    memset(&ipv6addr, 0, sizeof(struct sockaddr_in6));
    if(family == AF_INET)
    {
        addrlen = sizeof(struct sockaddr_in);
    }
    else if(family == AF_INET6)
    {
        addrlen = sizeof(struct sockaddr_in6);
    }
    ServerFamily = family;
    CreateSocket(family, type, protocol);
    servaddr = NULL;
}
NetServer::~NetServer()
{
    servaddr = NULL;
}
bool NetServer::initAddr(UINT16 port, const CHAR* addr)
{
    if(ServerFamily == AF_INET)
    {
        ipv4addr.sin_family = ServerFamily;
        ipv4addr.sin_port = NetTool::Chtons(port);
        if(addr == NULL)
        {
            ipv4addr.sin_addr.s_addr = NetTool::Cntohl(INADDR_ANY);
        }
        else
        {
            if(NetTool::CinetPton(ServerFamily, addr, &ipv4addr.sin_addr) < 0)
            {
                return false;
            }
        }
        servaddr = (struct sockaddr*)&ipv4addr;
    }
    else if(ServerFamily == AF_INET6)
    {
        ipv6addr.sin6_family = ServerFamily;
        ipv6addr.sin6_port = NetTool::Cntohs(port);
        if(addr == NULL)
        {
            ipv6addr.sin6_addr = in6addr_any;
        }
        else
        {
            if(NetTool::CinetPton(ServerFamily, addr, &ipv6addr.sin6_addr) < 0)
            {
                return false;
            }
        }
        servaddr = (struct sockaddr*)&ipv6addr;
    }
    else
    {
        return false;
    }
    if(Cbind(servaddr, addrlen) < 0)
    {
        return false;
    }
    return true;
}
bool NetServer::start()
{
    if(Clisten(128) < 0)
    {
        return false;
    }
    return true;
}
INT32 NetServer::CgetFamily()
{
    return ServerFamily;
}



NetClient::NetClient(const CHAR* hostName, const CHAR* port)
{
    memset(strhostname, 0, HOSTNAMELEN);
    memset(strport, 0, PORTLEN);
    memset(strcanonname, 0, HOSTNAMELEN);
    memset(&ipaddr, 0, sizeof(struct sockaddr_storage));

    memcpy(strhostname, hostName, HOSTNAMELEN);
    if(port == NULL)
    {
        memcpy(strport, "0", 2);
    }
    else
    {
        memcpy(strport, port, PORTLEN);
    }
}
NetClient::~NetClient()
{
}
bool NetClient::initAddr(INT32 family, INT32 type, INT32 protocol, INT32 flags)
{
    struct addrinfo hints;
    struct addrinfo *result, *ptr;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = family;
    hints.ai_socktype = type;
    hints.ai_protocol = protocol;
    hints.ai_flags = flags;

    ClientFamily = family;
    ClientType = type;
    ClientProtocol = protocol;
    if(NetTool::Cgetaddrinfo(strhostname, strport, &hints, &result) < 0)
    {
        LOG_ERROR("Cgetaddrinfo failer: %s",Cstrerror(errno));
        return false;
    }

    for(ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {
        if(CreateSocket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol) < 0)
        {
            continue;
        }
        else
        {
            memcpy(&ipaddr, ptr->ai_addr, ptr->ai_addrlen);
            memcpy(strcanonname, ptr->ai_canonname, HOSTNAMELEN);
            addrlen = ptr->ai_addrlen;
            if(ptr->ai_family == AF_INET)
            {
                struct sockaddr_in *pp = (struct sockaddr_in *)&ipaddr;
            CHAR strbuff[32] = {0};
            LOG_INFO("--------------------------------------------");
            LOG_INFO("ip is %s, port is %d",NetTool::CinetNtop(ptr->ai_family,&(pp->sin_addr),strbuff,32),
                        NetTool::Cntohs(pp->sin_port));
            LOG_INFO("--------------------------------------------");
            }
            else
            {
                                struct sockaddr_in6 *pp = (struct sockaddr_in6 *)&ipaddr;
            CHAR strbuff[32] = {0};
            LOG_INFO("--------------------------------------------");
            LOG_INFO("ip is %s, port is %d",NetTool::CinetNtop(ptr->ai_family,&(pp->sin6_addr),strbuff,32),
                        NetTool::Cntohs(pp->sin6_port));
            LOG_INFO("--------------------------------------------");
            }
            
            break;
        }
    }
    if(ptr == NULL)
    {
        LOG_ERROR("All ip can not create sockfd!");
        NetTool::Cfreeaddrinfo(result);
        return false;
    }
    NetTool::Cfreeaddrinfo(result);
    return true;
}
bool NetClient::startConnect()
{
    if(Cconnect((struct sockaddr*)&ipaddr, addrlen) < 0)
    {
        return false;
    }
    return true;
}
INT32 NetClient::CgetFamily()
{
    return ClientFamily;
}
