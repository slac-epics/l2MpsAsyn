# How to configure the driver

## Overview

l2MpsAsyn uses CPSW for the communication with FPGA and AsynPortDriver for its integration into EPICS.

The modules must be loaded in your IOC, and it will automatically create all the MPS related PVs.

This document describes how to configure the driver and load it into your IOC.

## l2MpsAsyn Configuration

In order to use *l2MpsAsyn* in your application you must call **L2MPSASYNConfig** on your IOC's st.cmd script.

With the following parameters

L2MPSASYNConfig(PORT_NAME, APP_ID, PREFIX_BASE )

| Parameter                  | Description
|----------------------------|-----------------------------
| PORT_NAME                  | The name given to this port driver.
| APP_ID                     | Application ID. Unique for each carrier.  Assigned by MPS, loaded by user
| PREFIX_BASE                | Used for PVs that carry MPS information common to all applications.  Assigned by MPS, loaded by user

**Notes:**
- *APP_ID* is defined in the MPS Central Database.
- *PREFIX_BASE* is used for the MPS information common to all applications, so it belongs to the MPS system.

## Use of the yamlLoader Module

You must the **yamlLoader** module with this module. You need to call **cpswLoadYamlFile()** before **L2MPSASYNConfig()** in your **st.cmd**.


## iocBoot/ioc/st.cmd

  ```
# ==========================================
#     Load MPS Driver
# ==========================================
## Configure asyn port driver
# L2MPSASYNConfig(
#    Port Name,    # the name given to this port driver
#    APP ID,       # the unique application ID
#    MPS Prefix)   # the MPS PV prefix
  L2MPSASYNConfig("${L2MPSASYN_PORT}","${APPID}","${L2MPS_PREFIX}")

# ==========================================
#     Load MPS Databases
# ==========================================

  dbLoadRecords("db/mps.db","P=${L2MPS_PREFIX},PORT=${L2MPSASYN_PORT}")
  dbLoadRecords("db/mps_ai.db","P=CBLM:UNDH:1375,ATTR=LOSS,BAY=0,INP=0")  # Generic Analog Input (MPS LN/AN IOC Application, INP=0,1,2)
  dbLoadRecords("db/mps_bcm.db","P=TORO:GUNB:212,BAY=0")  # BCM Application (Bay 0=Right, 1=Left)
  dbLoadRecords("db/mps_blen.db","P=BLEN:GUNB:200,BAY=0")  # BLEN Application (Bay 0=Right, 1=Left)
  dbLoadRecords("db/mps_bpm.db","P=BPMS:GUNB:212,BAY=0")  # BPM Application (Bay 0=Right, 1=Left)
```
