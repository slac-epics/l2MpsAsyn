#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cadef.h>
#include <caerr.h>
#include <db_access.h>

int pvPut(const char* pv_name, dbr_double_t value);
dbr_double_t pvGet(const char* pv_name);

int restoreThresholds(const char *fault_name,const char* prefix,int numThr,int alt,int idl,int nc);
