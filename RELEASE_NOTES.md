# Release notes

Release notes for the SLAC's LCLS2 HPS MPS EPICS Module.

## Releases:
* __R2.13.1__: 2022-09-21 J. Mock
  * Build against l2Mps 2.6.1 to fix a bug in register address names for SALT diagnostics
  * Remove TMIT ALT threshold PVs - TMIT is not building ALT tables anymore
  * Remove X / Y Idle threshold PVs - X/Y no longer building IDL table

* __R2.13.0__: 2022-09-21 J. Mock
  * Add new SALT diagnostic registers: `MpsTxPktPeriod`, `MpsRxPktPeriod`, `MpsTxPktPeriodMax`,
    `MpsRxPktPeriodMax`, `MpsTxPktPeriodMin`, `MpsRxPktPeriodMin`, `DiagnosticStrbCnt`,
    `MpsPllLockCnt`, `MpsTxEofeSentCnt`, and `MpsRxErrDetCnt`.

* __R2.12.0__: 2022-06-15 J. Mock
  * Make MPS_EN PV autosaved

* __R2.11.0__: 2022-05-03 J. Mock
  * Build against l2Mps R2.5.0 to change how idle table is initialized

* __R2.10.2__: 2022-04-14 J. Mock
  * Remove from mps_bpm.substitutions all thresholds that are not loaded in FW:
      Any X > T1
      Any Y > T1

* __R2.10.1__: 2022-04-14 J. Mock
  * Fix IDLE table Enable type to be UInt32Digital everywhere - functionality was 
    originally with float64 type in l2MpsAsynApp/src/drvL2MPSASYN.cpp

* __R2.10.0__: 2022-04-14 J. Mock
  * Revert CHRGDIFF in BPM back to TMIT after discussions of requirements with users

* __R2.9.1__: 2022-3-15 J. Mock
  * Change CHRG fault name to CHRGDIFF

* __R2.9.0__: 2022-1-26 J. Mock
  * Build against asyn R4.39-1.0.1

* __R2.8.0__: 2021-11-15 J. Mock
  * Build against YAMLLOADER R2.2.0

* __R2.7.0__: 2021-03-16 J. Vasquez
  * Update BPM TMIT threshold. BPMs now use TMIT difference, expressed in `pC`. So, for the TMIT
    threshold related PVs rename the PVs, replacing `TMIT` with `CHRG`, change units from `Nel`
    to `pC`, and change the scale factor PV from `$(P):TMIT_FWSCL` to `$(P):CHRG_FWSCL`.

* __R2.6.0__: 2020-02-05 J. Vasquez
  * Update buildroot to version 2019.08
  * Update CPSW to version R4.4.1
  * Update l2Mps to version R2.4.0
  * Update yamlLoader to version R2.1.0

* __R2.5.1__: 2019-11.18 K. Kim
  * build with asyn R4.32-1.0.0

* __R2.5.0__: 2019-10-29 J. Vasquez
  * Update versions: CPSW framework to R4.3.1, l2Mps to R2.3.0 and
    yamlLoader to R1.1.5.

* __R2.4.0__: 2019-09-23 J. Vasquez
  * Update versions: CPSW framework to R4.2.0, l2Mps to R2.2.0 and
    yamlLoader to R1.1.4.

* __R2.3.2__: 2019-08-16 J. Vasquez
  * Bug fix: Fix scale PV name for BCM and BLEN applications.
  * Bug fix: add a missing colon in the mps_blm.substitution file.

* __R2.3.1__: 2019-08-13 J. Vasquez
  * Update l2Mps to version R2.1.2. That version includes a bug fix
    where an argument name was shadowing the protected member name.
    This was causing the polling thread to consume a lot of CPU.

* __R2.3.0__: 2019-07-18 J. Vasquez
  * Use initHooks to request a threshold restore to the MpsManager
    when the IOC boots.

* __R2.2.1__: 2019-07-12 J. Vasquez
  * Update CPSW framework to version R4.1.2, and l2Mps to version
    R2.1.1.

* __R2.2.0__: 2019-06-27 J. Vasquez
  * Update CPSW framework to version R4.1.0, l2Mps to version
    R2.1.0, and yamlLoader to version R1.1.3.

* __R2.1.0__: 2019-06-12 J. Vasquez
  * Addition of info(autosaveFields) to thr.template, this will
    enable autosave for thresholds and threshold enable PVs.
  * Disable the MPS application when the EPICS module is loaded.
    So, the MPS application will start disabled by default every
    time the IOC boots.

* __R2.0.0__: 2019-02-27 J. Vasquez
  * Update l2Mps to version R2.0.0. Now the threshold scale factor
    is divided into a slope and a offset value.
  * Update CPSW framework to version R3.6.6, boost to 1.64.0 and
    yaml-cpp to yaml-cpp-0.5.3_boost-1.64.0.
  * Update yamlLoader to version R1.1.2.
  * Set PINI=YES for passive records, so that restored values during
    IOC init get written to FW.

* __R1.2.0__: 2018-10-26 J.Vasquez
  * Update l2Mps to R1.2.0 and CPSW framework to R3.6.4

* __R1.1.0__: 2018-06-28 J. Vasquez
  * Update l2Mps driver to version R1.1.0.
  * The new l2Mps API send pairs of data and valid flags; use
    the status flag to set the record status and severity
    accordingly.

* __R1.0.2__: 2018-06-18 J. Vasquez
  * Update l2Mps driver to version R1.0.2.
  * se std::bind to set the callback fucntion, which are now of
    type std::function.
  * Remove the static methods and the global object pointer.
  * Remove boost_system library from makefile.
  * Update dependecies instruction in README file.

* __R1.0.1__: 2018-04-09 J. Vasquez
  * Update l2Mps driver to version R1.0.1.
  * Update yamlLoader version to R1.1.0.
  * Fix compiler warnings.

* __R1.0.0__: 2018-02-27 J. Vasquez
  * First stable release.
