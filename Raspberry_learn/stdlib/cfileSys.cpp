#include "cfileSys.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include "clog.h"

CfileSys::CfileSys() : fd(-1), isOpenFileSucces(false)
{
}
CfileSys::CfileSys(const CHAR* path, INT32 oflag, mode_t mode) : fd(-1), isOpenFileSucces(false)
{
    copen(path, oflag, mode);
}
CfileSys::CfileSys(const CHAR* path, INT32 oflag) : fd(-1), isOpenFileSucces(false)
{
    copen(path, oflag);
}
CfileSys::CfileSys(FILE* stream) : fd(-1), isOpenFileSucces(false)
{
    copen(stream);
}

bool CfileSys::copen(FILE* stream)
{
    if(isOpenFileSucces == true || fd >= 0)
    {
        DBG("file is already open, please close it first");
        return false;
    }
    fd = fileno(stream);
    if(fd < 0)
    {
        isOpenFileSucces = false;
    }
    else
    {
        isOpenFileSucces = true;
    }
    return isOpenFileSucces;
}

bool CfileSys::copen(const CHAR* path, INT32 oflag, mode_t mode)
{
    if(isOpenFileSucces == true || fd >= 0)
    {
        DBG("file is already open, please close it first");
        return false;
    }
    fd = open(path, oflag, mode);
    if(fd < 0)
    {
        isOpenFileSucces = false;
    }
    else
    {
        isOpenFileSucces = true;
    }
    return isOpenFileSucces;
}
bool CfileSys::copen(const CHAR* path, INT32 oflag)
{
    if(isOpenFileSucces == true || fd >= 0)
    {
	DBG("isopenFile success = %d, fd = %d",isOpenFileSucces, fd);
        DBG("file is already open, please close it first");
        return false;
    }
    fd = open(path, oflag);
    if(fd < 0)
    {
        isOpenFileSucces = false;
    }
    else
    {
        isOpenFileSucces = true;
    }
    return isOpenFileSucces;
}
bool CfileSys::cclose()
{
    if(isOpenFileSucces == true && fd >= 0)
    {
        close(fd);
        fd = -1;
        isOpenFileSucces = false;
        return true;
    }
    isOpenFileSucces = false;
    fd = -1;
    return false;
}

CfileSys::~CfileSys()
{
    cclose();
}
INT32 const CfileSys::getFd()
{
    return fd;
}

bool CfileSys::fileCanUse()
{
    return isOpenFileSucces;
}

off_t CfileSys::clseek(off_t offset, INT32 whence)
{
    return lseek(fd,offset, whence);
}
ssize_t CfileSys::cread(VOID* buff, size_t nbytes)
{
    return read(fd, buff, nbytes);
}
ssize_t CfileSys::cwrite(const VOID* buff, size_t nbytes)
{
    return write(fd, buff, nbytes);
}
ssize_t CfileSys::cpwrite(const VOID*buf, size_t nbytes, off_t offset)/* offset && write*/
{
    return pwrite(fd, buf, nbytes, offset);
}
ssize_t CfileSys::cpread(VOID* buf, size_t nbytes, off_t offset)/*for offset && read*/
{
    return pread(fd, buf, nbytes, offset);
}
INT32 CfileSys::cdup()
{
    return dup(fd);
}
INT32 CfileSys::cdup2(INT32 fd2)
{
    return dup2(fd, fd2);
}
INT32 CfileSys::cfsync()
{
    return fsync(fd);
}
INT32 CfileSys::cfdatasync()/*like fsync,but fdatasync just update file data*/
{
    return fdatasync(fd);
}
VOID CfileSys::csync()/*update all file need update*/
{
    sync();
}

/*
错误——返回-1，具体错误码保存在errno中

struct stat  
{   
    dev_t       st_dev;      ID of device containing file -文件所在设备的ID 
    ino_t       st_ino;      inode number -inode节点号    
    mode_t      st_mode;     protection -保护模式
    nlink_t     st_nlink;    number of hard links -链向此文件的连接数(硬连接)  
    uid_t       st_uid;      user ID of owner -user id   
    gid_t       st_gid;      group ID of owner - group id   
    dev_t       st_rdev;     device ID (if special file) -设备号，针对设备文件    
    off_t       st_size;     total size, in bytes -文件大小，字节为单位    
    blksize_t   st_blksize;  blocksize for filesystem I/O -系统块的大小    
    blkcnt_t    st_blocks;   number of blocks allocated -文件所占块数    
    time_t      st_atime;    time of last access -最近存取时间   
    time_t      st_mtime;    time of last modification -最近修改时间 
    time_t      st_ctime;    time of last status change  
};
*/
INT32 CfileSys::cfstat(struct stat* buf)
{
    return fstat(fd, buf);
}

INT32 CfileSys::Cfchmod(mode_t mode)
{
    return fchmod(fd, mode);
}

INT32 CfileSys::Cfchown(uid_t owner, gid_t group)
{
    return fchown(fd, owner, group);
}
INT32 CfileSys::Cftruncate(off_t length)
{
    return ftruncate(fd, length);
}
INT32 CfileSys::Cfutimens(const struct timespec times[2])
{
    return futimens(fd, times);
}

INT32 CfileSys::Cfcntl(INT32 cmd)
{
    return fcntl(fd, cmd);
}
INT32 CfileSys::Cfcntl(INT32 cmd, LONG arg)
{
    return fcntl(fd, cmd, arg);
}
INT32 CfileSys::Cfcntl(INT32 cmd ,struct flock* lock)
{
    return fcntl(fd, cmd ,lock);
}

INT32 CfileSys::Cioctl(ULONG request, INT32 val)
{
    return ioctl(fd, request, val);
}
