#include "common.h"

#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <errno.h>
#include <fcntl.h>

#include <dirent.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <signal.h>
#include "deelx.h"


static INT32 Cerrno = 0;
static INT32 Terrno = 0;
static VOID setErrno(INT32 merrno)
{
    Cerrno = merrno;
}
INT32 getErrno(VOID)
{
    Terrno = Cerrno;
    Cerrno = 0;
    return Terrno;
}
CHAR* Cstrerror(INT32 Cerrno)
{
    return strerror(Cerrno);
}
bool FindPattern(const CHAR* pContent, const CHAR* pPattern, INT32 &nStart, INT32 &nEnd)
{
    CRegexpT< CHAR > Regexp(pPattern);
    MatchResult clResult = Regexp.Match(pContent);
    bool bFind = false;
    if(clResult.IsMatched())
    {
        nStart = clResult.GetStart();
        nEnd = clResult.GetEnd();
        bFind = true;
    }
    return bFind;
}

bool DoComman(const CHAR* cmd, const CHAR* type, CHAR* result, size_t len)
{
    FILE* fp = NULL;
    if(cmd == NULL || type == NULL || result == NULL || len <= 0)
    {
        return false;
    }
    fp = popen(cmd, type);
    if(fp == NULL)
    {  
        setErrno(errno);
        printf("popen cmd fail\n");
        return false;
    }
    if(fread(result, 1, len, fp) < 0)
    {   
        setErrno(errno);
        pclose(fp);
        return false;
    }
    printf("do command (%s) sucessful\n",cmd);
    pclose(fp);
    return true;
}

bool GetIPFromIfconfig(const CHAR* cmd_buff, const CHAR* partten, CHAR IP[], INT32 IP_len)
{
    if(IP_len < 24)
    {
        //DBG("IP[] len is too short!");
        return false;
    }
    const CHAR* p = cmd_buff;
    INT32 nstart = 0, nend = 0, nLen = 0;
    if(FindPattern(p, partten, nstart, nend))
    {
        //DBG("find eth0: nstart is %d, nedn is %d",nstart, nend);
        nLen = nend - nstart;
        memcpy(IP, p + nstart, nend - nstart);
        IP[nLen] = ':';
	    IP[++nLen] = '\0';
	    p += nend;
        if(FindPattern(p, IP_RULE, nstart, nend))
        {
            //DBG("find eth0 IP : nstart is %d, nedn is %d",nstart, nend);
            memcpy(IP + nLen, p + nstart, nend - nstart);
            IP[nLen + nend - nstart] = '\0';
            //DBG("%s",IP);
            return true;
        }
    }
    //DBG("parse ip fail");
    return false;
}
bool RemoveCRLF(CHAR* data)
{
    if(data == NULL)
    {
        return false;
    }
    CHAR* p = NULL;
    if((p = strchr(data, '\r')) != NULL)
    {
        *p = '\0';
    }
    else
    {
        if((p = strchr(data, '\n')) != NULL)
        {
            *p = '\0';
        }
    }
    return true;
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

/*
执行成功返回0，失败返回－1，错误代码保存在errno。
*/
INT32 Cfstat(INT32 fd, struct stat *buf)
{
     INT32 ret = fstat(fd, buf);
     if(ret < 0)
     {
         setErrno(errno);
     }
     return ret;
}
INT32 Cstat(const CHAR *path, struct stat *buf)
{
    INT32 ret = stat(path, buf);
    if(ret < 0)
    {
       setErrno(errno); 
    }
    return ret;
}

INT32 Clstat(const CHAR *path, struct stat *buf)
{
    INT32 ret = lstat(path, buf);
    if(ret < 0)
    {
        setErrno(errno); 
    }
    return ret;
}

INT32 Cfstatat(INT32 fd, const CHAR* patchname, struct stat *buf, INT32 flag)
{
    INT32 ret = fstatat(fd, patchname, buf, flag);
    if(ret < 0)
    {
        setErrno(errno); 
    }
    return ret;
}

INT32 Cfcntl(INT32 fd, INT32 cmd)
{
    INT32 ret = fcntl(fd, cmd);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
INT32 Cfcntl(INT32 fd, INT32 cmd, LONG arg)
{
    INT32 ret = fcntl(fd, cmd, arg);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
INT32 Cfcntl(INT32 fd, INT32 cmd ,struct flock* lock)
{
    INT32 ret = fcntl(fd, cmd ,lock);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}

INT32 Cchmod(const CHAR *path, mode_t mode)
{
    INT32 ret = chmod(path, mode);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
INT32 Cfchmod(INT32 fd, mode_t mode)
{
    INT32 ret = fchmod(fd, mode);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
INT32 Cfchmodat(INT32 fd, const CHAR* patchname, mode_t mode, INT32 flag)
{
    INT32 ret = fchmodat(fd, patchname, mode, flag);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}

INT32 Copenat(INT32 fd, const CHAR* path, INT32 oflag)
{
    INT32 ret = openat(fd, path, oflag);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
INT32 Copenat(INT32 fd, const CHAR* path, INT32 oflag, mode_t mode)
{
    INT32 ret = openat(fd, path, oflag, mode);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret; 
}

INT32 Caccess(const CHAR* pathname, INT32 mode)/*mode is F_OK/R_OK/W_OK/X_OK*/
{
    INT32 ret = access(pathname, mode);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
INT32 Cfaccessat(INT32 fd, const CHAR* pathname, INT32 mode, INT32 flag)
{
    INT32 ret = faccessat(fd, pathname, mode, flag);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
INT32 Clink(const CHAR* existingpath, const CHAR* newpath)
{
    INT32 ret = link(existingpath, newpath);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
INT32 Clinkat(INT32 efd, const CHAR* existingpath, INT32 fd, const CHAR* newpath, INT32 flag)
{
    INT32 ret = linkat(efd, existingpath, fd, newpath, flag);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
INT32 Cunlink(const CHAR* pathname)
{
    INT32 ret = unlink(pathname);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
INT32 Cunlinkat(INT32 fd, const CHAR* pathname, INT32 flag)
{
    INT32 ret = unlinkat(fd, pathname, flag);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
INT32 Ctruncate(const CHAR* pathname, off_t length)
{
    INT32 ret = truncate(pathname, length);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
INT32 Cftruncate(INT32 fd, off_t length)
{
    INT32 ret = ftruncate(fd, length);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
INT32 Cremove(const CHAR* patchname)
{
    INT32 ret = remove(patchname);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
INT32 Crename(const CHAR* oldname, const CHAR* newname)
{
    INT32 ret = rename(oldname, newname);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
INT32 Crenameat(INT32 oldfd, const CHAR* oldname, INT32 newfd, const CHAR* newname)
{
    INT32 ret = renameat(oldfd, oldname, newfd, newname);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
INT32 Csymlink(const CHAR* actualpath, const CHAR* sympath)
{
    INT32 ret = symlink(actualpath, sympath);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
INT32 Csymlinkat(const CHAR* actualpath, INT32 fd, const CHAR* sympath)
{
    INT32 ret = symlinkat(actualpath, fd, sympath);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
ssize_t Creadlink(const CHAR* pathname, CHAR* buf, size_t bufsize)
{
    INT32 ret = readlink(pathname, buf, bufsize);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
ssize_t Creadlinkat(INT32 fd, const CHAR* pathname, CHAR* buf, size_t bufsize)
{
    INT32 ret = readlinkat(fd, pathname, buf, bufsize);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
INT32 Cfutimens(INT32 fd, const struct timespec times[2])
{
    INT32 ret = futimens(fd, times);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
INT32 Cutimensat(INT32 fd, const CHAR* path, const struct timespec times[2], INT32 flag)
{
    INT32 ret = utimensat(fd, path, times, flag);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
INT32 Cutimes(const CHAR* pathname, const struct timeval times[2])
{
    INT32 ret = utimes(pathname, times);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
INT32 Cmkdir(const CHAR* pathname, mode_t mode)
{
    INT32 ret = mkdir( pathname, mode);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
INT32 Cmkdirat(INT32 fd, const CHAR* pathname, mode_t mode)
{
    INT32 ret = mkdirat(fd, pathname, mode);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}
INT32 Crmdir(const CHAR* pathname)
{
    INT32 ret = rmdir(pathname);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}

INT32 Cioctl(INT32 fd, ULONG request, INT32 val)
{
    INT32 ret = ioctl(fd, request, val);
    if(ret < 0)
    {
        setErrno(errno);
    }
    return ret;
}

INT32 Cnice(INT32 incr)
{
    INT32 res = nice(incr);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}

INT32 Cgetpriority(INT32 which, id_t who)
{
    INT32 res = getpriority(which, who);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
INT32 Csetpriority(INT32 which, id_t who, INT32 value)
{
    INT32 res = setpriority(which, who, value);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
INT32 Cmount(const CHAR *source, const CHAR *target, const CHAR *filesystemtype, ULONG mountflags, const VOID *data)
{
    INT32 res = mount(source, target, filesystemtype, mountflags, data);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
INT32 Cumount(const CHAR *target)
{
    INT32 res = umount(target);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
INT32 Cumount2(const CHAR *target, INT32 flags)
{
    INT32 res = umount2(target, flags);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
DIR* Copendir(const CHAR* pathname)
{
    DIR* dir = opendir(pathname);
    if(dir == NULL)
    {
        setErrno(errno);
    }
    return dir;
}
DIR* Cfdopendir(INT32 fd)
{
    DIR* dir = fdopendir(fd);
    if(dir == NULL)
    {
        setErrno(errno);
    }
    return dir;
}

struct dirent* Creaddir(DIR* dir)
{
    struct dirent* dirent = readdir(dir);
    if(dirent == NULL)
    {
        setErrno(errno);
    }
    return dirent;
}
VOID Crewinddir(DIR* dir)
{
    rewinddir(dir);
}
INT32 Cclosedir(DIR* dir)
{
    INT32 res = closedir(dir);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
LONG Ctelldir(DIR* dir)
{
    LONG res = telldir(dir);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
VOID Cseekdir(DIR* dir, LONG loc)
{
    seekdir(dir, loc);
}

bool getLocalTime(CHAR* mtime, INT32 len)
{
    if(mtime == NULL || len < 0)
    {
    	return false;
    }
    time_t now ;
    struct tm *tm_now ;
    time(&now) ;
    tm_now = localtime(&now) ;
    snprintf(mtime, len, "%d-%d-%d %d:%d:%d",tm_now->tm_year+1900, tm_now->tm_mon+1,tm_now->tm_mday,tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
    //printf("now datetime: %d-%d-%d %d:%d:%d\n",tm_now->tm_year+1900, tm_now->tm_mon+1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec) ;
    return true;
}

INT32 Cchdir(const CHAR* pathname)
{
    INT32 res = chdir(pathname);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
INT32 Cfchdir(INT32 fd)
{
    INT32 res = fchdir(fd);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
CHAR* Cgetcwd(CHAR* buf, size_t size)
{
    CHAR* p = getcwd(buf, size);
    if(p == NULL)
    {
        setErrno(errno);
    }
    return p;
}


/*********************************IOSC**********************************/
INT32 Cgetchar(VOID)
{
    INT32 res = getchar();
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
INT32 Cputchar(INT32 c)
{
    INT32 res = putchar(c);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
INT32 Cputs(const CHAR* str)
{
    INT32 res = puts(str);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}

pid_t Cgetpid(VOID)
{
    return getpid();
}
pid_t Cgetppid(VOID)
{
    return getppid();
}
uid_t Cgetuid(VOID)
{
    return getuid();
}
uid_t Cgeteuid(VOID)
{
    return geteuid();
}
gid_t Cgetgid(VOID)
{
    return getgid();
}
gid_t Cgetegid(VOID)
{
    return getegid();
}

INT32 Cgetresuid(uid_t *ruid, uid_t *euid, uid_t *suid)
{
    INT32 res = getresuid(ruid, euid, suid);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
INT32 Cgetresgid(gid_t *rgid, gid_t *egid, gid_t *sgid)
{
    INT32 res = getresgid(rgid, egid, sgid);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}

INT32 Csetuid(uid_t uid)
{
    INT32 res = setuid(uid);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
INT32 Csetgid(gid_t gid)
{
    INT32 res = setgid(gid);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
INT32 Csetreuid(uid_t ruid, uid_t euid)
{
    INT32 res = setreuid(ruid, euid);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
INT32 Csetregid(gid_t rgid, gid_t egid)
{
    INT32 res = setregid(rgid, egid);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
INT32 Cseteuid(uid_t uid)
{
    INT32 res = seteuid(uid);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
INT32 Csetegid(gid_t gid)
{
    INT32 res = setegid(gid);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}

pid_t Cfork(VOID)
{
    pid_t pid = fork();
    if(pid < 0)
    {
        setErrno(errno);
    }
    return pid;
}

pid_t Cwait(INT32* statloc)
{
    pid_t pid = wait(statloc);
    if(pid < 0)
    {
        setErrno(errno);
    }
    return pid;
}
pid_t Cwaitpid(pid_t pid, INT32* statloc, INT32 options)
{
    pid_t npid = waitpid(pid, statloc, options);
    if(npid < 0)
    {
        setErrno(errno);
    }
    return npid;
}
INT32 Cwaitid(idtype_t idtype, id_t id, siginfo_t *infop, INT32 options)
{
    INT32 res = waitid(idtype, id, infop, options);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
INT32 Csystem(const CHAR* cmdstring)
{
    INT32 res = system(cmdstring);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
VOID Cexit(INT32 status) //ISO C,fclose all open file and fllush all stream
{
    exit(status);
}
VOID  C_Exit(INT32 status) //ISO C,fclose all open file and fllush all stream
{
    _Exit(status);
}
VOID C_exit(INT32 status)// system call
{
    _exit(status);
}

INT32 Catexit(VOID(*func)(VOID))
{
    return atexit(func);
}
CHAR* Cgetenv(const CHAR* name)
{
    return getenv(name);
}
INT32 Cputenv(CHAR* str)
{
    INT32 res = putenv(str);
    if(res != 0)
    {
        setErrno(errno);
    }
    return res;
}
INT32 Csetenv(const CHAR* name, const CHAR* value, INT32 rewrite/*if true,then rewrite*/)
{
    INT32 res = setenv(name, value, rewrite/*if true,then rewrite*/);
    if(res != 0)
    {
        setErrno(errno);
    }
    return res;
}
INT32 Cunsetenv(const CHAR* name)
{
    INT32 res = unsetenv(name);
    if(res != 0)
    {
        setErrno(errno);
    }
    return res;
}
/*********************************IOSC**********************************/
struct passwd* Cgetpwnam(const CHAR* name)
{
    struct passwd *pwd = getpwnam(name);
    if(pwd == NULL)
    {
        setErrno(errno);
    }
    return pwd;
}
struct passwd* Cgetpwuid(uid_t uid)
{
    struct passwd *pwd = getpwuid(uid);
    if(pwd == NULL)
    {
        setErrno(errno);
    }
    return pwd;
}

CHAR* Cgetlogin(VOID)
{
    CHAR* pwd = getlogin();
    if(pwd == NULL)
    {
        setErrno(errno);
    }
    return pwd;
}
/*
SIG_FUNC* Csignal(INT32 signo, SIG_FUNC* func)
{
    return signal(signo, func);
}
*/
INT32 Ckill(pid_t pid, INT32 signo)
{
    INT32 res = kill(pid, signo);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
INT32 Craise(INT32 signo)
{
    INT32 res = raise(signo);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}

UINT32 Calarm(UINT32 seconds)
{
    return alarm(seconds);
}

INT32 Cpause(VOID)
{
    INT32 res = pause();
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}

INT32 Csigemptyset(sigset_t *set)
{
    INT32 res = sigemptyset(set);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
INT32 Csigfillset(sigset_t *set)
{
    INT32 res = sigfillset(set);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
INT32 Csigaddset(sigset_t *set, INT32 signo)
{
    INT32 res = sigaddset(set, signo);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
INT32 Csigdelset(sigset_t *set, INT32 signo)
{
    INT32 res = sigdelset(set, signo);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
bool  Csigismember(const sigset_t *set, INT32 signo)
{
    return sigismember(set, signo);
}
INT32 Csigprocmask(INT32 how, const sigset_t *set, sigset_t *oset)
{
    INT32 res = sigprocmask(how, set, oset);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
INT32 Csigpending(sigset_t *set)
{
    INT32 res = sigpending(set);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
INT32 Csigaction(INT32 signo, const struct sigaction* act, struct sigaction* oact)
{
    INT32 res = sigaction(signo, act, oact);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
SIG_FUNC *Csignal(INT32 signo, SIG_FUNC* func)
{
    struct sigaction act, oact;
    act.sa_handler = func;
    Csigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    if(signo == SIGALRM)
    {
    #ifdef SA_INTERRUPT
        act.sa_flags |= SA_INTERRUPT;
    #endif
    }
    else
    {
        act.sa_flags |= SA_RESTART;
    }
    if(Csigaction(signo, &act, &oact) < 0)
    {
        return SIG_ERR;
    }
    return oact.sa_handler;
}
INT32 Csigsuspend(const sigset_t *sigmask)
{
    INT32 res = sigsuspend(sigmask);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;//always return -1
}

VOID Cpsignal(INT32 signo, const CHAR* msg)
{
    return psignal(signo, msg);
}
VOID Cpsiginfo(const siginfo_t *info, const CHAR* msg)
{
    return psiginfo(info, msg);
}
CHAR* Cstrsignal(INT32 signo)
{
    return strsignal(signo);
}
INT32 Cnanosleep(const struct timespec *reqtp, timespec *remtp)
{
    INT32 res = nanosleep(reqtp, remtp);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
INT32 Cclock_nanosleep(clockid_t clock_id, INT32 flags, const struct timespec *reqtp, timespec *remtp)
{
    INT32 res = clock_nanosleep(clock_id, flags, reqtp, remtp);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}