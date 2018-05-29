# List of PV generated by this module

This module, when loaded in your IOC, will automatically created all the MPS related PVs.

The module generates two groups of PVs:
* PVs common to all applications.
* PVs related to the application itself.

This document describe the PVs that are generated.

## List of common MPS PVs

These PVs are related to the Common Platform MPS modules and are present in all applications, therefore these PVs are common to all IOCs.

Their name is prefixed by the parameter *PREFIX_BASE*, used in the driver configuration as described in **README.configureDriver.md**.

| PV name                               | Description
|---------------------------------------|----------------------------------
| `$(PREFIX_BASE):APP_ID`               | Mps application ID.
| `$(PREFIX_BASE):MPS_EN`               | Mps application enable (setting).
| `$(PREFIX_BASE):MPS_EN_RBV`           | Mps application enable (readback).
| `$(PREFIX_BASE):MPS_VER`              | MPS version (setting).
| `$(PREFIX_BASE):MPS_VER_RBV`          | MPS version (readback).
| `$(PREFIX_BASE):APP_TYPE`             | Application type.
| `$(PREFIX_BASE):MPS_SLOT`             | Application is installed in slot2.
| `$(PREFIX_BASE):LC1_MODE`             | LCLS1 mode (setting).
| `$(PREFIX_BASE):LC1_MODE_RBV`         | LCLS1 mode (readback).
| `$(PREFIX_BASE):DIGITAL_EN`           | Application generates digital messages.
| `$(PREFIX_BASE):BYTE_COUNT`           | Number of bytes in the MPS message.
| `$(PREFIX_BASE):BEAM_DEST_MASK_RBV`   | Beam destination mask: One bit per destination for BPM or kicker.
| `$(PREFIX_BASE):ALT_DEST_MASK_RBV`    | Beam destination mask: One bit per destination for no-beam table.
| `$(PREFIX_BASE):MSG_CNT`              | Mps message counter.
| `$(PREFIX_BASE):ROLL_OVER_EN`         | Status Counter Roll Over Enable.
| `$(PREFIX_BASE):PLL_LOCKED`           | MPS PLL Lock Status.
| `$(PREFIX_BASE):TX_LINK_UP`           | Status of the MPS TX link.
| `$(PREFIX_BASE):TX_LINK_UP_CNT`       | MPS TX link up counter.
| `$(PREFIX_BASE):RX_LINK_UP_$(N)`      | Status of the MPS RX link N (N = 0..13).
| `$(PREFIX_BASE):RX_LINK_UP_CNT_$(N)`  | MPS RX link N up counter (N = 0..13).
| `$(PREFIX_BASE):TX_PKT_SENT_CNT`      | MPS TX Packet Sent Counter.
| `$(PREFIX_BASE):RX_PKT_RCV_CNT_$(N)`  | MPS RX N Packet Received Counter (N = 0..13).
| `$(PREFIX_BASE):LAST_MSG_APP_ID`      | MPS last message: application ID.
| `$(PREFIX_BASE):LAST_MSG_TMSTMP`      | MPS last message: time stamp.
| `$(PREFIX_BASE):LAST_MSG_BYTE_$(N)`   | MPS last message: byte N (N = 0..5).
| `$(PREFIX_BASE):LAST_MSG_LC1_EN`      | MPS last message: LCLS1 mode flag.

## List of application related PVs

These PVs are related to the application thresholds. There will be a group of these PVs for each Bay application.

Their name is prefixed by the parameters *PREFIX_BAY0* and *PREFIX_BAY1*, used in the driver configuration as described in **README.configureDriver.md**.

| PV name                                               | Description
|-------------------------------------------------------|----------------------------------
| `$(PREFIX_BAY):$(PROPERTY)_THR_CH`                    | Threshold channel.
| `$(PREFIX_BAY):$(PROPERTY)_THR_BYTEMAP`               | Byte position of this threshold in the MPS message.
| `$(PREFIX_BAY):$(PROPERTY)_THR_CNT`                   | Number of enabled threshold (for STD and ALT tables).
| `$(PREFIX_BAY):$(PROPERTY)_LC1_EN`                    | LCLS1 table enabled.
| `$(PREFIX_BAY):$(PROPERTY)_IDL_EN`                    | IDLE (no-beam) table enable (setting).
| `$(PREFIX_BAY):$(PROPERTY)_IDL_EN_RBV`                | IDLE (no-beam) table enable (readback).
| `$(PREFIX_BAY):$(PROPERTY)_ALT_EN`                    | Alternative table enabled.
| `$(PREFIX_BAY):$(PROPERTY)_SF`                        | PV used to read the application scale factor and send it to the driver (see **README.scaleFactor.md**).
| `$(PREFIX_BAY):$(PROPERTY)_SF_RBV`                    | Readback of the scale factor being used for this channel.
| `$(PREFIX_BAY):$(PROPERTY)_T$(N)_$(TABLE)_L_EN`       | Minimum threshold enable (setting).
| `$(PREFIX_BAY):$(PROPERTY)_T$(N)_$(TABLE)_L_EN_RBV`   | Minimum threshold enable (readback).
| `$(PREFIX_BAY):$(PROPERTY)_T$(N)_$(TABLE)_H_EN`       | Maximum threshold enable (setting).
| `$(PREFIX_BAY):$(PROPERTY)_T$(N)_$(TABLE)_H_EN_RBV`   | Maximum threshold enable (readback).
| `$(PREFIX_BAY):$(PROPERTY)_T$(N)_$(TABLE)_L`          | Minimum threshold (setting).
| `$(PREFIX_BAY):$(PROPERTY)_T$(N)_$(TABLE)_L_RBV`      | Minimum threshold (readback).
| `$(PREFIX_BAY):$(PROPERTY)_T$(N)_$(TABLE)_H`          | Maximum threshold (setting).
| `$(PREFIX_BAY):$(PROPERTY)_T$(N)_$(TABLE)_H_RBV`      | Maximum threshold (readback).

Where:
* __PROPERTY__: is the physical property the threshold is being applied to. For example:
** For BPMs they are _X_, _Y_, _TIMIT_
** For BCMs they are _CHARGE_, _DIFF_
** For BLMs they are the integration channels (_I0_ ... _I3_)
* __TABLE__: is the table type:
** empty for the standard table
** _ALT_ for the alternative table
** _IDL_ for the no-beam table
** _LC1_ for the LCLS1 table
* __N__: is threshold number (0..7 for STD and ALT, and 0 for IDL and LC1)