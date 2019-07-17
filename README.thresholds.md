# Thresholds

The values set for the threshold PVs (listed in [README.pvList.md]) are tracked by an MPS runtime database. Any changes to thresholds must go through the MpsManager server, which records the changes in the runtime database and then changes the PVs. Direct modification of MPS thresholds via `caput` is not allowed. Any changes must go through the MpsManager.

## Reboot

When an IOC reboots it automatically issues a request to the MpsManager to restore the threshold PVs. The request happens after `iocInit()` and it relies on some MPS environment variables loaded by the IOC during startup. The environment variables used to connect to MpsManager are:

Variable         | Description
-----------------|-----------------
MPS_MANAGER_HOST | Server where the MpsManager is running (lcls-daemon2 in production)
MPS_MANAGER_PORT | MpsManager port (default=1975)
MPS_ANA_APP_ID   | Global ID for the MPS analog application (defined by the MPS configuration)

If `MPS_MANAGER_HOST` or `MPS_MANAGER_PORT` are not defined the default values will be used to connect to MpsManager.

If `MPS_ANA_APP_ID` is not defined the thresholds will not be downloaded. Users will need to request threshold setting using proper scripts or user screens.

Once thresholds are set MpsManager the `$(PREFIX_BASE):THR_LOADED` is set, which in turn will enable the `$(PREFIX_BASE):MPS_EN` PV - this PV *must* be enabled in order to start MPS in the application IOC. It must be enabled only after the thresholds have been properly set by MpsManager.