#include "mpsApplicationInfo.h"

// Information used to override defaults read from hostname(),
// and the FW configuration. These are used for testing purposes only.
char* mpsCpuName    = MPS_USE_DEFAULT_CPU_NAME;
int   mpsCrateId    = MPS_USE_DEFAULT_CRATE_ID;
int   mpsSlotNumber = MPS_USE_DEFAULT_SLOT_NUMBER;

int setMpsApplicationInfo(const char* cpuName, int crateId, int slotNumber)
{
  printf("Setting application crate id to: '%d'\n", crateId);
  mpsCrateId = crateId;

  printf("Setting application slot number to: '%d'\n", slotNumber);
  mpsSlotNumber = slotNumber;

  if ( ( ! cpuName ) || ( cpuName[0] == '\0' ) )
    {
      fprintf( stderr, "Error: specified cpuName for application is empty. Ignoring it.\n" );
    }
  else
    {
      printf("Setting application cpuName to: '%s'.\n", cpuName);
      mpsCpuName = (char*) malloc( strlen(cpuName) * sizeof(char) );
      strcpy(mpsCpuName, cpuName);
    }

  return 0;
}

void getMpsApplicationInfo(char** cpuName, int* crateId, int* slotNumber)
{
    *cpuName = (char*) malloc( strlen(mpsCpuName) * sizeof(char) );
    strcpy(*cpuName, mpsCpuName);
    *crateId = mpsCrateId;
    *slotNumber = mpsSlotNumber;
}


/* The setMpsApplication* functions are exposed to the IOC shell
   so that the user can call it from st.cmd */
static const iocshArg setMpsApplicationInfoArgs0 = { "MpsApplicationInfoCpuName",    iocshArgString };
static const iocshArg setMpsApplicationInfoArgs1 = { "MpsApplicationInfoCrateId",    iocshArgInt    };
static const iocshArg setMpsApplicationInfoArgs2 = { "MpsApplicationInfoSlotNumber", iocshArgInt    };

static const iocshArg * const setMpsApplicationInfoArgs[] =
{
  &setMpsApplicationInfoArgs0,
  &setMpsApplicationInfoArgs1,
  &setMpsApplicationInfoArgs2
};
 
static const iocshFuncDef setMpsApplicationInfoFuncDef = { "L2MPSASYNSetApplicationInfo", 3, setMpsApplicationInfoArgs };

static void setMpsApplicationInfoCallFunc(const iocshArgBuf *args)
{
  setMpsApplicationInfo(args[0].sval, args[1].ival, args[2].ival);
}

void setMpsApplicationInfoRegister(void)
{
   iocshRegister(&setMpsApplicationInfoFuncDef, setMpsApplicationInfoCallFunc);
}

epicsExportRegistrar(setMpsApplicationInfoRegister);
