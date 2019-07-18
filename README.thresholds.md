# Thresholds

The values set for the threshold PVs (listed in [here](README.pvList.md)) are tracked by an MPS runtime database. Any changes to thresholds must go through the MpsManager server, which records the changes in the runtime database and then changes the PVs. Direct modification of MPS thresholds via `caput` is not allowed. Any changes must go through the MpsManager.

## Reboot

When an IOC reboots it automatically issues a request to the MpsManager to restore the threshold PVs. The request happens after `iocInit()`.

By default, the server name `lcls-daemon2` and port number `1975` are used to connect to the MpsManager. However, those values can be changed calling the function `L2MPSASYNSetManagerHost` in your `st.cmd` file. The usage of that function is

```
L2MPSASYNSetManagerHost(MpsManagerHostName, MpsManagerPortNumber)
```

where:

Parameter            | Description                            | Default value
---------------------|----------------------------------------|-------------------
MpsManagerHostName   | Server where the MpsManager is running | lcls-daemon2
MpsManagerPortNumber | MpsManager port number                 | 1975

If an empty string is passed to `MpsManagerHostName`, then it will be omitted and the default value will be used.

If zero is passed to `MpsManagerPortNumber`, then it will be ignored and the default value will be used.

The global MPS analog application ID is used when contacting the MpsManager to identify this system. This application ID is obtained directly from the L2MPSASYN driver. If the ID is invalid (0), then the thresholds will not be downloaded. Users will need to request threshold setting using proper scripts or user screens.

Once thresholds are set by the MpsManager the `$(PREFIX_BASE):THR_LOADED` is set, which in turn will enable the `$(PREFIX_BASE):MPS_EN` PV - this PV *must* be enabled in order to start MPS in the application IOC. It must be enabled only after the thresholds have been properly set by MpsManager.