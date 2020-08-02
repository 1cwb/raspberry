#include "cfile.h"
#include <wchar.h>
#include <errno.h>
INT32 Cfile::getErrno(VOID)
{
    Terrno = Cerrno;
    Cerrno = 0;
    return Terrno;
}

VOID Cfile::setErrno(INT32 merrno)
{
    Cerrno = merrno;
}
Cfile::Cfile() : file(NULL), Cerrno(0), Terrno(0)
{

}

Cfile::Cfile(VOID* buf, size_t size, const CHAR* type)/*for memory stream*/
{
    Cfmemopen(buf, size, type);
}

bool Cfile::Cfmemopen(VOID* buf, size_t size, const CHAR* type)
{
    if(file != NULL)
    {
        DBG("file is open, please close it first");
        return false;
    }
    file = fmemopen(buf, size, type);
    if(file == NULL)
    {
	    DBG("ERROR! file is NULL");
        setErrno(errno);
        return false;
    }
    return true;
}

Cfile::Cfile(const CHAR* fileName, const CHAR* type) : file(NULL), Cerrno(0), Terrno(0)
{
    Cfopen(fileName, type);
}
Cfile::Cfile(const CHAR* fileName, const CHAR* type, FILE* fp) : file(NULL), Cerrno(0), Terrno(0)
{
    Cfreopen(fileName, type, fp);
}
Cfile::Cfile(INT32 fd, const CHAR* type) : file(NULL), Cerrno(0), Terrno(0)
{
    Cfdopen(fd, type);
}
Cfile::~Cfile()
{
    Cfclose();
}
bool Cfile::Cfopen(const CHAR* pathname, const CHAR* type)
{
    if(file != NULL)
    {
        DBG("file is open, please close it first");
        return false;
    }
    file = fopen(pathname, type);
    if(file == NULL)
    {
	DBG("file is NULL");
        setErrno(errno);
        return false;
    }
    return true;
}
bool Cfile::Cfreopen(const CHAR* pathname, const CHAR* type, FILE* fp)
{
    if(file != NULL)
    {
        DBG("file is open, please close it first");
        return false;
    }
    file = freopen(pathname, type, fp);
    if(file == NULL)
    {
        setErrno(errno);
        return false;
    }
    return true;
}
bool Cfile::Cfdopen(INT32 fd, const CHAR* type)
{
    if(file != NULL)
    {
        DBG("file is open, please close it first");
        return false;
    }
    file = fdopen(fd, type);
    if(file == NULL)
    {
        setErrno(errno);
        return false;
    }
    return true;
}
INT32 Cfile::Cfwide(INT32 mode/*<0 ,byte, =0,noset, >0 Wide byte*/)
{
    INT32 res = fwide(file ,mode/*<0 ,byte, =0,noset, >0 Wide byte*/);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
VOID Cfile::Csetbuf(CHAR* buf/*buf size must be BUFSIZ*/)
{
    setbuf(file,buf/*buf size must be BUFSIZ*/);
}
INT32 Cfile::Csetvbuf(CHAR* buf, INT32 mode/*_IOFBF/_IOLBF/_IONBF*/, size_t size/*buf size*/)
{
    INT32 res = setvbuf(file, buf, mode/*_IOFBF/_IOLBF/_IONBF*/,size/*buf size*/);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
INT32 Cfile::Cfflush()
{
    INT32 res = fflush(file);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
bool Cfile::isFileopenSuccess()
{
    return file == NULL ? false : true;
}
INT32 Cfile::Cfclose()
{
    if(file != NULL)
    {
        fclose(file);
        file = NULL;
    }
    return 0;
}
INT32 Cfile::Cfgetc()
{
    INT32 res = fgetc(file);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
INT32 Cfile::Cferror()
{
    return ferror(file);
}
INT32 Cfile::Cfeof()
{
    return feof(file);
}
VOID Cfile::Cclearerr()
{
    clearerr(file);
}
INT32 Cfile::Cungetc(INT32 c)
{
    INT32 res = ungetc(c, file);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
CHAR* Cfile::Cfgets(CHAR* buf, INT32 n)
{
    CHAR* p = fgets(buf, n, file);
    if(p == NULL)
    {
        setErrno(errno);
    }
    return p;
}
INT32 Cfile::Cfputc(INT32 c)
{
    INT32 res = fputc(c, file);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
INT32 Cfile::Cfputs(const CHAR* str)
{
    INT32 res = fputs(str, file);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
size_t Cfile::Cfread(VOID* buf, size_t size, size_t nobj)
{
    size_t res = fread(buf, size, nobj, file);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
size_t Cfile::Cfwrite(const VOID* buf, size_t size, size_t nobj)
{
    size_t res = fwrite( buf, size, nobj, file);
    if(res < 0)
    {
        setErrno(errno);
    }
    return res;
}
LONG Cfile::Cftell()
{
    return ftell(file);
}
INT32 Cfile::Cfseek(LONG offset, INT32 whence/*SEEK_SET SEEK_CUR SEEK_END*/)
{
    return fseek(file, offset, whence/*SEEK_SET SEEK_CUR SEEK_END*/);
}
VOID Cfile::Crewind()
{
    rewind(file);
}
off_t Cfile::Cftello()
{
    return ftello(file);
}
off_t Cfile::Cfseeko(off_t offset, INT32 whence/*SEEK_SET SEEK_CUR SEEK_END*/)
{
    return fseeko(file, offset, whence/*SEEK_SET SEEK_CUR SEEK_END*/);
}
INT32 Cfile::Cfgetpos(fpos_t* pos)
{
    return fgetpos(file, pos);
}
INT32 Cfile::Cfsetpos(const fpos_t* pos)
{
    return fsetpos(file, pos);
}
FILE* Cfile::getFILE()
{
    if(file == NULL)
    {
    	DBG("file == NULL");
    }
    return file;
}
INT32 Cfile::getFILEno()
{
    return fileno(file);
}