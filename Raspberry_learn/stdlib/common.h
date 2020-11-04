#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <pwd.h>
#include <fcntl.h>

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
typedef LLONG               INT64;
typedef ULLONG              UINT64;
typedef void                VOID;
typedef double              DOUBLE;
typedef float               FLOAT;

#define PATCH_LEN 128
#define FILE_NAME_LEN 128
#define PATCH_FILE_LEN 255

/*
SIG_ERR, SIG_DFL, SIG_IGN
void (*signal(int signo, void (*func)(int))) (int);
*/
typedef VOID SIG_FUNC(INT32);
#define LINUX_SIGNAL_SUPPORT_NUM 64

#define IP_RULE "([0-9]{1,3}[.]){3}[0-9]{1,3}"
#define MIN(a, b) (((a) <= (b)) ? (a) : (b))
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
INT32 Cfcntl(INT32 fd, INT32 cmd ,struct flock* lock, INT32 lock_len);

INT32 Cioctl(INT32 fd, ULONG request, INT32 val);

/*
    should check errno == 0,when return value = -1 (-1 is avaliable value)
*/
INT32 Cnice(INT32 incr);
/*
    return value: get nice value, fail return -1
    which: PRIO_PROCESS, PRIO_PGRP, PRIO_USER
    who:   0, choice whic param 
*/
INT32 Cgetpriority(INT32 which, id_t who);
/*
    return ablue: fail -1, success: 0
    which: PRIO_PROCESS, PRIO_PGRP, PRIO_USER
    who:   0, choice whic param 
*/
INT32 Csetpriority(INT32 which, id_t who, INT32 value);
/*
参数：   
source：将要挂上的文件系统，通常是一个设备名。
target：文件系统所要挂在的目标目录。
filesystemtype：文件系统的类型，可以是"ext2"，"msdos"，"proc"，"nfs"，"iso9660" 。。。
mountflags：指定文件系统的读写访问标志，可能值有以下
MS_BIND：执行bind挂载，使文件或者子目录树在文件系统内的另一个点上可视。
MS_DIRSYNC：同步目录的更新。
MS_MANDLOCK：允许在文件上执行强制锁。
MS_MOVE：移动子目录树。
MS_NOATIME：不要更新文件上的访问时间。
MS_NODEV：不允许访问设备文件。
MS_NODIRATIME：不允许更新目录上的访问时间。
MS_NOEXEC：不允许在挂上的文件系统上执行程序。
MS_NOSUID：执行程序时，不遵照set-user-ID 和 set-group-ID位。
MS_RDONLY：指定文件系统为只读。
MS_REMOUNT：重新加载文件系统。这允许你改变现存文件系统的mountflag和数据，而无需使用先卸载，再挂上文件系统的方式。
MS_SYNCHRONOUS：同步文件的更新。
MNT_FORCE：强制卸载，即使文件系统处于忙状态。
MNT_EXPIRE：将挂载点标志为过时。
data：文件系统特有的参数。
    
返回说明：   
成功执行时，返回0。失败返回-1，errno被设为以下的某个值   
EACCES：权能不足，可能原因是，路径的一部分不可搜索，或者挂载只读的文件系统时，没有指定 MS_RDONLY 标志。
EAGAIN：成功地将不处于忙状态的文件系统标志为过时。
EBUSY：一. 源文件系统已被挂上。或者不可以以只读的方式重新挂载，因为它还拥有以写方式打开的文件。二. 目标处于忙状态。
EFAULT： 内存空间访问出错。
EINVAL：操作无效，可能是源文件系统超级块无效。
ELOOP ：路径解析的过程中存在太多的符号连接。
EMFILE：无需块设备要求的情况下，无用设备表已满。
ENAMETOOLONG：路径名超出可允许的长度。
ENODEV：内核不支持某中文件系统。
ENOENT：路径名部分内容表示的目录不存在。
ENOMEM： 核心内存不足。
ENOTBLK：source不是块设备。
ENOTDIR：路径名的部分内容不是目录。
EPERM ： 调用者权能不足。
ENXIO：块主设备号超出所允许的范围。
*/
INT32 Cmount(const CHAR *source, const CHAR *target, const CHAR *filesystemtype, ULONG mountflags, const VOID *data);
INT32 Cumount(const CHAR *target);
INT32 Cumount2(const CHAR *target, INT32 flags);

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

/*SIGNAL*/
/*

#define SIG_ERR	((__sighandler_t) -1)		//Error return.  
#define SIG_DFL	((__sighandler_t) 0)		// Default action.  
#define SIG_IGN	((__sighandler_t) 1)		// Ignore signal.  
 
#ifdef __USE_UNIX98
# define SIG_HOLD	((__sighandler_t) 2)	// Add signal to hold mask.
#endif
 
 
 Signals. 
#define	SIGHUP		1	 Hangup (POSIX). 
#define	SIGINT		2	 Interrupt (ANSI).  
#define	SIGQUIT		3	 Quit (POSIX).  
#define	SIGILL		4	 Illegal instruction (ANSI).  
#define	SIGTRAP		5	 Trace trap (POSIX).  
#define	SIGABRT		6	 Abort (ANSI).  
#define	SIGIOT		6	 IOT trap (4.2 BSD).  
#define	SIGBUS		7	 BUS error (4.2 BSD).  
#define	SIGFPE		8	 Floating-point exception (ANSI).  
#define	SIGKILL		9	 Kill, unblockable (POSIX).  
#define	SIGUSR1		10	 User-defined signal 1 (POSIX).  
#define	SIGSEGV		11	 Segmentation violation (ANSI).  
#define	SIGUSR2		12	 User-defined signal 2 (POSIX).  
#define	SIGPIPE		13	 Broken pipe (POSIX). 
#define	SIGALRM		14	 Alarm clock (POSIX). 
#define	SIGTERM		15	 Termination (ANSI). 
#define	SIGSTKFLT	16	 Stack fault. 
#define	SIGCLD		SIGCHLD	 Same as SIGCHLD (System V). 
#define	SIGCHLD		17	 Child status has changed (POSIX). 
#define	SIGCONT		18	 Continue (POSIX). 
#define	SIGSTOP		19	 Stop, unblockable (POSIX).
#define	SIGTSTP		20	 Keyboard stop (POSIX). 
#define	SIGTTIN		21	 Background read from tty (POSIX).
#define	SIGTTOU		22	 Background write to tty (POSIX).  
#define	SIGURG		23	 Urgent condition on socket (4.2 BSD).
#define	SIGXCPU		24	 CPU limit exceeded (4.2 BSD).
#define	SIGXFSZ		25	 File size limit exceeded (4.2 BSD).
#define	SIGVTALRM	26	 Virtual alarm clock (4.2 BSD).
#define	SIGPROF		27	Profiling alarm clock (4.2 BSD).
#define	SIGWINCH	28	 Window size change (4.3 BSD, Sun).
#define	SIGPOLL		SIGIO	 Pollable event occurred (System V).
#define	SIGIO		29	 I/O now possible (4.2 BSD).
#define	SIGPWR		30	 Power failure restart (System V).  
#define SIGSYS		31	 Bad system call.  
#define SIGUNUSED	31
#define	_NSIG		65	//Biggest signal number + 1
*/

//SIG_FUNC* Csignal(INT32 signo, SIG_FUNC* func);
INT32 Ckill(pid_t pid, INT32 signo);
INT32 Craise(INT32 signo);
UINT32 Calarm(UINT32 seconds);
INT32 Cpause(VOID);
INT32 Csigemptyset(sigset_t *set);
INT32 Csigfillset(sigset_t *set);
INT32 Csigaddset(sigset_t *set, INT32 signo);
INT32 Csigdelset(sigset_t *set, INT32 signo);
bool  Csigismember(const sigset_t *set, INT32 signo);
INT32 Csigprocmask(INT32 how, const sigset_t *set, sigset_t *oset);//for noly single thread
INT32 Csigpending(sigset_t *set);
/*
struct sigaction {
    void (*sa_handler)(int); //add for signal handler or SIG_IGN, SIG_DFL
    sigset_t sa_mask;        //additional signals to block
    int      sa_flags;       //signal options, Figure 10.16
    //alternate handler
    void (*sa_sigaction)(int, siginfo_t* void*);
};
sa_flag是一个选项，主要理解两个
SA_INTERRUPT 由此信号中断的系统调用不会自动重启
SA_RESTART 由此信号中断的系统调用会自动重启
SA_SIGINFO 提供附加信息，一个指向siginfo结构的指针以及一个指向进程上下文标识符的指针
SA_NOCLDSTOP 若signo是SICHLD
SA_NOCLDWAIT 若signo是SICHLD
SA_NODEFER
SA_ONSTACK
SA_RESETHAND
*/
INT32 Csigaction(INT32 signo, const struct sigaction* act, struct sigaction* oact);
SIG_FUNC *Csignal(INT32 signo, SIG_FUNC* func);
INT32 Csigsuspend(const sigset_t *sigmask);
VOID Cpsignal(INT32 signo, const CHAR* msg);
VOID Cpsiginfo(const siginfo_t *info, const CHAR* msg);
CHAR* Cstrsignal(INT32 signo);
VOID Cabort(VOID);
INT32 Cnanosleep(const struct timespec *reqtp, timespec *remtp);
INT32 Cclock_nanosleep(clockid_t clock_id, INT32 flags, const struct timespec *reqtp, timespec *remtp);
/*********************************IOSC**********************************/
pid_t Csetsid(VOID);
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
INT32 Cgetrlimit(INT32 resource, struct rlimit *rlptr);
INT32 Csetrlimit(INT32 resource, const struct rlimit *rlptr);

INT32 Cgeterrno();
CHAR* Cstrerror(INT32 Cerrno);
bool FindPattern(const CHAR* pContent, const CHAR* pPattern, INT32 &nStart, INT32 &nEnd);
bool DoComman(const CHAR* cmd, const CHAR* type, CHAR* result, size_t len);
bool GetIPFromIfconfig(const CHAR* cmd_buff, const CHAR* partten, CHAR IP[], INT32 IP_len);
bool getLocalTime(CHAR* mtime, INT32 len);
bool RemoveCRLF(CHAR* data);
bool Daemonize(const CHAR* cmd);


typedef enum
{
MSG_USB_REMOVE = 0,
MSG_USB_ADD,
MSG_USB_NOT_MOUNTED,
MSG_OTHER_EVENT,
MSG_SHOW_TIME,
MSG_UPDATE_IP,
MSG_SHOW_IP,
MSG_WRITE_MSG_TO_DISK_OK,
MSG_WRITE_MSG_TO_DISK_FAIL,
MSG_UNKNOW_EVENT,
MSG_INVALIED,
MSG_EVENT_MAX
}MSG_EVENT;

#endif
