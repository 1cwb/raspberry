#include "cmmap.h"
#include "clog.h"
Cmmap::Cmmap(INT32 fd, off_t off, VOID* addr, size_t len, INT32 port, INT32 flag)
{
    this->len = len;
    mapaddr = mmap(addr, len, port, flag, fd, off);
    if(mapaddr == MAP_FAILED)
    {
        LOG_ERROR("Create Cmaap class failed!");
        mapaddr = NULL;
    }
}
Cmmap::~Cmmap()
{
    if(mapaddr != NULL)
    {
        munmap(mapaddr, len);
    }
}
VOID* Cmmap::getMapAddr()
{
    return mapaddr;
}
INT32 Cmmap::Cmprotect(INT32 port)
{
    return mprotect(mapaddr, len, port);
}
INT32 Cmmap::Cmsync(INT32 flags)
{
    return msync(mapaddr, len, flags);
}
