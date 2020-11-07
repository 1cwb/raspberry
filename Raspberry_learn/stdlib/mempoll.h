#ifndef MEMPOLL_H
#define MEMPOLL_H
#include "common.h"
typedef struct poll_s poll_t;
typedef struct poll_large_s poll_large_t;
{
    UCHAR*     last;
    UCHAR*     end;
    poll_t*    next;
    INT32      faild;
}poll_data_t;

struct poll_s
{
    poll_data_t    d;
    size_t         max;
    poll_t*        current;
    poll_large_t*  large;
    //poll_cleanup_t* cleanup;
};

struct poll_large_s
{
    poll_large_t* next;
    VOID*         allco;
};
class Mempoll
{
public:

private:

};

#endif