# Release notes

Release notes for the SLAC's LCLS2 HPS MPS EPICS Module.

## Releases:
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
