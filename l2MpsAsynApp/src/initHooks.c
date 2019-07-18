/* initHooks.c  ioc initialization hooks */

#include <stdio.h>
#include <initHooks.h>
#include <epicsPrint.h>
#include <iocsh.h>
#include <epicsExport.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "thresholds.h"

/*
 * INITHOOKS
 *
 * called by iocInit at various points during initialization
 *
 */

static void printErrorMessage(char *extraInfo) {
    printf("ERROR: Failed to restore application MPS thresholds.\n");
    printf("       Thresholds must be restored in order to enable MPS!\n");
    if (strlen(extraInfo) > 0) {
        printf("%s\n", extraInfo);
    }
}

// MMPS manager hostname and port number, and application ID
char*    mpsManagerHostName   = "lcls-daemon2";
int      mpsManagerPortNumber = 1975;
uint16_t mpsManagerAppId      = 0;

/* If this function (initHooks) is loaded, iocInit calls this function
 * at certain defined points during IOC initialization */
static void l2MpsAsynInitHooks(initHookState state)
{
    char errorMessage[300];
    char *str;
    int len;

    switch (state)
    {
        case initHookAtEnd :

            printf("l2MpsAsynInitHooks: Trying to restore thresholds from '%s:%d' for appId: '%d'...\n", mpsManagerHostName, mpsManagerPortNumber, mpsManagerAppId);

            if (restoreThresholds(mpsManagerHostName, mpsManagerPortNumber, mpsManagerAppId) != 0)
            {
                printErrorMessage("");
            }
            else
            {
                printf("Success. MPS thresholds were restored.\n");
            }

            break;

        default:
            break;
    }
    return;
}

/* Function to set the application ID used as identification on
   the MPS manager */
void l2MpsAsynSetManagerAppId(const uint16_t id)
{
   printf("Setting MPS Manager App ID to: '%" PRIu16 "'\n", id);

   mpsManagerAppId = id;
}

/* Function to override the default MPS manager host name and port
   number from the IOC shell */

int l2MpsAsynSetManagerHost(const char* host, int port)
{
    if ( ( ! host ) || ( host[0] == '\0' ) )
    {
        fprintf( stderr, "Error: MPS Manager hostname is empty\n" );
        return 1;
    }

    if ( port <= 0 )
    {
        fprintf( stderr, "Error: MPS Manager port number is invalid\n" );
        return 1;
    }

    printf("Setting MPS Manager host to: '%s', and port number to '%d'\n", host, port);

    mpsManagerHostName = (char*) malloc( strlen(host) * sizeof(char) );
    strcpy(mpsManagerHostName, host);
    mpsManagerPortNumber = port;

    return 0;
}

static const iocshArg setManagerHostArgs0 = { "MpsManagerHostName",   iocshArgString };
static const iocshArg setManagerHostArgs1 = { "MpsManagerPortNumber", iocshArgInt    };

static const iocshArg * const setManagerHostArgs[] =
{
    &setManagerHostArgs0,
    &setManagerHostArgs1
};

static const iocshFuncDef setManaherHostFuncDef = { "l2MpsAsynSetManagerHost", 2, setManagerHostArgs };

static void setManaherHostCallFunc(const iocshArgBuf *args)
{
    l2MpsAsynSetManagerHost(args[0].sval, args[1].ival);
}

void l2MpsAsynInitHooksRegister(void)
{
   initHookRegister(l2MpsAsynInitHooks);
   iocshRegister(&setManaherHostFuncDef, setManaherHostCallFunc);
}

epicsExportRegistrar(l2MpsAsynInitHooksRegister);
