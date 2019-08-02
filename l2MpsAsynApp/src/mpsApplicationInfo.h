#include <stdio.h>
#include <epicsPrint.h>
#include <iocsh.h>
#include <epicsExport.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#define MPS_USE_DEFAULT_CPU_NAME    "default"
#define MPS_USE_DEFAULT_CRATE_ID    -1
#define MPS_USE_DEFAULT_SLOT_NUMBER -1

// This function is used change the default CPU name, slot number and crate id
// that is used to locate the MPS database generated information
int setMpsApplicationInfo(const char* cpuName, int crateId, int slotNumber);

// This function is used to get the information needed to contact the
// MPS Manager.
void getMpsApplicationInfo(char** cpuName, int* crateId, int* slotNumber);
