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
#include "mpsManagerInfo.h"

/*
 * INITHOOKS
 *
 * called by iocInit at various points during initialization
 *
 */

static void printErrorMessage(char *extraInfo) {
    printf("l2MpsAsynInitHooks ERROR:\n");
    printf("    Failed to restore application MPS thresholds.\n");
    printf("    Thresholds must be restored in order to enable MPS!\n");
    if (strlen(extraInfo) > 0) {
        printf("%s\n", extraInfo);
    }
}

/* If this function (initHooks) is loaded, iocInit calls this function
 * at certain defined points during IOC initialization */
static void l2MpsAsynInitHooks(initHookState state)
{
    switch (state)
    {
        case initHookAtEnd :

            // Need an statement after a label which is not a definition
            // So, adding an empty statement here.
            ;

            // Get the MPS Manager information
            const char *host = NULL;
            int         port = 0;
            uint16_t    id   = 0;
            getMpsManagerInfo(&host, &port, &id);

            // Check if the application ID is valid.
            // Hostname and port number have already been checked.
            if (id == 0)
            {
                printErrorMessage("    Reason: Invalid application ID (0).");
                return;
            }

            // Try to restore the MPS threshold from the MPS Manager
            printf("l2MpsAsynInitHooks: Trying to restore thresholds from '%s:%d' for appId: '%d'...\n", host, port, id);
            if (restoreThresholds(host, port, id) != 0)
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

void l2MpsAsynInitHooksRegister(void)
{
   initHookRegister(l2MpsAsynInitHooks);
}

epicsExportRegistrar(l2MpsAsynInitHooksRegister);
