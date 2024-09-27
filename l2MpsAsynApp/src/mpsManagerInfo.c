#include "mpsManagerInfo.h"

// Information needed to contact the MPS Manager.
// This are the default values which can be changes
// using the provided functions.
uint16_t mpsManagerAppId       = 0;
char     mpsAppPrefix[20]      = "\0";
char     mpsFaultNames[12][50] = {"\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0"};
int      mpsThrNums[12]        = {0,0,0,0,0,0,0,0,0,0,0,0};
int      mpsAlt[12]            = {0,0,0,0,0,0,0,0,0,0,0,0};
int      mpsIdl[12]            = {1,1,1,1,1,1,1,1,1,1,1,1};
int      mpsNc[12]             = {1,1,1,1,1,1,1,1,1,1,1,1};
int      mpsRestore            = 1;
int      thr_count             = 0;

static void printErrorMessage(char *extraInfo) {
    printf("l2MpsAsynInitHooks ERROR:\n");
    printf("    Failed to restore application MPS thresholds.\n");
    printf("    Thresholds must be restored in order to enable MPS!\n");
    if (strlen(extraInfo) > 0) {
        printf("%s\n", extraInfo);
    }
}

static int enableMps() {
    char PV[256];
    char PV_RBV[256];
    int ret;   
    snprintf(PV, sizeof(PV), "%s:MPS_EN",mpsAppPrefix);
    snprintf(PV_RBV, sizeof(PV_RBV), "%s:MPS_EN_RBV",mpsAppPrefix);
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
    printf("L2MPSASYN: Working on App %i, %s...\n",mpsManagerAppId,mpsAppPrefix);
    char PV[256];
    uint16_t aid_mgr = 0;
    if (aid > 0) {
        snprintf(PV, sizeof(PV), "%s:APP_ID_MGR",mpsAppPrefix);
        double aid_mgr_d;
        if (pvGet(PV,&aid_mgr_d) < 0) {
            return 0;
        }
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
        strcpy(mpsAppPrefix,pre);
        printf("L2MPSASYN: Setting MPS Prefix to: %s\n",mpsAppPrefix);
    }
    return 0;
}

int registerMpsManagerFault(const char* fault)
{
    if ( ( ! fault ) || ( fault[0] == '\0' ) )
    {
        fprintf(stderr, "ERROR: Cannot register empty MPS fault.\n");
        return -1;
    }
    if (thr_count > 11) {
        fprintf(stderr, "ERROR: Too many MPS faults registered.\n");
        return -1;
    }
    int i;
    int cont = 1;
    for (i = 0; i < 12; i++) {
        if (strcmp(fault,mpsFaultNames[i]) == 0) {
            cont = 0;
        }
    }
    if ( !cont )
    {
        fprintf(stderr, "ERROR: MPS Fault %s already registered as.\n", fault);
        return -1;
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
        strcpy(mpsFaultNames[thr_count],fault);
        mpsThrNums[thr_count] = thrNum;
        printf("Registering MPS Fault %s with %i thresholds at index %i\n",mpsFaultNames[thr_count],mpsThrNums[thr_count],thr_count);
        thr_count = thr_count + 1;
        return 0;
    }
}
void mpsManagerRestoreThresholds() {
    ca_context_create(ca_enable_preemptive_callback);
    int cont = checkAppId(getMpsManagerAppId());
    if (cont > 0) {
        int err = 0;
        if (mpsRestore) {
            int i;
            for (i = 0; i < 12; i++) {
                if (strlen(mpsFaultNames[i]) > 0) {
                    if (restoreThresholds(mpsFaultNames[i],mpsAppPrefix,mpsThrNums[i],mpsAlt[i],mpsIdl[i],mpsNc[i]) != 0) {
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
