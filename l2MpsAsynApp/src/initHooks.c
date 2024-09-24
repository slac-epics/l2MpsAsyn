/* initHooks.c  ioc initialization hooks */

#include <stdio.h>
#include <initHooks.h>
#include "mpsManagerInfo.h"

/*
 * INITHOOKS
 *
 * called by iocInit at various points during initialization
 *
 */

/* If this function (initHooks) is loaded, iocInit calls this function
 * at certain defined points during IOC initialization */
static void l2MpsAsynInitHooks(initHookState state)
{
    switch (state)
    {
        case initHookAfterIocRunning  :
            ;
            mpsManagerRestoreThresholds();
            
            break;

        default:
            break;
    }
    return;
}

void l2MpsAsynInitHooksRegister(void)
{
   initHookRegister(l2MpsAsynInitHooks);
}

epicsExportRegistrar(l2MpsAsynInitHooksRegister);
