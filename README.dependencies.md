# l2MpsAsyn dependencies

This module depends on the following external packages:
- l2Mps
- CPSW (framework)
- BOOST
- YAML-CPP

and on the following EPICS modules:
- Asyn
- yamlLoader
- l2MpsAsyn

In order to use this module you need to add the following definitions into your IOC application:

## configure/CONFIG_SITE

```
CPSW_FRAMEWORK_PACKAGE_NAME=cpsw/framework
CPSW_FRAMEWORK_VERSION=R3.5.4
CPSW_FRAMEWORK_TOP=$(PACKAGE_SITE_TOP)/$(CPSW_FRAMEWORK_PACKAGE_NAME)/$(CPSW_FRAMEWORK_VERSION)
CPSW_FRAMEWORK_LIB = $(CPSW_FRAMEWORK_TOP)/$(PKG_ARCH)/lib
CPSW_FRAMEWORK_INCLUDE = $(CPSW_FRAMEWORK_TOP)/$(PKG_ARCH)/include

YAML_PACKAGE_NAME=yaml-cpp
YAML_VERSION=yaml-cpp-0.5.3
YAML_TOP=$(PACKAGE_SITE_TOP)/$(YAML_PACKAGE_NAME)/$(YAML_VERSION)
YAML_LIB= $(YAML_TOP)/$(PKG_ARCH)/lib
YAML_INCLUDE=$(YAML_TOP)/$(PKG_ARCH)/include

BOOST_PACKAGE_NAME=boost
BOOST_VERSION=1.63.0
BOOST_TOP=$(PACKAGE_SITE_TOP)/$(BOOST_PACKAGE_NAME)/$(BOOST_VERSION)
BOOST_LIB = $(BOOST_TOP)/$(PKG_ARCH)/lib
BOOST_INCLUDE = $(BOOST_TOP)/$(PKG_ARCH)/include

L2MPS_PACKAGE_NAME=l2Mps
L2MPS_VERSION=R1.0.0
L2MPS_TOP=$(PACKAGE_SITE_TOP)/$(L2MPS_PACKAGE_NAME)/$(L2MPS_VERSION)
L2MPS_LIB=$(L2MPS_TOP)/$(PKG_ARCH)/lib
L2MPS_INCLUDE=$(L2MPS_TOP)/$(PKG_ARCH)/include
```

## configure/CONFIG_SITE.Common.linuxRT-x86_64

```
PKG_ARCH=$(LINUXRT_BUILDROOT_VERSION)-x86_64
```

## configure/CONFIG_SITE.Common.rhel6-x86_64

```
PKG_ARCH=$(LINUX_VERSION)-x86_64
```

## configure/RELEASE

```
ASYN_MODULE_VERSION=R4.30-0.3.0
YAMLLOADER_MODULE_VERSION=R1.0.3
L2MPSASYN_MODULE_VERSION=R1.0.0

ASYN=$(EPICS_MODULES)/asyn/$(ASYN_MODULE_VERSION)
YAMLLOADER=$(EPICS_MODULES)/yamlLoader/$(YAMLLOADER_MODULE_VERSION)
L2MPSASYN=$(EPICS_MODULES)/l2MpsAsyn/$(L2MPSASYN_MODULE_VERSION)
```

## xxxApp/src/Makefile

```
# =====================================================
# Path to "NON EPICS" External PACKAGES: USER INCLUDES
# =====================================================
USR_INCLUDES = $(addprefix -I,$(BOOST_INCLUDE) $(CPSW_FRAMEWORK_INCLUDE) $(YAML_INCLUDE) $(L2MPS_INCLUDE))
# =====================================================

# ======================================================
#PATH TO "NON EPICS" EXTERNAL PACKAGES: USER LIBRARIES
# ======================================================
cpsw_DIR = $(CPSW_FRAMEWORK_LIB)
yaml-cpp_DIR = $(YAML_LIB)
l2Mps_DIR = $(L2MPS_LIB)
# ======================================================

# ======================================================
# LINK "NON EPICS" EXTERNAL PACKAGE LIBRARIES STATICALLY
# ======================================================
USR_LIBS_Linux += l2Mps cpsw yaml-cpp
# ======================================================

# l2MpsAsyn and yamlLoader DBD
xxx_DBD += asyn.dbd
xxx_DBD += l2MpsAsyn.dbd
xxx_DBD += yamlLoader.dbd

# =====================================================
# Link in the libraries from other EPICS modules
# =====================================================
xxx_LIBS += yamlLoader
xxx_LIBS += l2MpsAsyn

xxx_LIBS += asyn
```

## xxxApp/Db/Makefile

```
# ==========================================
# LCLS2 MPS application specific databases
# ==========================================

# Common database to all applications
DB_INSTALLS += $(L2MPSASYN)/db/mps.db   # All application need this one.

# Application specific database
DB_INSTALLS += $(L2MPSASYN)/db/mps_bpm.db  # Each application only needs to
DB_INSTALLS += $(L2MPSASYN)/db/mps_blen.db # include its specific db file.
DB_INSTALLS += $(L2MPSASYN)/db/mps_blm.db  #
DB_INSTALLS += $(L2MPSASYN)/db/mps_bcm.db  #
```
