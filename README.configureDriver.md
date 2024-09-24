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
|----------------------------|-----------------------------
| APP_ID                     | Application ID. Unique for each carrier.  Assigned by MPS, loaded by user
|----------------------------|-----------------------------
| PREFIX_BASE                | Used for PVs that carry MPS information common to all applications.  Assigned by MPS, loaded by user

**Notes:**
- *APP_ID* is defined in the MPS Central Database.
- *PREFIX_BASE* is used for the MPS information common to all applications, so it belongs to the MPS system.

## Use of the yamlLoader Module

You must the **yamlLoader** module with this module. You need to call **cpswLoadYamlFile()** before **L2MPSASYNConfig()** in your **st.cmd**.
