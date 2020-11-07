#ifndef MEMPOLL_H
#define MEMPOLL_H
#include "common.h"

#define POOL_ALIGMENT 16
#define MAX_ALLOC_FROM_POOL (4096 - 1) //page size
#define ALIGN_PTR(p, a)   \
    (UCHAR*) (((U_PTR)(p) + (U_PTR)(a - 1)) & ~((U_PTR)a - 1))

#define ALIGNMENT sizeof(U_PTR)

typedef struct pool_s pool_t;
typedef struct pool_large_s pool_large_t;
typedef struct
{
    UCHAR*     last;
    UCHAR*     end;
    pool_t*    next;
    INT32      faild;
}pool_data_t;

struct pool_s
{
    pool_data_t    d;
    size_t         max;
    pool_t*        current;
    pool_large_t*  large;
};

struct pool_large_s
{
    pool_large_t* next;
    VOID*         allco;
};
class Mempool
{
public:
    Mempool(size_t size);
    ~Mempool();
    bool canBeuse();
    bool resetPool();
    VOID* palloc(size_t size);
    VOID* pnalloc(size_t size);
    VOID* pcalloc(size_t size);
private:
    Mempool(const Mempool&);
    VOID operator=(Mempool&);
    VOID* memalign(size_t alignment, size_t size);
    pool_t *p;
};

#endif