#include "mpsManagerInfo.h"

// Information needed to contact the MPS Manager.
// This are the default values which can be changes
// using the provided functions.
uint16_t mpsManagerAppId      = 0;
char*    mpsAppPrefix         = "MPLN:GUNB:MP10:1";
char*    mpsFaultNames[12]    = {"","","","","","","","","","","",""};
int      mpsThrNums[12]       = {0,0,0,0,0,0,0,0,0,0,0,0};
int      mpsAlt[12]           = {0,0,0,0,0,0,0,0,0,0,0,0};
int      mpsIdl[12]           = {1,1,1,1,1,1,1,1,1,1,1,1};
int      mpsNc[12]            = {1,1,1,1,1,1,1,1,1,1,1,1};
int      mpsRestore           = 1;
int      thr_count            = 0;

static void printErrorMessage(char *extraInfo) {
    printf("l2MpsAsynInitHooks ERROR:\n");
    printf("    Failed to restore application MPS thresholds.\n");
    printf("    Thresholds must be restored in order to enable MPS!\n");
    if (strlen(extraInfo) > 0) {
        printf("%s\n", extraInfo);
    }
}

static int enableMps() {
    char *prefix=NULL;
    char *PV[256];
    char *PV_RBV[256];
    int ret;
    getMpsManagerPrefix(&prefix);
    snprintf(PV,sizeof(PV),"%s:THR_LOADED",prefix);
    if (pvPut(PV,1) < 0) {
        printf("L2MPSASYN: Failed to set %s\n",PV);
    }    
    snprintf(PV, sizeof(PV), "%s:MPS_EN",prefix);
    snprintf(PV_RBV, sizeof(PV_RBV), "%s:MPS_EN_RBV",prefix);
    ret = pvPut(PV, 1.0);
    if (ret > -1) {
        printf("L2MPSASYN: Successfully enabled MPS\n");
    }
    else {
        printErrorMessage("Could not write MPS_EN PV!");
    }
    return 0;
}

static int checkAppId(uint16_t aid) {
    char *prefix=NULL;
    char *PV[256];
    getMpsManagerPrefix(&prefix);
    uint16_t aid_mgr = 0;
    if (aid > 0) {
        snprintf(PV, sizeof(PV), "%s:APP_ID_MGR",prefix);
        dbr_double_t aid_mgr_d = pvGet(PV);
        aid_mgr = (uint16_t)aid_mgr_d;
    }
    if(aid == aid_mgr) {
        return 1;
    }
    else {
        return 0;
    }
}

int setMpsManagerAppId(const uint16_t id)
{
    printf("Setting MPS Manager App ID to: '%" PRIu16 "'\n", id);

    mpsManagerAppId = id;

    return 0;
}

uint16_t getMpsManagerAppId() {
    return mpsManagerAppId;
}

int getMpsRestore() {
    return mpsRestore;
}

int setMpsManagerNcFalse() {
    int i;
    for (i = 0; i < 12; i++)
    {
        mpsNc[i] = 0;
    }
    return 0;
}

int setMpsManagerRestoreFalse() {
    mpsRestore = 0;
    return 0;
}

int setMpsManagerPrefix(const char* pre) {
    if ( ( ! pre ) || ( pre[0] == '\0' ) ){
        fprintf( stderr, "ERROR: L2MPSASYN - MPS Prefix must be defined\n");
    }
    else {
        mpsAppPrefix = (char*) malloc(strlen(pre) * sizeof(char));
        strcpy(mpsAppPrefix,pre);
        printf("L2MPSASYN: Setting MPS Prefix to: %s\n",mpsAppPrefix);
    }
    return 0;
}

void getMpsManagerPrefix(char** pre) {
    *pre = (char*) malloc(strlen(mpsAppPrefix)*sizeof(char));
    strcpy(*pre, mpsAppPrefix);
}

int registerMpsManagerFault(const char* fault)
{
    if ( ( ! fault ) || ( fault[0] == '\0' ) )
    {
        fprintf(stderr, "ERROR: Cannot register empty MPS fault.\n");
    }
    else if ( strlen(mpsFaultNames[thr_count]) > 0 )
    {
        fprintf(stderr, "ERROR: MPS Fault Channel %i already registered.\n", thr_count);
    }
    else 
    {
        int thrNum = 0;
        if ( (strstr(fault,"BPMS") && strstr(fault,":X")) || (strstr(fault,"BPMS") && strstr(fault,":Y")) )
        {
            thrNum = 2;
            mpsAlt[thr_count] = 1;
            mpsIdl[thr_count] = 0;
        }
        else
        {
            thrNum = 8;
        }
        mpsFaultNames[thr_count] = (char*) malloc( strlen(fault) * sizeof(char) );
        strcpy(mpsFaultNames[thr_count],fault);
        mpsThrNums[thr_count] = thrNum;
        printf("Registering MPS Fault %s with %i thresholds\n",mpsFaultNames[thr_count],mpsThrNums[thr_count]);
        thr_count = thr_count + 1;
    }
}

void getMpsManagerInfo(int chan, char** fault, int* thr, int* alt, int* idl, int* nc, int* restore)
{
    *fault = mpsFaultNames[chan];
    *thr = mpsThrNums[chan];
    *alt = mpsAlt[chan];
    *idl = mpsIdl[chan];
    *nc = mpsNc[chan];
    *restore = mpsRestore;
}

void mpsManagerRestoreThresholds() {
    ca_context_create(ca_enable_preemptive_callback);
    int cont = checkAppId(getMpsManagerAppId());
    if (cont) {
        char *prefix = NULL;
        getMpsManagerPrefix(&prefix);
        int err = 0;
        if (getMpsRestore()) {
            int i;
            for (i = 0; i < 12; i++) {
                char     *mpsFaultName = NULL;
                int      mpsThrNum;
                int      mpsAlt;
                int      mpsIdl;
                int      mpsNc;
                int      mpsRestore;
                getMpsManagerInfo(i,&mpsFaultName, &mpsThrNum, &mpsAlt, &mpsIdl, &mpsNc, &mpsRestore);
                if (strlen(mpsFaultName) > 0) {
                    if (restoreThresholds(mpsFaultName,prefix,mpsThrNum,mpsAlt,mpsIdl,mpsNc) != 0) {
                        err++;
                    }
                }                    
            }
        }
        if (err == 0) {
            enableMps();
        }
        else {
        printErrorMessage("\t\tFailed to restore all thresholds");
        }  
    }
    else {
        printErrorMessage("\t\tAPP ID is not correct");
    }   
}

static const iocshArg * const restoreThreshArgs[] = {};
static const iocshFuncDef restoreThreshFuncDef = {"L2MPSASYNRestoreThresholds",0,restoreThreshArgs};
static void restoreThreshCallFunc(const iocshArgBuf *args) {
    mpsManagerRestoreThresholds();
}
void restoreThreshFaultRegister (void) {
    iocshRegister(&restoreThreshFuncDef, restoreThreshCallFunc);
}


/* The registerMpsManagerFault is exposed to the IOC shell
   so that the user can call it from st.cmd */
static const iocshArg registerMpsManagerFaultArgs0 = { "MpsManagerFaultName",   iocshArgString };

static const iocshArg * const registerMpsManagerFaultArgs[] =
{
    &registerMpsManagerFaultArgs0
};

static const iocshFuncDef registerMpsManagerFaultFuncDef = { "L2MPSASYNRegisterFault", 1, registerMpsManagerFaultArgs };

static void registerMpsManagerFaultCallFunc(const iocshArgBuf *args)
{
    registerMpsManagerFault(args[0].sval);
}

void registerMpsManagerFaultRegister(void)
{
   iocshRegister(&registerMpsManagerFaultFuncDef, registerMpsManagerFaultCallFunc);
}

epicsExportRegistrar(registerMpsManagerFaultRegister);
epicsExportRegistrar(restoreThreshFaultRegister);

