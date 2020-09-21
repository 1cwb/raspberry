#ifndef SOCKET_H
#define SOCKET_H
#include <netinet/in.h>
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
private:

};


#endif