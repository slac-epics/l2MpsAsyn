# Threshold scale factors

Threshold are applied in firmware in raw units (ADC counts for example). But the threshold PVs can represent engineering units is a scale factor is provided by the application.

The way to pass the scale factor is to include a PV in you application with the name: **`$(APP_PREFIX):$(PROPERTY)_FWSCL`** with the scale factor expressed in [EGU/raw] units.

The MPS application has a PV called **`$(APP_PREFIX):$(PROPERTY)_SF`** which reads this scale factor PV and send its value to the driver. A readback value is provided in the PV **`$(APP_PREFIX):$(PROPERTY)_SF_RBV`**.

## Examples

BPM applications can provide, for example:

**`$(APP_PREFIX):X_FWSCL`** [mm/raw]
**`$(APP_PREFIX):Y_FWSCL`** [mm/raw]
**`$(APP_PREFIX):TMIT_FWSCL`** [Nel/raw]

Then the threshold for **X** and **Y** will be expressed in **mm**, and the thresholds for **TMIT** will be expressed in **Nel**.

## Database example

If you do not have already a PV with the scale factor, you can use the database provided by this module called `mps_scale_factor.db`. You just need to do the following changes:

### xxxApp/Db/Makefile

```
DB_INSTALLS += $(L2MPSASYN)/db/mps_scale_factor.db
```

### st.cmd

```
dbLoadRecords("db/mps_scale_factor.db", "P=${APP_PREFIX}, PROPERTY=${PROPERTY}, EGU=${PROPERTY_EGU_UNITS}, PREC=${SCALE_FACTOR_PRECISION}, VAL=${SCALE_FACTOR}")
```

Note: A list of properties for each application is provided in **README.pvList.md**.
