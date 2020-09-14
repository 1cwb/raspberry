#ifndef CMMAP_H
#define CMMAP_H
#include <sys/mman.h>
#include "common.h"

class Cmmap
{
public:
    Cmmap(INT32 fd, off_t off, VOID* addr, size_t len, INT32 port = PROT_READ | PROT_WRITE, INT32 flag = MAP_SHARED);
    ~Cmmap();
    VOID* getMapAddr();
    INT32 Cmprotect(INT32 port);//PROT_READ PROT_WRITE PORT_EXEC PORT_NONE
    INT32 Cmsync(INT32 flags = MS_ASYNC); //if need wait sync before return,you should set flags = MS_SYNC;
private:
    VOID* mapaddr;
    size_t len;
};

#endif