#ifndef CFILE_H
#define CFILE_H

#include "common.h"

//STDIN_FILENO
//STDOUT_FILENO
//STDERR_FILENO

/*open mode:
r/rb        O_RDONLY
w/wb        creat a new file len = 0 or for write, O_WRONLY|O_CREAT|O_TRUNC
a/ab        append O_WRONLY|O_CREAT|O_APPEND
r+/r+b/rb+  for read and write O_RDWR
w+/w+b/wb+  create a new file len = 0 or for read and write O_RDWR|O_CREAT|O_TRUNC
a+/a+b/ab+  append or create for read and write
*/
/*stdin/stdout/stderr*/
class Cfile
{
public:
    Cfile(const CHAR* fileName, const CHAR* type = "a+");
    Cfile(const CHAR* fileName, const CHAR* type, FILE* fp);
    Cfile(INT32 fd, const CHAR* type = "a+");
    Cfile(VOID* buf, size_t size, const CHAR* type = "a+");/*for memory stream*/
    Cfile();
    ~Cfile();
    bool Cfopen(const CHAR* pathname, const CHAR* type = "a+");
    bool Cfreopen(const CHAR* pathname, const CHAR* type, FILE* fp);
    bool Cfdopen(INT32 fd, const CHAR* type = "a+");
    bool Cfmemopen(VOID* buf, size_t size, const CHAR* type = "a+");
    INT32 Cfwide(INT32 mode/*<0 ,byte, =0,noset, >0 Wide byte*/);
    VOID Csetbuf(CHAR* buf/*buf size must be BUFSIZ*/);
    INT32 Csetvbuf(CHAR* buf, INT32 mode/*_IOFBF/_IOLBF/_IONBF*/, size_t size/*buf size*/);
    INT32 Cfflush();
    bool isFileopenSuccess();
    INT32 Cfclose();
    INT32 Cfgetc();
    INT32 Cferror();
    INT32 Cfeof();
    VOID Cclearerr();
    INT32 Cungetc(INT32 c);
    CHAR* Cfgets(CHAR* buf, INT32 n);
    INT32 Cfputc(INT32 c);
    INT32 Cfputs(const CHAR* str);
    size_t Cfread(VOID* buf, size_t size, size_t nobj);
    size_t Cfwrite(const VOID* buf, size_t size, size_t nobj);
    LONG Cftell();
    INT32 Cfseek(LONG offset, INT32 whence/*SEEK_SET SEEK_CUR SEEK_END*/);
    VOID Crewind();
    off_t Cftello();
    off_t Cfseeko(off_t offset, INT32 whence/*SEEK_SET SEEK_CUR SEEK_END*/);
    INT32 Cfgetpos(fpos_t* pos);
    INT32 Cfsetpos(const fpos_t* pos);
    FILE* getFILE();
    INT32 getFILEno();
    INT32 getErrno(VOID);
private:
    VOID setErrno(INT32 merrno);
    FILE *file;
    
    INT32 Cerrno;
    INT32 Terrno;
};

#endif
