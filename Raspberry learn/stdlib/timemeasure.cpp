#include "timemeasure.h"
#include "common.h"

TimeMeasure::TimeMeasure()
{
    clktck = sysconf(_SC_CLK_TCK);
    if(clktck < 0)
    {
        DBG("TimeMeasure() init failer!!");
    }
}
TimeMeasure::~TimeMeasure()
{

}
bool TimeMeasure::begin()
{
    if((start = times(&tmsstart)) < 0)
    {
        DBG("TimeMeasure::begin() fail");
        return false;
    }
    return true;
}
bool TimeMeasure::end()
{
    if((endtm = times(&tmsend)) < 0)
    {
        DBG("TimeMeasure::end() fail");
        return false;
    }
    return true;
}
DOUBLE TimeMeasure::getRealTime()
{
    return (DOUBLE) ((endtm - start) / (DOUBLE)clktck);
}
DOUBLE TimeMeasure::getUserTime()
{
    return (DOUBLE) ((tmsend.tms_utime - tmsstart.tms_utime) / (DOUBLE)clktck);
}

DOUBLE TimeMeasure::getSysTime()
{
    return (DOUBLE) ((tmsend.tms_stime - tmsstart.tms_stime) / (DOUBLE)clktck);
}
DOUBLE TimeMeasure::getChdUserTime()
{
    return (DOUBLE) ((tmsend.tms_cutime - tmsstart.tms_cutime) / (DOUBLE)clktck); 
}
DOUBLE TimeMeasure::getChdSysTime()
{
    return (DOUBLE) ((tmsend.tms_cstime - tmsstart.tms_cstime) / (DOUBLE)clktck); 
}
VOID TimeMeasure::printAllTime()
{
    DBG("real: %7.2f",getRealTime());
    DBG("user: %7.2f",getUserTime());
    DBG("sys: %7.2f",getSysTime());
    DBG("user chd: %7.2f",getChdUserTime());
    DBG("sys chd: %7.2f",getChdSysTime());
}



