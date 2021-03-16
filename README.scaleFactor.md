# Threshold scale factors

Threshold are applied in firmware in raw units (ADC counts for example). But the threshold PVs can represent engineering units is a scale factor is provided by the application.

The MPS application has two PVs called **`$(APP_PREFIX):$(PROPERTY)_SS`**  and **`$(APP_PREFIX):$(PROPERTY)_SO`** which read the scale slope and offset value, respectively, from tow PVs provided by the application. The MPS PVs read the values from the application PVs and the values to the low level driver where the scaling is applied. Readback values are also provided in the PVs **`$(APP_PREFIX):$(PROPERTY)_SS_RBV`** and **`$(APP_PREFIX):$(PROPERTY)_SO_RBV`** respectively.

The name of application PVs are **`$(APP_PREFIX):$(PROPERTY)_FWSLO`**  for the slope value, and **`$(APP_PREFIX):$(PROPERTY)_FWOFF`** for the offset value. BPM applications is an exception to this rule; it only provides a slope value PV named **`$(APP_PREFIX):$(PROPERTY)_FWSCL`**.

The slope value is expressed in [EGU/raw] units, while the offset value is expressed in [raw].

## Equations

When a threshold PV (expressed in EGU) is set, the actual firmware register value is:

```
Register [raw] = threshold [EGU] / Slope [EGU/raw] + Offset [raw]
```

On the other hand, when the threshold value is read back from the firmware register (expressed in raw units), then the threshold PV value is:

```
Threshold [EGU] = ( Register [raw] - Offset [raw] ) * Slope [EGU/raw]
```

## Examples

BPM applications can provide, for example:

**`$(APP_PREFIX):X_FWSCL`** [mm/raw]
**`$(APP_PREFIX):Y_FWSCL`** [mm/raw]
**`$(APP_PREFIX):CHRG_FWSCL`** [pC/raw]

Then the threshold for **X** and **Y** will be expressed in **mm**, and the thresholds for **CHRG** will be expressed in **pC**.

A BLM application, on the other hand, used to read a solenoid device, can provide, for example:

**`$(APP_PREFIX):I0_FWSLO`** [A/raw]
**`$(APP_PREFIX):I0_FWOFF`** [raw]

then the threshold for **I0** will be expressed in **A**.

## Database example

If you do not have already a PV with the scale factor, you can use the database provided by this module called `mps_scale_factor.db`. It provides the two PVs for the slope and offset values.

To add it to your application you just need to do the following changes:

### xxxApp/Db/Makefile

```
DB_INSTALLS += $(L2MPSASYN)/db/mps_scale_factor.db
```

### st.cmd

```
dbLoadRecords("db/mps_scale_factor.db", "P=${APP_PREFIX}, PROPERTY=${PROPERTY}, EGU=${PROPERTY_EGU_UNITS}, PREC=${SCALE_SLOPE_PRECISION}, SLOPE=${SLOPE_VALUE}, OFFSET=${OFFSET_VALUE}")
```

Note: A list of properties for each application is provided in **README.pvList.md**.
