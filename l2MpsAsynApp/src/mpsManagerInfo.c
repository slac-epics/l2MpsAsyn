#include "mpsManagerInfo.h"

// MMPS manager hostname and port number
char*    mpsManagerHostName   = "lcls-daemon2";
int      mpsManagerPortNumber = 1975;
uint16_t mpsManagerAppId      = 0;

int setMpsManagerAppId(const uint16_t id)
{
    printf("Setting MPS Manager App ID to: '%" PRIu16 "'\n", id);

    mpsManagerAppId = id;

    return 0;
}

/* Function to override the default MPS manager host name and port
 *    number from the IOC shell */

int setMpsManagerHost(const char* host, int port)
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

void getMpsManagerInfo(const char** host, int* port, uint16_t* id)
{
    *host = (char*) malloc( strlen(mpsManagerHostName) * sizeof(char) );
    strcpy(*host, mpsManagerHostName);
    *port = mpsManagerPortNumber;
    *id = mpsManagerAppId;
}


static const iocshArg setMpsManagerHostArgs0 = { "MpsManagerHostName",   iocshArgString };
static const iocshArg setMpsManagerHostArgs1 = { "MpsManagerPortNumber", iocshArgInt    };

static const iocshArg * const setMpsManagerHostArgs[] =
{
    &setMpsManagerHostArgs0,
    &setMpsManagerHostArgs1
};

static const iocshFuncDef setMpsManagerHostFuncDef = { "L2MPSASYNSetManagerHost", 2, setMpsManagerHostArgs };

static void setMpsManagerHostCallFunc(const iocshArgBuf *args)
{
    setMpsManagerHost(args[0].sval, args[1].ival);
}

void setMpsManagerRegister(void)
{
   iocshRegister(&setMpsManagerHostFuncDef, setMpsManagerHostCallFunc);
}

epicsExportRegistrar(setMpsManagerRegister);