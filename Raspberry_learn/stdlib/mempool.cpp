#include <string.h>
#include "mempool.h"

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
                ptr->d.last = m + size;
                return m;
            }
            ptr->d.faild ++;
            ptr = ptr->d.next;
        } while (ptr);
        return pallocBlock(size);
    }
    return pallocLarge(size);
}
VOID* Mempool::pnalloc(size_t size)
{
    return NULL;
}
VOID* Mempool::pcalloc(size_t size)
{
    VOID* m = NULL;
    m = palloc(size);
    if(m != NULL)
    {
        memset(m, 0, size);
    }
    return m;
}

VOID* Mempool::pallocBlock(size_t size)
{
    UCHAR* m = NULL;
    size_t psize = 0;
    pool_t* ptr = NULL;
    pool_t* newptr = NULL;

    psize = (size_t) (p->d.end - (UCHAR*)p);
    m = (UCHAR*)memalign(POOL_ALIGMENT, psize);
    if(m == NULL)
    {
        return NULL;
    }

    newptr = (pool_t*)m;
    newptr->d.end = (UCHAR*) (m + psize);
    newptr->d.next = NULL;
    newptr->d.faild = 0;
    m += sizeof(pool_data_t);
    m = ALIGN_PTR(m, ALIGNMENT);
    newptr->d.last = (UCHAR*) (m + size);
    for(ptr = p->current; ptr->d.next != NULL; ptr = ptr->d.next)
    {
        if(ptr->d.faild++ > 4)
        {
            p->current = ptr->d.next;
        }
    }
    ptr->d.next = newptr;
    return m;
}
VOID* Mempool::pallocLarge(size_t size)
{
    VOID *ptr = NULL;
    UINT32 n = 0;
    pool_large_t* large = NULL;

    ptr = malloc(size);
    if(ptr == NULL)
    {
        return NULL;
    }
    for(large = p->large; large != NULL; large = large->next)
    {
        if(large->allco == NULL)
        {
            large->allco = ptr;
            return ptr;
        }
        if(n ++ > 3)
        {
            break;
        }
    }
    large = (pool_large_t*)palloc(sizeof(pool_large_t));
    if(large == NULL)
    {
        free(ptr);
        return NULL;
    }
    large->allco = ptr;
    large->next = p->large;
    p->large = large;
    return ptr;
}

bool Mempool::freeLarge(VOID* ptr)
{
    pool_large_t* large = NULL;
    for(large = p->large; large != NULL; large = large->next)
    {
        if(large->allco == ptr)
        {
            free(large->allco);
            large->allco = NULL;
            return true;
        }
    }
    return false;
}

VOID Mempool::debug()
{
    INT32 PAGE = 0;
    INT32 total = 0;
    INT32 freemem = 0;
    DBG("start================================================");
    for(pool_data_t* temp = &p->d; temp != NULL; temp = &temp->next->d)
    {
        if(PAGE == 0)
        {
            total = (INT32)((UCHAR*)temp->end - ((UCHAR*)temp + sizeof(pool_t)));
        }
        else
        {
            total = (INT32)((UCHAR*)temp->end - ((UCHAR*)temp + sizeof(pool_data_t)));
        }
        freemem = (INT32)((UCHAR*)temp->end - (UCHAR*)(temp->last));
        DBG(" temp(%p) temp->end(%p) temp->last(%p)", temp, temp->end, temp->last);
        DBG("PAGE[%d]: TOTAL(%d) USED(%d) FREE(%d)", PAGE, total, total - freemem, freemem);
        PAGE ++;
    }
    DBG("stop================================================");
}