#ifndef CFILESYS_H
#define CFILESYS_H
#include "common.h"
/*
all of these I/O function use syscall
*/
    
typedef enum
{
    OPEN,
    OPNE_AT
}OPEN_FUNC;
class CfileSys
{
public:
    CfileSys(const CHAR* path, INT32 oflag, mode_t mode);
    CfileSys(const CHAR* path, INT32 oflag);
    CfileSys(FILE* stream);
    ~CfileSys();
    CfileSys();
    bool copen(const CHAR* path, INT32 oflag, mode_t mode);
    bool copen(const CHAR* path, INT32 oflag);
    bool copen(FILE* stream);
    off_t clseek(off_t offset, INT32 whence);
    ssize_t cread(VOID* buff, size_t nbytes);
    ssize_t cwrite(const VOID* buff, size_t nbytes);
    ssize_t cpwrite(const VOID*buf, size_t nbytes, off_t offset);/* offset && write*/
    ssize_t cpread(VOID* buf, size_t nbytes, off_t offset);/*for offset && read*/
    INT32 cdup();
    INT32 cdup2(int fd2);
    INT32 cfsync();
    INT32 cfdatasync();/*like fsync,but fdatasync just update file data*/
    VOID csync();/*update all file need update*/
    bool cclose();
    INT32 const getFd();
    bool fileCanUse();
    INT32 cfstat(struct stat* buf);
    INT32 Cfchmod(mode_t mode);
    INT32 Cfchown(uid_t owner, gid_t group);
    INT32 Cftruncate(off_t length);
    INT32 Cfutimens(const struct timespec times[2]);
    INT32 Cfcntl(INT32 cmd);
    INT32 Cfcntl(INT32 cmd, LONG arg);
    INT32 Cfcntl(INT32 cmd ,struct flock* lock);
    INT32 Cioctl(ULONG request, INT32 val);
    INT32 getErrno(VOID);
private:
    VOID setErrno(INT32 merrno);
    INT32 fd;
    bool isOpenFileSucces;
    INT32 Cerrno;
    INT32 Terrno;
};
#endif
