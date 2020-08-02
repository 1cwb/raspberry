#include "cfileSys.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>


VOID CfileSys::setErrno(INT32 merrno)
{
    Cerrno = merrno;
}
INT32 CfileSys::getErrno(VOID)
{
    Terrno = Cerrno;
    Cerrno = 0;
    return Terrno;
}
CfileSys::CfileSys() : fd(-1), isOpenFileSucces(false), Cerrno(0), Terrno(0)
{
}
CfileSys::CfileSys(const CHAR* path, INT32 oflag, mode_t mode) : fd(-1), isOpenFileSucces(false) , Cerrno(0), Terrno(0)
{
    copen(path, oflag, mode);
}
CfileSys::CfileSys(const CHAR* path, INT32 oflag) : fd(-1), isOpenFileSucces(false) , Cerrno(0), Terrno(0)
{
    copen(path, oflag);
}
CfileSys::CfileSys(FILE* stream) : fd(-1), isOpenFileSucces(false) , Cerrno(0), Terrno(0)
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
        setErrno(errno);
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
        setErrno(errno);
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
        setErrno(errno);
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
    INT32 ret = lseek(fd,offset, whence);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
ssize_t CfileSys::cread(VOID* buff, size_t nbytes)
{
    INT32 ret = read(fd, buff, nbytes);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
ssize_t CfileSys::cwrite(const VOID* buff, size_t nbytes)
{
    INT32 ret = write(fd, buff, nbytes);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
ssize_t CfileSys::cpwrite(const VOID*buf, size_t nbytes, off_t offset)/* offset && write*/
{
    INT32 ret = pwrite(fd, buf, nbytes, offset);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
ssize_t CfileSys::cpread(VOID* buf, size_t nbytes, off_t offset)/*for offset && read*/
{
    INT32 ret = pread(fd, buf, nbytes, offset);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
INT32 CfileSys::cdup()
{
    INT32 ret = dup(fd);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
INT32 CfileSys::cdup2(INT32 fd2)
{
    INT32 ret = dup2(fd, fd2);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
INT32 CfileSys::cfsync()
{
    INT32 ret = fsync(fd);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
INT32 CfileSys::cfdatasync()/*like fsync,but fdatasync just update file data*/
{
    INT32 ret = fdatasync(fd);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
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
    INT32 ret = fstat(fd, buf);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}

INT32 CfileSys::Cfchmod(mode_t mode)
{
    INT32 ret = fchmod(fd, mode);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}

INT32 CfileSys::Cfchown(uid_t owner, gid_t group)
{
    INT32 ret = fchown(fd, owner, group);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
INT32 CfileSys::Cftruncate(off_t length)
{
    INT32 ret = ftruncate(fd, length);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
INT32 CfileSys::Cfutimens(const struct timespec times[2])
{
    INT32 ret = futimens(fd, times);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}

INT32 CfileSys::Cfcntl(INT32 cmd)
{
    INT32 ret = fcntl(fd, cmd);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
INT32 CfileSys::Cfcntl(INT32 cmd, LONG arg)
{
    INT32 ret = fcntl(fd, cmd, arg);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
INT32 CfileSys::Cfcntl(INT32 cmd ,struct flock* lock)
{
    INT32 ret = fcntl(fd, cmd ,lock);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}

INT32 CfileSys::Cioctl(ULONG request, INT32 val)
{
    INT32 ret = ioctl(fd, request, val);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
