#include "csignal.h"
#include <string.h>
Csignal::Csignal()
{
    alreadyUsedSpace = 0;
    totalSiganlSpace = sizeof(sigfunc) / sizeof(sig_st);
    memset(sigfunc, 0, sizeof(sigfunc));
}
Csignal::~Csignal()
{

}
bool Csignal::catchSignal(INT32 signo, SIG_FUNC* func)
{
    if(alreadyUsedSpace == totalSiganlSpace)
    {
        DBG("Error! signal Buff is Full!");
        return false;
    }
    for(INT32 i = 0; i < totalSiganlSpace; i++)
    {
        if(signo == sigfunc[i].signo)
        {
            DBG("signal %d  already be used",signo);
            return false;
        }
    }
    for(INT32 i = 0; i < totalSiganlSpace; i++)
    {
        if(sigfunc[i].signo == 0)
        {
            if(signal(signo, func) != SIG_ERR)
            {
                sigfunc[i].signo = signo;
                sigfunc[i].sigfunc = func;
                alreadyUsedSpace ++;
                return true;
            }
        }
    }
    return false;
}

    //INT32 totalSiganlSpace;
    //INT32 alreadyUsedSpace;
