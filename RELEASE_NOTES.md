# Release notes

Release notes for the SLAC's LCLS2 HPS MPS EPICS Module.

## Releases:
* __R1.0.3__: 2018-06-28 J. Vasquez
  * Update l2Mps driver to version R1.0.3.
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
