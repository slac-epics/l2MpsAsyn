# MPS database

The MPS driver uses information obtained from the MPS database configuration. This information consist on EPICS database definitions (`mps.db` and `mps.env`) and firmware configuration files (`config.yaml`). Those files are automatically generated from the MPS database.

The MPS configuration files are generated and placed in directory following this structure

```
<MPS_CONFIGURATION_TOP>/app_db/<CPU_NAME>/<CRATE_ID>/<SLOT_NUMBER>/
```

Where:
- *CPU_NAME*    : is the name of the CPU where the IOC is currently running,
- *CRATE_ID*    : is the crate ID number of the ATCA crate where the application carrier is installed,
- *SLOT_NUMBER* : is the slot number of the ATCA crate where the application carrier is installed.

With this information is is possible to identify all IOC installed in the system.

By default the *MPS_CONFIGURATION_TOP* points to `/afs/slac/g/lcls/physics/mps_configuration/current/`, but this path can be changed by calling this function in you `st.cmd` file, before calling **L2MPSASYNConfig()**:

```
setMpsConfigurationPath(const char* path)
```

The values of *CPU_NAME*, *CRATE_ID* and *SLOT_NUMBER* can be overriden by using the function `L2MPSASYNSetApplicationInfo`. The usage of that function is

```
L2MPSASYNSetApplicationInfo(CpuName, CrateId, SlotNumber)
```

where:

Parameter  | Description                 | Default parameter  | Value used if default param specified
-----------|-----------------------------|--------------------|--------------------------------------
CpuName    | Alternative cpu host name   | "default"          | from `gethostname()` function
CrateId    | Alternative crate id        | -1                 | from config.yaml
SlotNumber | Alternative slot number     | -1                 | from config.yaml
