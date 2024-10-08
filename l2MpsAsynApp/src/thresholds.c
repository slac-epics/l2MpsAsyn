#include <thresholds.h>

int pvPut(char* pv_name, dbr_double_t value) {
    chid cid;
    if(ca_create_channel(pv_name, NULL,NULL,10,&cid) != ECA_NORMAL) {
        printf("ERROR: pvPut: Channel not created\n");
        return -1;
    }
    else{
        if(ca_pend_io(1.0) != ECA_NORMAL) {
            printf("ERROR: pvPut: Channel %s not found\n",pv_name);
            return -1; 
        }
        else {
            if(ca_put(DBR_DOUBLE, cid, &value) != ECA_NORMAL) {
                printf("ERROR: pvPut: Channel not put\n");
                return -1; 
            }
            else{
                if(ca_pend_io(1.0) != ECA_NORMAL) {
                    printf("ERROR: pvPut: Channel not flushed\n");
                    return -1; 
                }
                else {
                    if (ca_clear_channel(cid) != ECA_NORMAL) {
                        printf("ERROR: pvPut: Channel not cleared\n");
                        return -1; 
                    }
                    else {
                        return 0;
                    }
                }
            }
        }
    }
    printf("ERROR: pvPut fail!\n");
    return -1;
}

int pvGet(char* pv_name, double *value) {
    chid cid;
    dbr_double_t data;
    if(ca_create_channel(pv_name, NULL,NULL,10,&cid) != ECA_NORMAL) {
        printf("ERROR: pvGet: Channel not created\n");
        return -1;
    }
    else{
        if(ca_pend_io(1.0) != ECA_NORMAL) {
            printf("ERROR: pvGet: Channel not found\n");
            return -1; 
        }
        else {
            if(ca_get(DBR_DOUBLE, cid, &data) != ECA_NORMAL) {
                printf("ERROR: pvGet: Channel not put\n");
                return -1; 
            }
            else{
                if(ca_pend_io(1.0) != ECA_NORMAL) {
                    printf("ERROR: pvGet: Channel not flushed\n");
                    return -1; 
                }
                else {
                    if (ca_clear_channel(cid) != ECA_NORMAL) {
                        printf("ERROR: pvGet: Channel not cleared\n");
                        return -1; 
                    }
                    else {
                        *value = data;
                        return 0;
                    }
                }
            }
        }
    }
    printf("ERROR: pvGet fail!\n");
    return -1;
}

int restoreThreshold(char* thr_pv,char* restore_pv) {
    double restore_val;
    if (pvGet(restore_pv, &restore_val) <0) {
        printf("ERROR: Cannot get %s from manager\n",restore_pv);
        return -1;
    }
    else{
        if (pvPut(thr_pv, restore_val) < 0) {
            printf("L2MPSAsyn: Failed to put restore val for %s\n",thr_pv);
            return -1;
        }
        else {
            return 0;
        }
    }
    printf("ERROR: restoreThreshold failed at %s\n",thr_pv);
    return -1;
}

int buildPvNames(char* fault_name, char* thr_name) {
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
    return 0;
}

int restoreThresholds(char *fault_name,char* prefix,int numThr,int alt,int idl,int nc)
{
    char thr[256];
    char name_pv[256];
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
    if (err == 0) {
        snprintf(name_pv, sizeof(name_pv),"%s_NAME.DISA",fault_name);
        if (pvPut(name_pv, 1) < 0) {
            printf("L2MPSAsyn: Failed to put restore val for %s\n",name_pv);
        }
    }
    return err;
}

