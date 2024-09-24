# Thresholds

The values set for the threshold PVs (listed in [here](README.pvList.md)) are tracked by an MPS runtime database. Any changes to thresholds must go through the MpsManager server, which records the changes in the runtime database and then changes the PVs.

## Reboot

When an IOC reboots it automatically issues a request to the MpsManager to restore the threshold PVs. The request happens after `iocInit()`.

Once thresholds are set by the MpsManager the `$(PREFIX_BASE):MPS_EN` is set to enable - this PV *must* be enabled in order to start MPS in the application IOC. It must be enabled only after the thresholds have been properly set by MpsManager.