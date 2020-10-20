#ifndef CSELECT_H
#define CSELECT_H
#include <sys/select.h>
#include "common.h"
typedef enum
{
READ_FD_EM,
WRITE_FD_EM,
EXCEP_FD_EM
}FD_TYPE;

class Cselect
{
public:
    Cselect();
    ~Cselect();
    bool fdIsSet(INT32 fd, FD_TYPE type);
    VOID fdClear(INT32 fd, FD_TYPE type);
    VOID fdSet(INT32 fd, FD_TYPE type);
    VOID fdZero(FD_TYPE type);
    VOID fdZeroAll();
    INT32 selectfd(INT32 maxfdpl, struct timeval* tvptr);
private:
    fd_set readfds;
    fd_set writefds;
    fd_set exceptfds;
};
#endif