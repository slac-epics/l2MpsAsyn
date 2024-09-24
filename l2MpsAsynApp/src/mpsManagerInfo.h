#include <epicsPrint.h>
#include <iocsh.h>
#include <epicsExport.h>
#include <inttypes.h>
#include "thresholds.h"

// This function is used to set the Application ID used
// when contacting the MPS Manager.
int setMpsManagerAppId(const uint16_t id);

// This function is used to set the NC present flag to false for
// threshold restoration
int setMpsManagerNcFalse();

// This function registers a fault with manager so thresholds can
// be restored
int registerMpsManagerFault(const char* fault);

// Some app types do not restore thresholds but need MpsEnable set to true.
int setMpsManagerRestoreFalse();

int setMpsManagerPrefix(const char* pre);

void mpsManagerRestoreThresholds();