#include <thresholds.h>

int pvPut(const char* pv_name, dbr_double_t value) {
    chid cid;
    SEVCHK(ca_create_channel(pv_name, NULL,NULL,10,&cid), "pvPut: Create channel failed\n");
    SEVCHK(ca_pend_io(1.0),"pvPut: Search failed\n");
    SEVCHK(ca_put(DBR_DOUBLE, cid, &value),"pvPut: Put Failed\n");
    SEVCHK(ca_pend_io(1.0), "pvPut: Pend I/O failed\n");
    SEVCHK(ca_clear_channel(cid),"pvPut: Clear channel failed\n");
    return 0;
}

dbr_double_t pvGet(const char* pv_name) {
    chid cid;
    dbr_double_t value;
    SEVCHK(ca_create_channel(pv_name, NULL,NULL,10,&cid), "pvGet: Create channel failed\n");
    SEVCHK(ca_pend_io(1.0),"pvGet: Search failed\n");
    SEVCHK(ca_get(DBR_DOUBLE, cid, &value),"pvGet: Get Failed\n");
    SEVCHK(ca_pend_io(1.0), "pvGet: Pend I/O failed\n");
    SEVCHK(ca_clear_channel(cid),"pvGet: Clear channel failed\n");
    return value;
}

int restoreThreshold(const char* thr_pv, const char* restore_pv) {
    dbr_double_t restore_val = pvGet(restore_pv);
    if (pvPut(thr_pv, restore_val) < 0) {
        printf("L2MPSAsyn: Failed to put restore val for %s\n",thr_pv);
       return -1;
    }
    return 0;
}

int buildPvNames(const char* fault_name, char* thr_name) {
    char thr_pv[256];
    char restore_pv[256];
    // L_EN
    snprintf(thr_pv, sizeof(thr_pv),"%s_%s_L_EN",fault_name,thr_name);
    snprintf(restore_pv,sizeof(restore_pv),"%s_%s_L_EN_MGR",fault_name,thr_name);
    if (restoreThreshold(thr_pv,restore_pv) < 0) {
        printf("L2MPSASYN: Failed to restore %s\n",thr_pv);
        return -1;
    }
    // H_EN
    snprintf(thr_pv, sizeof(thr_pv),"%s_%s_H_EN",fault_name,thr_name);
    snprintf(restore_pv,sizeof(restore_pv),"%s_%s_H_EN_MGR",fault_name,thr_name);
    if (restoreThreshold(thr_pv,restore_pv) < 0) {
        printf("L2MPSASYN: Failed to restore %s\n",thr_pv);
        return -1;
    }
    // L
    snprintf(thr_pv, sizeof(thr_pv),"%s_%s_L",fault_name,thr_name);
    snprintf(restore_pv,sizeof(restore_pv),"%s_%s_L_MGR",fault_name,thr_name);
    if (restoreThreshold(thr_pv,restore_pv) < 0) {
        printf("L2MPSASYN: Failed to restore %s\n",thr_pv);
        return -1;
    }
    // H
    snprintf(thr_pv, sizeof(thr_pv),"%s_%s_H",fault_name,thr_name);
    snprintf(restore_pv,sizeof(restore_pv),"%s_%s_H_MGR",fault_name,thr_name);
    if (restoreThreshold(thr_pv,restore_pv) < 0) {
        printf("L2MPSASYN: Failed to restore %s\n",thr_pv);
        return -1;
    }
    // Reset Latched bit.
    return 0;
}

int restoreThresholds(const char *fault_name,const char* prefix,int numThr,int alt,int idl,int nc)
{
    char thr[256];
    int err = 0;
    printf("L2MPSASYN: Restoring thresholds for %s\n",fault_name);
    int i = 0;
    for(i = 0; i < numThr; i++) {
        snprintf(thr,sizeof(thr),"T%i",i);
        if (buildPvNames(fault_name,thr) < 0) {
            err++;
        }
        if (alt) {
            snprintf(thr,sizeof(thr),"T%i_ALT",i);
            if (buildPvNames(fault_name,thr) < 0) {
                err++;
            }
        }
    }
    if (idl) {
        snprintf(thr,sizeof(thr),"NO_BEAM");
        if (buildPvNames(fault_name,thr) < 0) {
            err++;
        }
    }
    if (nc) {
        snprintf(thr,sizeof(thr),"T0_NC");
        if (buildPvNames(fault_name,thr) < 0) {
            err++;
        }
    }
    return err;
}

