#include <stdio.h>
#include <epicsPrint.h>
#include <iocsh.h>
#include <epicsExport.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

// This function is used change the default MPS manager
// hostname and port number.
int setMpsManagerHost(const char* host, int port);

// This function is used to set the Application ID used
// when contacting the MPS Manager.
int setMpsManagerAppId(const uint16_t id);

// This function is used to get the information needed to contact the
// MPS Manager.
void getMpsManagerInfo(const char** host, int* port, uint16_t* id);