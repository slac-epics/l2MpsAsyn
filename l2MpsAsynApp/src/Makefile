TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

USR_CXXFLAGS += -std=c++0x

# <name>.dbd will be created from <name>Include.dbd
DBD += l2MpsAsyn.dbd 

INC += drvL2MPSASYN.h

LIBRARY_IOC += l2MpsAsyn
LIB_SRCS += drvL2MPSASYN.cpp
LIB_LIBS += asyn 
LIB_LIBS += yamlLoader


#=====================================================
# Path to "NON EPICS" External PACKAGES: USER INCLUDES
#======================================================
USR_INCLUDES = -I$(BOOST_INCLUDE) -I$(CPSW_FRAMEWORK_INCLUDE) -I$(YAML_INCLUDE) -I$(L2MPS_INCLUDE)
cpsw_DIR = $(CPSW_FRAMEWORK_LIB)
boost_system_DIR = $(BOOST_LIB)
yaml-cpp_DIR = $(YAML_LIB)
l2Mps_DIR = $(L2MPS_LIB)
USR_LIBS_Linux += l2Mps cpsw boost_system yaml-cpp
#======================================================
 
#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
