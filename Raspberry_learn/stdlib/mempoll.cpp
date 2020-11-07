#include "mempoll.h"

Mempool::Mempool(size_t size) : p(NULL)
{
    p = (pool_t*)memalign(POOL_ALIGMENT, size);
    if(p == NULL)
    {
        return;
    }
    p->d.last = (UCHAR*)p + sizeof(pool_t);
    p->d.end  = (UCHAR*)p + size;
    p->d.next = NULL;
    p->d.faild = 0;
  
    size = size - sizeof(pool_t);
    p->max = (size < MAX_ALLOC_FROM_POOL ? size : MAX_ALLOC_FROM_POOL);
    p->current = p;
    p->large = NULL;
}
Mempool::~Mempool()
{
    pool_t *temp = NULL;
    pool_t *n = NULL;
    pool_large_t* large = NULL;
    for(large = p->large; large != NULL; large = large->next)
    {
        if(large->allco != NULL)
        {
            free(large->allco);
            large->allco = NULL;
        }
    }
    for(temp = p, n = p->d.next; ;temp = n, n = temp->d.next)
    {
        free(temp);
        if(n == NULL)
        {
            break;
        }
    }
}

VOID* Mempool::memalign(size_t alignment, size_t size)
{
    VOID* alignPtr = NULL;
    INT32 err = -1;

    err = posix_memalign(&alignPtr, alignment, size);
    if(err)
    {
        DBG("Error: malloc mem fail!");
        alignPtr = NULL;
    }
    return alignPtr;
}

bool Mempool::canBeuse()
{
    return p != NULL;
}

bool Mempool::resetPool()
{
    pool_t* temp = NULL;
    pool_large_t* large = NULL;

    for(large = p->large; large != NULL; large = large->next)
    {
        if(large->allco != NULL)
        {
            free(large->allco);
            large->allco = NULL;
        }
    }
    for(temp = p; temp != NULL; temp = temp->d.next)
    {
        temp->d.last = (UCHAR*)temp + sizeof(pool_t);
        temp->d.faild = 0;
    }
    p->current = p;
    p->large = NULL;
    return true;
}
VOID* Mempool::palloc(size_t size)
{
    UCHAR* m = NULL;
    pool_t* ptr = NULL;
    
    if(size <= p->max)
    {
        ptr = p->current;
        do
        {
            m = ALIGN_PTR(ptr->d.last, ALIGNMENT);
            if((size_t) (ptr->d.end -m) >= size)
            {
                p->d.last = m + size;
                return m;
            }
            ptr = ptr->d.next;
        } while (ptr);
        return pallocBlock(size);
    }
    return pallocLarge(size);
}
VOID* Mempool::pnalloc(size_t size)
{

}
VOID* Mempool::pcalloc(size_t size)
{

}