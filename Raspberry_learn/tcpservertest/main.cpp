#include <unistd.h>
#include "common.h"
#include "clog.h"
int main()
{
    Clog::getInstance()->Init(STDOUT_FILENO,LEVEL_DEBUG);
    LOG_DEBUG("hello world\n");
}
