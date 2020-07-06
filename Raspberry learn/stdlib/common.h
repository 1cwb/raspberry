#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <pwd.h>

#if __WORDSIZE == 64
typedef long int            INT_PTR;
typedef unsigned long       U_PTR;
#else
typedef int                 INT_PTR;
typedef unsigned int        U_PTR;
#endif

typedef char                INT8;
typedef unsigned char       UINT8;
typedef char                CHAR;
typedef unsigned char       UCHAR;
typedef short               INT16;
typedef unsigned short      UINT16;
typedef int                 INT32;
typedef unsigned int        UINT32;
typedef INT_PTR             LONG;
typedef U_PTR               ULONG;
typedef long long           LLONG;
typedef unsigned long long  ULLONG;
typedef void                VOID;
#define DBG_INFO
#ifdef DBG_INFO
#define DBG(fmt, ...) printf("%s()[%d] [" fmt "]\n",__FUNCTION__, __LINE__,##__VA_ARGS__)
#else
#define DBG(fmt, ...) printf("[" fmt "]\n",##__VA_ARGS__)
#endif

#define TINY_CHECK(exp) \
{ \
    if(!(exp)) \
    { \
        fprintf(stderr, "File:%s, Line:%d Exp: %d is true, abort.\n",__FILE__,__LINE__,exp);abort(); \
    } \
}

#define PATCH_LEN 128
#define FILE_NAME_LEN 128
#define PATCH_FILE_LEN 255

#define IP_RULE "([0-9]{1,3}[.]){3}[0-9]{1,3}"


INT32 getErrno(VOID);
CHAR* Cstrerror(INT32 Cerrno);
bool FindPattern(const CHAR* pContent, const CHAR* pPattern, INT32 &nStart, INT32 &nEnd);
bool DoComman(const CHAR* cmd, const CHAR* type, CHAR* result, size_t len);
bool GetIPFromIfconfig(const CHAR* cmd_buff, const CHAR* partten, CHAR IP[], INT32 IP_len);
bool getLocalTime(CHAR* mtime, INT32 len);

/*
 *These functhion is Linux Kernel space function
*/
/****************************Kernel Space************************************/
/*
mode_t
S_ISREG() //normal file
S_ISDIR() //dir    file
S_ISCHR() //string file
S_ISBLK() //block  file
S_ISFIFO() //pipe or FIFO
S_ISLNK() //link file
S_ISSOCK() //sock file
//FOR UID AND GID
S_ISUID
S_ISGID
//FOR Access permission
S_IRUSR   //usr read
S_IWUSR   //usr write
S_IXUSR   //usr exec
S_IRGRP   //group read
S_IWGRP   //group write
S_IXGRP   //group exec
S_IROTH
S_IWOTH
S_IXOTH
*/
/*
stat point
S_TYPEISMQ()  //QUEUE
S_TYPEISSEM() //SIGNAL
S_TYPEISSHM   //SHAR MEMORY
*/
INT32 Copenat(INT32 fd, const CHAR* path, INT32 oflag);
INT32 Copenat(INT32 fd, const CHAR* path, INT32 oflag, mode_t mode);
INT32 Cfstat(INT32 fd, struct stat *buf);
INT32 Cstat(const CHAR *path, struct stat *buf);
INT32 Clstat(const CHAR *path, struct stat *buf);
INT32 Cfstatat(INT32 fd, const CHAR* patchname, struct stat *buf, INT32 flag);
INT32 Cchmod(const CHAR *path, mode_t mode);
INT32 Cfchmod(INT32 fd, mode_t mode);
INT32 Cfchmodat(INT32 fd, const CHAR* patchname, mode_t mode, INT32 flag);
INT32 Caccess(const CHAR* pathname, INT32 mode);/*mode is F_OK/R_OK/W_OK/X_OK*/
INT32 Cfaccessat(INT32 fd, const CHAR* pathname, INT32 mode, INT32 flag);
INT32 Cchown(const CHAR* pathname, uid_t owner, gid_t group);
INT32 Cfchown(INT32 fd, uid_t owner, gid_t group);
INT32 Cfchownat(INT32 fd, const CHAR* pathname, uid_t owner, gid_t group, INT32 flag);
INT32 Clchown(const CHAR* pathname,uid_t owner, gid_t group);
INT32 Clink(const CHAR* existingpath, const CHAR* newpath);
INT32 Clinkat(INT32 efd, const CHAR* existingpath, INT32 fd, const CHAR* newpath, INT32 flag);
INT32 Cunlink(const CHAR* pathname);
INT32 Cunlinkat(INT32 fd, const CHAR* pathname, INT32 flag);
INT32 Ctruncate(const CHAR* pathname, off_t length);
INT32 Cftruncate(INT32 fd, off_t length);
INT32 Cremove(const CHAR* patchname);
INT32 Crename(const CHAR* oldname, const CHAR* newname);
INT32 Crenameat(INT32 oldfd, const CHAR* oldname, INT32 newfd, const CHAR* newname);
INT32 Csymlink(const CHAR* actualpath, const CHAR* sympath);
INT32 Csymlinkat(const CHAR* actualpath, INT32 fd, const CHAR* sympath);
ssize_t Creadlink(const CHAR* pathname, CHAR* buf, size_t bufsize);
ssize_t Creadlinkat(INT32 fd, const CHAR* pathname, CHAR* buf, size_t bufsize);
INT32 Cfutimens(INT32 fd, const struct timespec times[2]);
INT32 Cutimensat(INT32 fd, const CHAR* path, const struct timespec times[2], INT32 flag);
INT32 Cutimes(const CHAR* pathname, const struct timeval times[2]);
INT32 Cmkdir(const CHAR* pathname, mode_t mode = 0755);
INT32 Cmkdirat(INT32 fd, const CHAR* pathname, mode_t mode);
INT32 Crmdir(const CHAR* pathname);

DIR* Copendir(const CHAR* pathname);
DIR* Cfdopendir(INT32 fd);
struct dirent* Creaddir(DIR* dir);
VOID Crewinddir(DIR* dir);
INT32 Cclosedir(DIR* dir);
LONG Ctelldir(DIR* dir);
VOID Cseekdir(DIR* dir, LONG loc);
mode_t Cumask(mode_t cmask);
INT32 Cchdir(const CHAR* pathname);
INT32 Cfchdir(INT32 fd);
CHAR* Cgetcwd(CHAR* buf, size_t size);

/*
（1）F_DUPFD
与dup函数功能一样，复制由fd指向的文件描述符，调用成功后返回新的文件描述符，与旧的文件描述符共同指向同一个文件。
（2）F_GETFD
读取文件描述符close-on-exec标志
（3）F_SETFD
将文件描述符close-on-exec标志设置为第三个参数arg的最后一位
（4）F_GETFL
获取文件打开方式的标志，标志值含义与open调用一致
（5）F_SETFL
设置文件打开方式为arg指定方式
struct flock
{
    short_l_type;    锁的类型
    short_l_whence;  偏移量的起始位置：SEEK_SET,SEEK_CUR,SEEK_END
    off_t_l_start;   加锁的起始偏移
    off_t_l_len;     上锁字节
    pid_t_l_pid;     锁的属主进程ID
}; 
(6)F_SETLK
(7)F_SETLKW
(8)F_GETLK
*/
INT32 Cfcntl(INT32 fd, INT32 cmd);
INT32 Cfcntl(INT32 fd, INT32 cmd, LONG arg);
INT32 Cfcntl(INT32 fd, INT32 cmd ,struct flock* lock);

INT32 Cioctl(INT32 fd, ULONG request, INT32 val);

/*
    should check errno == 0,when return value = -1 (-1 is avaliable value)
*/
INT32 Cnice(INT32 incr);
/*
    return value: get nice value, fail return -1
    which: PRIO_PROCESS, PRIO_PGRP, PRIO_USER
    who:   0, choice whic param, 
*/
INT32 Cgetpriority(INT32 which, id_t who);
/*
    return ablue: fail -1, success: 0
*/
INT32 Csetpriority(INT32 which, id_t who, INT32 value);
/****************************Kernel Space************************************/

/**
 ** Below function is IOS C Fuction
 **
 **/
/*********************************IOSC**********************************/
INT32 Cgetchar(VOID);
INT32 Cputchar(INT32 c);
INT32 Cputs(const CHAR* str);
//create temp file:
FILE* Ctmpfile(VOID);/*auto unlick after create*/
CHAR* Cmkdtemp(CHAR* temppath);/*need unlink by your self*/
INT32 Cmkstemp(CHAR* temppath);/*need unlink by your self, S_IRUSR | S_IWUSR | S_IXUSR*/
/*********************************IOSC**********************************/
pid_t Cgetpid(VOID);
pid_t Cgetppid(VOID);
uid_t Cgetuid(VOID);
uid_t Cgeteuid(VOID);
gid_t Cgetgid(VOID);
gid_t Cgetegid(VOID);

INT32 Cgetresuid(uid_t *ruid, uid_t *euid, uid_t *suid);//get uid
INT32 Cgetresgid(gid_t *rgid, gid_t *egid, gid_t *sgid);//get saved gid

INT32 Csetuid(uid_t uid);
INT32 Csetgid(gid_t gid);
INT32 Csetreuid(uid_t ruid, uid_t euid); //exchange ruid and euid
INT32 Csetregid(gid_t rgid, gid_t egid);
INT32 Cseteuid(uid_t uid);
INT32 Csetegid(gid_t gid);

pid_t Cfork(VOID);
pid_t Cwait(INT32* statloc);
/*
return value:

statloc:
WIFEXITED(status) ==> WEXITSTATUS(status) get exit(status);
WIFSIGNALED(status) ==> WTERMSIG(status)/WCOREDUMP(status) get siganl; 
WIFSTOPPED(status) ==> WSTOPSIG(status) pause
WIFCONTINUED(status) ==> resue
options:
WCONTINUED
WNOHANG
WUNTRACED
*/
pid_t Cwaitpid(pid_t pid, INT32* statloc, INT32 options);
/*
idtype: P_PID P_GID P_ALL
infop: si_pid(son pid) si_uid(son userid) si_signo(always SIGCHILD)
       si_code(son event:CLD_EXIT CLD_KILLED CLD_DUMPED CLD_STOPPED CLD_CONTINUED CLD_TRAPPED)
options: WCONTINUED WEXITED WNOHANG WNOWAIT(DO NOT CHANGE STATE) WSTOPPED(wait son stoped by signal)
         
*/
INT32 Cwaitid(idtype_t idtype, id_t id, siginfo_t *infop, INT32 options);

/*
    system() will call fork() exec() waitpid()
    return value:
    fork() fail: -1, check errno
    exec() fail: exit(127)
    success: check watpid return value; 
*/
INT32 Csystem(const CHAR* cmdstring);

VOID Cexit(INT32 status);
VOID C_Exit(INT32 status);
VOID C_exit(INT32 status);
INT32 Catexit(VOID(*func)(VOID));//when exit ,will call this function
CHAR* Cgetenv(const CHAR* name);
INT32 Cputenv(CHAR* str);
INT32 Csetenv(const CHAR* name, const CHAR* value, INT32 rewrite/*if true,then rewrite*/);
INT32 Cunsetenv(const CHAR* name);

/*
    struct passwd{
        char *pw_name;    //username
        char *pw_passwd;  //user passwd
        uid_t pw_uid;     //user id
        gid_t pw_gid;     //group gid
        char *pw_gecos;   //user information
        char *pw_dir;     //home directory
        char *pw_shell;   //shell program
    };
*/
struct passwd* Cgetpwnam(const CHAR* name);
struct passwd* Cgetpwuid(uid_t uid);
/*
    when not open terminal, this function will be called failer;
    then,you can call Cgetpwnam or Cgetpwuid
*/
CHAR* Cgetlogin(VOID);
/*
struct rlimit{
    rlimit_t rlim_cur;
    rlimit_t rlil_max
};
RLIM_INFINITY nolimit
RLIMIT_AS 
RLIMIT_CORE the max core num, 0 will disble create core
RLIMIT_CPU 
RLIMIT_DATA
RLIMIT_FSIZE
RLIMIT_MEMLOCK
RLIMIT_MSGQUEUE
RLIMIT_NICE
RLIMIT_NOFILE
RLIMIT_NPROC
RLIMIT_NPTS
RLIMIT_RSS
RLIMIT_SBSIZE
RLIMIT_SIGPENDING
RLIMIT_STACK
RLIMIT_SWAP
RLIMIT_VMEM

*/
INT32 Cgetlimit(INT32 resource, struct rlimit *rlptr);
INT32 Csetlimit(INT32 resource, const struct rlimit *rlptr);

typedef enum
{
MSG_USB_REMOVE = 0,
MSG_USB_ADD,
MSG_OTHER_EVENT,
MSG_SHOW_TIME,
MSG_UPDATE_IP,
MSG_SHOW_IP,
MSG_WRITE_MSG_TO_DISK_OK,
MSG_WRITE_MSG_TO_DISK_FAIL,
MSG_UNKNOW_EVENT,
MSG_EVENT_MAX
}MSG_EVENT;

#endif
