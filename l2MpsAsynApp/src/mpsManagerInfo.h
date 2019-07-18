#include <stdio.h>
#include <epicsPrint.h>
#include <iocsh.h>
#include <epicsExport.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

int setMpsManagerHost(const char* host, int port);
int setMpsManagerAppId(const uint16_t id);

void getMpsManagerInfo(const char** host, int* port, uint16_t* id);