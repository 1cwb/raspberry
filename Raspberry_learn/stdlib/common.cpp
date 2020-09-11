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
#include "clog.h"
#include "deelx.h"


INT32 Cgeterrno()
{
    return errno;
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
        LOG_ERROR("popen cmd fail");
        return false;
    }
    sync();
    if(fread(result, 1, len, fp) < 0)
    {   
        pclose(fp);
        return false;
    }
    LOG_DEBUG("do command (%s) sucessful",cmd);
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
bool Daemonize(const CHAR* cmd)
{
    UINT32 i = 0;
    INT32 fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;

    DBG("Start daemon named %s !",cmd);

    Cumask(0);
   
    if(Cgetrlimit(RLIMIT_NOFILE, &rl) < 0)
    {
        DBG("%s can not get file limit", cmd);
    }
    if((pid = Cfork()) < 0)
    {
        DBG("%s fork fail!",cmd);
	    return false;
    }
    else if(pid != 0)
    {
         Cexit(0);
    }
    Csetsid();
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if(Csigaction(SIGHUP, &sa, NULL) < 0)
    {
        DBG("Add SIGHUP fail!");
    }
    if((pid = Cfork()) < 0)
    {
        DBG("%s fork fail!",cmd);
	    return false;
    }
    else if(pid != 0)
    {
         Cexit(0);
    }

    if(Cchdir("/") < 0)
    {
        DBG("can not change directory to /");
    }

    if(rl.rlim_max == RLIM_INFINITY)
    {
        rl.rlim_max = 1024;
    }
    for(i = 0; i < rl.rlim_max; i++)
    {
        close(i);
    }

    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);
    if(fd0 != 0 || fd1 != 1 || fd2 != 2)
    {
        DBG("close fd0-fd2 fail!");
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
     return fstat(fd, buf);
}
INT32 Cstat(const CHAR *path, struct stat *buf)
{
    return stat(path, buf);
}

INT32 Clstat(const CHAR *path, struct stat *buf)
{
    return lstat(path, buf);
}

INT32 Cfstatat(INT32 fd, const CHAR* patchname, struct stat *buf, INT32 flag)
{
    return fstatat(fd, patchname, buf, flag);
}

INT32 Cfcntl(INT32 fd, INT32 cmd)
{
    return fcntl(fd, cmd);
}
INT32 Cfcntl(INT32 fd, INT32 cmd, LONG arg)
{
    return fcntl(fd, cmd, arg);
}
INT32 Cfcntl(INT32 fd, INT32 cmd ,struct flock* lock)
{
    return fcntl(fd, cmd ,lock);
}

INT32 Cchmod(const CHAR *path, mode_t mode)
{
    return chmod(path, mode);
}
INT32 Cfchmod(INT32 fd, mode_t mode)
{
    return fchmod(fd, mode);
}
INT32 Cfchmodat(INT32 fd, const CHAR* patchname, mode_t mode, INT32 flag)
{
    return fchmodat(fd, patchname, mode, flag);
}

INT32 Copenat(INT32 fd, const CHAR* path, INT32 oflag)
{
    return openat(fd, path, oflag);
}
INT32 Copenat(INT32 fd, const CHAR* path, INT32 oflag, mode_t mode)
{
    return openat(fd, path, oflag, mode); 
}

INT32 Caccess(const CHAR* pathname, INT32 mode)/*mode is F_OK/R_OK/W_OK/X_OK*/
{
    return access(pathname, mode);
}
INT32 Cfaccessat(INT32 fd, const CHAR* pathname, INT32 mode, INT32 flag)
{
    return faccessat(fd, pathname, mode, flag);
}
INT32 Clink(const CHAR* existingpath, const CHAR* newpath)
{
    return link(existingpath, newpath);
}
INT32 Clinkat(INT32 efd, const CHAR* existingpath, INT32 fd, const CHAR* newpath, INT32 flag)
{
    return linkat(efd, existingpath, fd, newpath, flag);
}
INT32 Cunlink(const CHAR* pathname)
{
    return unlink(pathname);
}
INT32 Cunlinkat(INT32 fd, const CHAR* pathname, INT32 flag)
{
    return unlinkat(fd, pathname, flag);
}
INT32 Ctruncate(const CHAR* pathname, off_t length)
{
    return truncate(pathname, length);
}
INT32 Cftruncate(INT32 fd, off_t length)
{
    return ftruncate(fd, length);
}
INT32 Cremove(const CHAR* patchname)
{
    return remove(patchname);
}
INT32 Crename(const CHAR* oldname, const CHAR* newname)
{
    return rename(oldname, newname);
}
INT32 Crenameat(INT32 oldfd, const CHAR* oldname, INT32 newfd, const CHAR* newname)
{
    return renameat(oldfd, oldname, newfd, newname);
}
INT32 Csymlink(const CHAR* actualpath, const CHAR* sympath)
{
    return symlink(actualpath, sympath);
}
INT32 Csymlinkat(const CHAR* actualpath, INT32 fd, const CHAR* sympath)
{
    return symlinkat(actualpath, fd, sympath);
}
ssize_t Creadlink(const CHAR* pathname, CHAR* buf, size_t bufsize)
{
    return readlink(pathname, buf, bufsize);
}
ssize_t Creadlinkat(INT32 fd, const CHAR* pathname, CHAR* buf, size_t bufsize)
{
    return readlinkat(fd, pathname, buf, bufsize);
}
INT32 Cfutimens(INT32 fd, const struct timespec times[2])
{
    return futimens(fd, times);
}
INT32 Cutimensat(INT32 fd, const CHAR* path, const struct timespec times[2], INT32 flag)
{
    return utimensat(fd, path, times, flag);
}
INT32 Cutimes(const CHAR* pathname, const struct timeval times[2])
{
    return utimes(pathname, times);
}
INT32 Cmkdir(const CHAR* pathname, mode_t mode)
{
    return mkdir( pathname, mode);
}
INT32 Cmkdirat(INT32 fd, const CHAR* pathname, mode_t mode)
{
    return mkdirat(fd, pathname, mode);
}
INT32 Crmdir(const CHAR* pathname)
{
    return rmdir(pathname);
}

INT32 Cioctl(INT32 fd, ULONG request, INT32 val)
{
    return ioctl(fd, request, val);
}

INT32 Cnice(INT32 incr)
{
    return nice(incr);
}

INT32 Cgetpriority(INT32 which, id_t who)
{
    return getpriority(which, who);
}
INT32 Csetpriority(INT32 which, id_t who, INT32 value)
{
    return setpriority(which, who, value);
}
INT32 Cmount(const CHAR *source, const CHAR *target, const CHAR *filesystemtype, ULONG mountflags, const VOID *data)
{
    return mount(source, target, filesystemtype, mountflags, data);
}
INT32 Cumount(const CHAR *target)
{
    return umount(target);
}
INT32 Cumount2(const CHAR *target, INT32 flags)
{
    return umount2(target, flags);
}
DIR* Copendir(const CHAR* pathname)
{
    return opendir(pathname);
}
DIR* Cfdopendir(INT32 fd)
{
    return fdopendir(fd);
}

struct dirent* Creaddir(DIR* dir)
{
    return readdir(dir);
}
VOID Crewinddir(DIR* dir)
{
    rewinddir(dir);
}
INT32 Cclosedir(DIR* dir)
{
    return closedir(dir);
}
LONG Ctelldir(DIR* dir)
{
    return telldir(dir);
}
VOID Cseekdir(DIR* dir, LONG loc)
{
    seekdir(dir, loc);
}
mode_t Cumask(mode_t cmask)
{
    return umask(cmask);
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
    return chdir(pathname);
}
INT32 Cfchdir(INT32 fd)
{
    return fchdir(fd);
}
CHAR* Cgetcwd(CHAR* buf, size_t size)
{
    return getcwd(buf, size);
}


/*********************************IOSC**********************************/
INT32 Cgetchar(VOID)
{
    return getchar();
}
INT32 Cputchar(INT32 c)
{
    return putchar(c);
}
INT32 Cputs(const CHAR* str)
{
    return puts(str);
}
pid_t Csetsid(VOID)
{
    return setsid();
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
    return getresuid(ruid, euid, suid);
}
INT32 Cgetresgid(gid_t *rgid, gid_t *egid, gid_t *sgid)
{
    return getresgid(rgid, egid, sgid);
}

INT32 Csetuid(uid_t uid)
{
    return setuid(uid);
}
INT32 Csetgid(gid_t gid)
{
    return setgid(gid);
}
INT32 Csetreuid(uid_t ruid, uid_t euid)
{
    return setreuid(ruid, euid);
}
INT32 Csetregid(gid_t rgid, gid_t egid)
{
    return setregid(rgid, egid);
}
INT32 Cseteuid(uid_t uid)
{
    return seteuid(uid);
}
INT32 Csetegid(gid_t gid)
{
    return setegid(gid);
}

pid_t Cfork(VOID)
{
    return fork();
}

pid_t Cwait(INT32* statloc)
{
    return wait(statloc);
}
pid_t Cwaitpid(pid_t pid, INT32* statloc, INT32 options)
{
    return waitpid(pid, statloc, options);
}
INT32 Cwaitid(idtype_t idtype, id_t id, siginfo_t *infop, INT32 options)
{
    return waitid(idtype, id, infop, options);
}
INT32 Csystem(const CHAR* cmdstring)
{
    return system(cmdstring);
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
    return putenv(str);
}
INT32 Csetenv(const CHAR* name, const CHAR* value, INT32 rewrite/*if true,then rewrite*/)
{
    return setenv(name, value, rewrite/*if true,then rewrite*/);
}
INT32 Cunsetenv(const CHAR* name)
{
    return unsetenv(name);
}
INT32 Cgetrlimit(INT32 resource, struct rlimit *rlptr)
{
    return getrlimit(resource, rlptr);
}
INT32 Csetrlimit(INT32 resource, const struct rlimit *rlptr)
{
    return setrlimit(resource, rlptr);
}

/*********************************IOSC**********************************/
struct passwd* Cgetpwnam(const CHAR* name)
{
    return getpwnam(name);
}
struct passwd* Cgetpwuid(uid_t uid)
{
    return getpwuid(uid);
}

CHAR* Cgetlogin(VOID)
{
    return getlogin();
}
/*
SIG_FUNC* Csignal(INT32 signo, SIG_FUNC* func)
{
    return signal(signo, func);
}
*/
INT32 Ckill(pid_t pid, INT32 signo)
{
    return kill(pid, signo);
}
INT32 Craise(INT32 signo)
{
    return raise(signo);
}

UINT32 Calarm(UINT32 seconds)
{
    return alarm(seconds);
}

INT32 Cpause(VOID)
{
    return pause();
}

INT32 Csigemptyset(sigset_t *set)
{
    return sigemptyset(set);
}
INT32 Csigfillset(sigset_t *set)
{
    return sigfillset(set);
}
INT32 Csigaddset(sigset_t *set, INT32 signo)
{
    return sigaddset(set, signo);
}
INT32 Csigdelset(sigset_t *set, INT32 signo)
{
    return sigdelset(set, signo);
}
bool  Csigismember(const sigset_t *set, INT32 signo)
{
    return sigismember(set, signo);
}
INT32 Csigprocmask(INT32 how, const sigset_t *set, sigset_t *oset)
{
    return sigprocmask(how, set, oset);
}
INT32 Csigpending(sigset_t *set)
{
    return sigpending(set);
}
INT32 Csigaction(INT32 signo, const struct sigaction* act, struct sigaction* oact)
{
    return sigaction(signo, act, oact);
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
    return sigsuspend(sigmask);//always return -1
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
    return nanosleep(reqtp, remtp);
}
INT32 Cclock_nanosleep(clockid_t clock_id, INT32 flags, const struct timespec *reqtp, timespec *remtp)
{
    return clock_nanosleep(clock_id, flags, reqtp, remtp);
}
