/**
 *-----------------------------------------------------------------------------
 * Title      : LCLS-II MPS EPICS Module
 * ----------------------------------------------------------------------------
 * File       : drvL2MPSASYN.h
 * Author     : Jesus Vasquez, jvasquez@slac.stanford.edu
 * Created    : 2017-10-20
 * ----------------------------------------------------------------------------
 * Description:
 * EPICS Module for interfacing the LCLS-II MPS.
 * ----------------------------------------------------------------------------
 * This file is part of l2MpsAsyn. It is subject to
 * the license terms in the LICENSE.txt file found in the top-level directory
 * of this distribution and at:
    * https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html.
 * No part of l2MpsAsyn, including this file, may be
 * copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE.txt file.
 * ----------------------------------------------------------------------------
**/

#include <stdio.h>
#include <string.h>
#include <fstream>
#include <boost/any.hpp>
#include <vector>
#include "asynPortDriver.h"

#include <cpsw_api_builder.h>
#include <cpsw_api_user.h>
#include <yaml-cpp/yaml.h>

#include "l2Mps_mps.h"
#include "l2Mps_thr.h"
#include "l2Mps_bpm.h"
#include "l2Mps_blen.h"
#include "l2Mps_bcm.h"
#include "l2Mps_blm.h"
#include "l2Mps_link_node.h"

extern "C" {
    #include "mpsManagerInfo.h"
}

#define DRIVER_NAME         "L2MPS"
#define MAX_SIGNALS         (4)     // Max number of parameter list (number of bays)

// Asyn parameter list numbers
const int paramListAppBay0  = 0; // Bay 0 application
const int paramListAppBay1  = 1; // Bay 1 application
const int paramListMpsBase  = 2; // MPS base
const int paramListLinkNode = 3; // MPS LN

// BPM data types
typedef bool (IMpsBpm::*BpmW32_t)(const bpmThr_channel_t&, const float) const;
typedef bool (IMpsBpm::*BpmW1_t)(const bpmThr_channel_t&, const bool) const;
typedef bool (IMpsBpm::*bpm_setScaleSlope_func_t)(const bpm_channel_t&, const float) const;
typedef bool (IMpsBpm::*bpm_setScaleOffset_func_t)(const bpm_channel_t&, const float) const;
typedef bool (IMpsBpm::*bpm_setIdleEn_funct_t)(const bpm_channel_t&, const bool) const;

typedef std::map<int, std::pair<BpmW32_t, bpmThr_channel_t>> bpm_fmap_w32_t;
typedef std::map<int, std::pair<BpmW1_t,  bpmThr_channel_t>> bpm_fmap_w1_t;
typedef std::map<int, std::pair<bpm_setScaleSlope_func_t, bpm_channel_t>> bpm_scaleSlopeFuncMap_t;
typedef std::map<int, std::pair<bpm_setScaleOffset_func_t, bpm_channel_t>> bpm_scaleOffsetFuncMap_t;
typedef std::map<int, std::pair<bpm_setIdleEn_funct_t, bpm_channel_t>> bpm_setIdleEnMap_t;

// BLEN data types
typedef bool (IMpsBlen::*BlenW32_t)(const blenThr_channel_t&, const float) const;
typedef bool (IMpsBlen::*BlenW1_t)(const blenThr_channel_t&, const bool) const;
typedef bool (IMpsBlen::*blen_setScaleSlope_func_t)(const blen_channel_t&, const float) const;
typedef bool (IMpsBlen::*blen_setScaleOffset_func_t)(const blen_channel_t&, const float) const;
typedef bool (IMpsBlen::*blen_setIdleEn_funct_t)(const blen_channel_t&, const bool) const;

typedef std::map<int, std::pair<BlenW32_t, blenThr_channel_t>> blen_fmap_w32_t;
typedef std::map<int, std::pair<BlenW1_t,  blenThr_channel_t>> blen_fmap_w1_t;
typedef std::map<int, std::pair<blen_setScaleSlope_func_t, blen_channel_t>> blen_scaleSlopeFuncMap_t;
typedef std::map<int, std::pair<blen_setScaleOffset_func_t, blen_channel_t>> blen_scaleOffsetFuncMap_t;
typedef std::map<int, std::pair<blen_setIdleEn_funct_t, blen_channel_t>> blen_setIdleEnMap_t;

// BCM data types
typedef bool (IMpsBcm::*BcmW32_t)(const bcmThr_channel_t&, const float) const;
typedef bool (IMpsBcm::*BcmW1_t)(const bcmThr_channel_t&, const bool) const;
typedef bool (IMpsBcm::*bcm_setScaleSlope_func_t)(const bcm_channel_t&, const float) const;
typedef bool (IMpsBcm::*bcm_setScaleOffset_func_t)(const bcm_channel_t&, const float) const;
typedef bool (IMpsBcm::*bcm_setIdleEn_funct_t)(const bcm_channel_t&, const bool) const;

typedef std::map<int, std::pair<BcmW32_t, bcmThr_channel_t>> bcm_fmap_w32_t;
typedef std::map<int, std::pair<BcmW1_t,  bcmThr_channel_t>> bcm_fmap_w1_t;
typedef std::map<int, std::pair<bcm_setScaleSlope_func_t, bcm_channel_t>> bcm_scaleSlopeFuncMap_t;
typedef std::map<int, std::pair<bcm_setScaleOffset_func_t, bcm_channel_t>> bcm_scaleOffsetFuncMap_t;
typedef std::map<int, std::pair<bcm_setIdleEn_funct_t, bcm_channel_t>> bcm_setIdleEnMap_t;

// BLM data types
typedef bool (IMpsBlm::*BlmW32_t)(const blmThr_channel_t&, const float) const;
typedef bool (IMpsBlm::*BlmW1_t)(const blmThr_channel_t&, const bool) const;
typedef bool (IMpsBlm::*blm_setScaleSlope_func_t)(const blm_channel_t&, const float) const;
typedef bool (IMpsBlm::*blm_setScaleOffset_func_t)(const blm_channel_t&, const float) const;
typedef bool (IMpsBlm::*blm_setIdleEn_funct_t)(const blm_channel_t&, const bool) const;

typedef std::map<int, std::pair<BlmW32_t, blmThr_channel_t>> blm_fmap_w32_t;
typedef std::map<int, std::pair<BlmW1_t,  blmThr_channel_t>> blm_fmap_w1_t;
typedef std::map<int, std::pair<blm_setScaleSlope_func_t, blm_channel_t>> blm_scaleSlopeFuncMap_t;
typedef std::map<int, std::pair<blm_setScaleOffset_func_t, blm_channel_t>> blm_scaleOffsetFuncMap_t;
typedef std::map<int, std::pair<blm_setIdleEn_funct_t, blm_channel_t>> blm_setIdleEnMap_t;

// LN soft inputs types
typedef bool (IMpsSoftInputs::*ln_softInput_t)(std::size_t index, bool val) const;
typedef std::map<int, std::pair<ln_softInput_t, std::size_t>> ln_softInputsFuncMap_t;

// Types of register interfaces
enum paramTypeList
{
    PARAM_TYPE_INT,
    PARAM_TYPE_DIG,
    PARAM_TYPE_STR,
    PARAM_TYPE_SIZE
};

// MPS base parameter data type
struct mps_infoParam_t
{
    int                 appId;
    int                 version;
    int                 enable;
    int                 lcls1Mode;
    int                 byteCount;
    int                 digitalEn;
    int                 beamDestMask;
    int                 altDestMask;
    int                 msgCnt;
    int                 lastMsgAppId;
    int                 lastMsgLcls;
    int                 lastMsgTimestamp;
    std::vector<int>    lastMsgByte;
    int                 txLinkUp;
    int                 txLinkUpCnt;
    std::vector<int>    rxLinkUp;
    std::vector<int>    rxLinkUpCnt;
    int                 mpsSlot;
    int                 appType;
    int                 pllLocked;
    int                 rollOverEn;
    int                 txPktSentCnt;
    std::vector<int>    rxPktRcvdCnt;
    int                 txPktPeriod;
    int                 txPktPeriodMin;
    int                 txPktPeriodMax;
    std::vector<int>    rxPktPeriod;
    std::vector<int>    rxPktPeriodMin;
    std::vector<int>    rxPktPeriodMax;
    int                 diagStrbCnt;
    int                 pllLockCnt;
    int                 txEofeSentCnt;
    std::vector<int>    rxErrDetCnt;
    int                 chEnable;
    int                 rstCnt;
    int                 rstPll;
};

// Link node parameter list
struct ln_param_t
{
    // Soft inputs
    struct
    {
        int                 inputWord;
        int                 errorWord;
        std::vector<int>    inputBit;
        std::vector<int>    errorBit;
    } softInputs;
};

// Threshold table parameters data type
struct thr_tableParam_t
{
    int minEn;
    int maxEn;
    int min;
    int max;
    int minRaw;
    int maxRaw;
};

// Threshold table parameter map data type
typedef std::map<thr_table_t, thr_tableParam_t> thr_chParam_t;

struct thr_chInfoParam_t
{
    int  ch;
    int  count;
    int  byteMap;
    int  idleEn;
    int  altEn;
    int  lcls1En;
    int  scaleSlope;
    int  scaleOffset;
};

// Threshold parameter (information + table data) data type
struct  thr_param_t
{
    thr_chInfoParam_t info;
    thr_chParam_t     data;
};

// Application parameter map data type
struct cmp {
    bool operator()(const boost::any& l, const boost::any& r)
    {
        try
        {
            blm_channel_t left = boost::any_cast<blm_channel_t>(l);
            blm_channel_t right = boost::any_cast<blm_channel_t>(r);
            return left < right;
        }
        catch(const boost::bad_any_cast &)
        {
        }

        try
        {
            bcm_channel_t left = boost::any_cast<bcm_channel_t>(l);
            bcm_channel_t right = boost::any_cast<bcm_channel_t>(r);
            return left < right;
        }
        catch(const boost::bad_any_cast &)
        {
        }

        try
        {
            bpm_channel_t left = boost::any_cast<bpm_channel_t>(l);
            bpm_channel_t right = boost::any_cast<bpm_channel_t>(r);
            return left < right;
        }
        catch(const boost::bad_any_cast &)
        {
        }

        try
        {
            blen_channel_t left = boost::any_cast<blen_channel_t>(l);
            blen_channel_t right = boost::any_cast<blen_channel_t>(r);
            return left < right;
        }
        catch(const boost::bad_any_cast &)
        {
        }

        std::cout << "paramMap_t error: not comparison found!" << std::endl;
        return false;
    }
};
typedef std::map<boost::any, thr_param_t, cmp> paramMap_t;


class L2MPS : public asynPortDriver {
    public:
        // Constructor
        L2MPS(const char *portName);

        // Methods that we override from asynPortDriver
        virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
        virtual asynStatus writeUInt32Digital(asynUser *pasynUser, epicsUInt32 value, epicsUInt32 mask);
        virtual asynStatus writeFloat64 (asynUser *pasynUser, epicsFloat64 value);

        // Update single parameter value, status and severity
        void updateAlarmParam(int list, int index, bool valid);
        template<typename T>
        void updateIntegerParam(int list, int index, std::pair<bool, T> p);
        template<typename T>
        void updateStringParam(int list, int index, std::pair<bool, T> p);
        template<typename T>
        void updateUIntDigitalParam(int list, int index, std::pair<bool, T> p);
        template<typename T>
        void updateParamArray(int type, int list, const std::vector<int>& index, const std::pair< bool, std::vector<T> > p);
        template<typename T>
        void updateDoubleParam(int list, int index, std::pair<bool, T> p);

        // MPS base info callback function
        void updateMpsParametrs(mps_infoData_t info);

        // App callback function
        template<typename T>
        void updateAppParameters(int bay, T data);

        // Default parameters, which can be changed from the IOC shell
        static std::string mpsConfigrationPath;     // Default location of the MPS configuration

    private:
        const char *driverName_;        // This driver name
        const char *portName_;          // Port name (passed from st.cmd)
        MpsNode     node_;              // MPS node
        boost::any  amc[numberOfBays];  // AMC application objects
        MpsLinkNode mpsLinkNode;        // Link node object (used only by LN applications)

        // BPM application function maps
        bpm_fmap_w32_t              fMapBpmW32;
        bpm_fmap_w1_t               fMapBpmW1;
        bpm_scaleSlopeFuncMap_t     fMapBpmWScaleSlope;
        bpm_scaleOffsetFuncMap_t    fMapBpmWScaleOffset;
        bpm_setIdleEnMap_t          fMapBpmSetIdleEn;

        // BLEN application function maps
        blen_fmap_w32_t             fMapBlenW32;
        blen_fmap_w1_t              fMapBlenW1;
        blen_scaleSlopeFuncMap_t    fMapBlenWScaleSlope;
        blen_scaleOffsetFuncMap_t   fMapBlenWScaleOffset;
        blen_setIdleEnMap_t         fMapBlenSetIdleEn;

        // BCM application function maps
        bcm_fmap_w32_t              fMapBcmW32;
        bcm_fmap_w1_t               fMapBcmW1;
        bcm_scaleSlopeFuncMap_t     fMapBcmWScaleSlope;
        bcm_scaleOffsetFuncMap_t    fMapBcmWScaleOffset;
        bcm_setIdleEnMap_t          fMapBcmSetIdleEn;

        // BLM application function maps
        blm_fmap_w32_t              fMapBlmW32;
        blm_fmap_w1_t               fMapBlmW1;
        blm_scaleSlopeFuncMap_t     fMapBlmWScaleSlope;
        blm_scaleOffsetFuncMap_t    fMapBlmWScaleOffset;
        blm_setIdleEnMap_t          fMapBlmSetIdleEn;

        // LN soft inputs function map
        ln_softInputsFuncMap_t      fMapSoftInputs;

        // MPS base parameters
        mps_infoParam_t             mpsInfoParams;

        // Link node parameters
        ln_param_t                  lnParams;

        // Application parameters
        paramMap_t                  paramMap;

        // Application initialization functions =
        void InitBpmMaps(const int bay);
        void InitBlenMaps(const int bay);
        void InitBcmMaps(const int bay);
        void InitBlmMaps(const int bay);
};
