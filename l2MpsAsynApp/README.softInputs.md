# Software inputs

## Description

Link Nodes besides regular digital and analog inputs, also have software inputs. This software inputs are digital inputs driven by external EPICS PVs.

The software inputs are defined in the MPS configuration database and exported as EPICS databases that should be loaded by Link Node IOCs.

For each software inputs, there are 2 parameters to set:
- The value: this is the actual value send to the Central Node/Link Processor. It has a timeout mechanism of around 1 second, so value has to be updated at around 1Hz.
- The error value: this is the value send to the Central Node/Link Processor in case of a timeout.

Currently there are 16 software input available in each Link Node.


## Asyn parameters

When the FW application type is a Link Node, the following asyn parameters are generated:

Parameter name      | Parameter type    | Description
--------------------|-------------------|-----------------
SOFT_CH_VALUE_WORD  | asynInt32         | 16-bit word with all 16 input values (read-only)
SOFT_CH_ERROR_WORD  | asynInt32         | 16-bit word with all 16 error values (read-only)
SOFT_CH_VALUE_XX    | asynUInt32Digital | The value for each input (XX = 00..15)
SOFT_CH_ERROR_XX    | asynUInt32Digital | The error value for each input (XX = 00..15)

All this parameters are defined in the asyn library, with address = 3.

## EPICS record example

In order to use the software inputs, the MPS database needs to generate the appropriate EPICS database.

For example, for each inputs the database will looks like this (CH = 00..15):
```
# Input value (set)
record(bo, "$(P):SOFT_CH_VALUE_$(CH)") {
    field(DESC, "Soft input value, channel $(CH)")
    field(SCAN, "Passive")
    field(PINI, "YES")
    field(DTYP, "asynUInt32Digital")
    field(OUT,  "@asynMask($(PORT),3,0x01)SOFT_CH_VALUE_$(CH)")
    field(ZNAM, "LOW")
    field(ONAM, "HIGH")
}

# Input value (read back)
record(bi, "$(P):SOFT_CH_VALUE_$(CH)_RBV") {
    field(DESC, "Soft input value, channel $(CH)")
    field(SCAN, "I/O Intr")
    field(PINI, "NO")
    field(DTYP, "asynUInt32Digital")
    field(INP,  "@asynMask($(PORT),3,0x01)SOFT_CH_VALUE_$(CH)")
    field(ZNAM, "LOW")
    field(ONAM, "HIGH")
}

# Input error value (set)
record(bo, "$(P):SOFT_CH_ERROR_$(CH)") {
    field(DESC, "Soft input error, channel $(CH)")
    field(SCAN, "Passive")
    field(PINI, "YES")
    field(DTYP, "asynUInt32Digital")
    field(OUT,  "@asynMask($(PORT),3,0x01)SOFT_CH_ERROR_$(CH)")
    field(ZNAM, "LOW")
    field(ONAM, "HIGH")
}

# Input error value (read back)
record(bi, "$(P):SOFT_CH_ERROR_$(CH)_RBV") {
    field(DESC, "Soft input error, channel $(CH)")
    field(SCAN, "I/O Intr")
    field(PINI, "NO")
    field(DTYP, "asynUInt32Digital")
    field(INP,  "@asynMask($(PORT),3,0x01)SOFT_CH_ERROR_$(CH)")
    field(ZNAM, "LOW")
    field(ONAM, "HIGH")
}
```

And to access the 16-word values:

```
record(longin, "$(P):SOFT_CH_VALUE_WORD") {
    field(DESC, "Soft input word")
    field(SCAN, "I/O Intr")
    field(PINI, "NO")
    field(DTYP, "asynInt32")
    field(INP,  "@asyn($(PORT),3)SOFT_CH_VALUE_WORD")
}

record(longin, "$(P):SOFT_CH_ERROR_WORD") {
    field(DESC, "Soft input word")
    field(SCAN, "I/O Intr")
    field(PINI, "NO")
    field(DTYP, "asynInt32")
    field(INP,  "@asyn($(PORT),3)SOFT_CH_ERROR_WORD")
}
```
