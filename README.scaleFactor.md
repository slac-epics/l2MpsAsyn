# Threshold scale factors

Threshold are applied in firmware in raw units (ADC counts for example). But the threshold PVs can represent engineering units is a scale factor is provided by the application.

The way to pass the scale factor is to include a PV in you application with the name: **`$(PREFIX_BAY):$(PROPERTY)_FWSCL`** with the scale factor expressed in [EGU/raw] units.

The MPS application has a PV called **`$(PREFIX_BAY):$(PROPERTY)_SF`** which reads this scale factor PV and send its value to the driver. A readback value is provided in the PV **`$(PREFIX_BAY):$(PROPERTY)_SF_RBV`**.

## Examples

BPM applications can provide, for example:

**`$(PREFIX_BAY):X_FWSCL`** [mm/raw]
**`$(PREFIX_BAY):Y_FWSCL`** [mm/raw]
**`$(PREFIX_BAY):TMIT_FWSCL`** [Nel/raw]

Then the threshold for **X** and **Y** will be expressed in **mm**, and the thresholds for **TMIT** will be expressed in **Nel**.