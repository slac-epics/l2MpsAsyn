/* initHooks.c  ioc initialization hooks */

#include <stdio.h>
#include <initHooks.h>
#include <epicsPrint.h>
#include <iocsh.h>
#include <epicsExport.h>
#include <stdlib.h>
#include <string.h>
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

/* If this function (initHooks) is loaded, iocInit calls this function
 * at certain defined points during IOC initialization */
static void l2MpsAsynInitHooks(initHookState state)
{
    char errorMessage[300];
    char *str;
    char *host;
    char *portStr;
    int len;

    switch (state)
    {
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
            host = getenv("MPS_MANAGER_HOST");
            portStr = getenv("MPS_MANAGER_PORT");

            str = getenv("MPS_ANA_APP_ID");
            if (str == NULL)
                {
                sprintf(errorMessage, "ERROR: MPS_ANA_APP_ID environment variable not defined.\n");
                printErrorMessage(errorMessage);
                return;
            }

            len = strlen(str);

            if (host == NULL)
            {
                printf("WARNING: MPS_MANAGER_HOST environment variable not defined.\n");
                printf("         Using lcls-daemon2 as host. This variable is defined by the MPS configuration.\n");
                host = "lcls-daemon2";
            }

            if (portStr == NULL)
            {
                printf("WARNING: MPS_MANAGER_PORT environment variable not defined.\n");
                printf("         Using 1975 as port. This variable is defined by the MPS configuration.\n");
                portStr = "1975";
            }

            // ApplicationId string from environment variable should not have more that 4 chars or
            // have none
            if (len == 0 || len > 4)
            {
                sprintf(errorMessage, "ERROR: Invalid application ID string (%s).\n", str);
                printErrorMessage(errorMessage);
                return;
            }

            char *endPtr;
            strtol(str, &endPtr, 0);
            if (endPtr == str)
            {
                sprintf(errorMessage,
                "ERROR: Invalid application ID. Cannot convert string \"%s\" to a number.\n", str);
                printErrorMessage(errorMessage);
                return;
            }
            int appId = atoi(str);

            strtol(portStr, &endPtr, 0);
            if (endPtr == portStr)
            {
                sprintf(errorMessage,
                "ERROR: Invalid server port. Cannot convert string \"%s\" to a number.\n", portStr);
                printErrorMessage(errorMessage);
                return;
            }
            int port = atoi(portStr);

            if (restoreThresholds(host, port, appId) != 0)
            {
                printErrorMessage("");
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
