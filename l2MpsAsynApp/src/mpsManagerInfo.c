#include "mpsManagerInfo.h"

// Information needed to contact the MPS Manager.
// This are the default values which can be changes
// using the provided functions.
char*    mpsManagerHostName   = "lcls-daemon2";
int      mpsManagerPortNumber = 1975;
uint16_t mpsManagerAppId      = 0;


int setMpsManagerAppId(const uint16_t id)
{
    printf("Setting MPS Manager App ID to: '%" PRIu16 "'\n", id);

    mpsManagerAppId = id;

    return 0;
}

int setMpsManagerHost(const char* host, int port)
{
    if ( ( ! host ) || ( host[0] == '\0' ) )
    {
        fprintf( stderr, "Error: MPS Manager hostname is empty. Ignoring it.\n" );
    }
    else
    {
        printf("Setting MPS Manager hostname to: '%s'.\n", host);
        mpsManagerHostName = (char*) malloc( strlen(host) * sizeof(char) );
        strcpy(mpsManagerHostName, host);
    }

    if ( port <= 0 )
    {
        fprintf( stderr, "Error: MPS Manager port number is invalid. Ignoring it.\n" );
    }
    else
    {
        printf("Setting MPS Manager port number to '%d'\n", port);
        mpsManagerPortNumber = port;
    }

    return 0;
}

void getMpsManagerInfo(const char** host, int* port, uint16_t* id)
{
    *host = (char*) malloc( strlen(mpsManagerHostName) * sizeof(char) );
    strcpy(*host, mpsManagerHostName);
    *port = mpsManagerPortNumber;
    *id = mpsManagerAppId;
}


/* The setMpsManagerHost is exposed to the IOC shell
   so that the user can call it from st.cmd */
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