#include "socket.h"

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