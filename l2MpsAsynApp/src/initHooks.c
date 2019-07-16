/* initHooks.c	ioc initialization hooks */ 

#include <stdio.h>
#include <initHooks.h>
#include <epicsPrint.h>
#include <iocsh.h>
#include <epicsExport.h>
#include <stdlib.h>

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
  char *str;
   switch (state) {
   case initHookAtBeginning :
     break;
   case initHookAfterCallbackInit :
     break;
   case initHookAfterCaLinkInit :
     break;
   case initHookAfterInitDrvSup :
     break;
   case initHookAfterInitRecSup :
     break;
   case initHookAfterInitDevSup :
     break;
   case initHookAfterInitDatabase :
     break;
   case initHookAfterFinishDevSup :
     break;
   case initHookAfterScanInit :
     break;
   case initHookAfterInitialProcess :
     break;
   case initHookAfterInterruptAccept :
     break;
   case initHookAtEnd :
     str = getenv("MPS_ANA_APP_ID");
     restoreThresholds(str);
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
