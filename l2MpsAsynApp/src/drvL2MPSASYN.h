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

#define DRIVER_NAME         "L2MPS"
#define MAX_SIGNALS         (3)     // Max number of parameter list (number of bays)
#define NUM_PARAMS          (1500)  // Max number of paramters

// Number of AMC bays on a carrier
const uint8_t numberOfBays = 2;

// Asyn parameter list numbers
const int paramListAppBay0 = 0; // Bay 0 application
const int paramListAppBay1 = 1; // Bay 1 application
const int paramListMpsBase = 2; // MPS base

// Number of RX Links
const std::size_t numberOfRxLinks = 14;

// Default MPS Root Path
const std::string defaultMpsRootPath("mmio/AmcCarrierCore/AppMps");

// BPM data types
typedef void (IMpsBpm::*BpmW32_t)(const bpmThr_channel_t&, const float) const;
typedef void (IMpsBpm::*BpmW1_t)(const bpmThr_channel_t&, const bool) const;
typedef void (IMpsBpm::*bpm_setScale_func_t)(const bpm_channel_t&, const float) const;
typedef void (IMpsBpm::*bpm_setIdleEn_funct_t)(const bpm_channel_t&, const bool) const;

typedef std::map<int, std::pair<BpmW32_t, bpmThr_channel_t>> bpm_fmap_w32_t;
typedef std::map<int, std::pair<BpmW1_t,  bpmThr_channel_t>> bpm_fmap_w1_t;
typedef std::map<int, std::pair<bpm_setScale_func_t, bpm_channel_t>> bpm_scaleFuncMap_t;
typedef std::map<int, std::pair<bpm_setIdleEn_funct_t, bpm_channel_t>> bpm_setIdleEnMap_t;

// BLEN data types
typedef void (IMpsBlen::*BlenW32_t)(const blenThr_channel_t&, const float) const;
typedef void (IMpsBlen::*BlenW1_t)(const blenThr_channel_t&, const bool) const;
typedef void (IMpsBlen::*blen_setScale_func_t)(const blen_channel_t&, const float) const;
typedef void (IMpsBlen::*blen_setIdleEn_funct_t)(const blen_channel_t&, const bool) const;

typedef std::map<int, std::pair<BlenW32_t, blenThr_channel_t>> blen_fmap_w32_t;
typedef std::map<int, std::pair<BlenW1_t,  blenThr_channel_t>> blen_fmap_w1_t;
typedef std::map<int, std::pair<blen_setScale_func_t, blen_channel_t>> blen_scaleFuncMap_t;
typedef std::map<int, std::pair<blen_setIdleEn_funct_t, blen_channel_t>> blen_setIdleEnMap_t;

// BCM data types
typedef void (IMpsBcm::*BcmW32_t)(const bcmThr_channel_t&, const float) const;
typedef void (IMpsBcm::*BcmW1_t)(const bcmThr_channel_t&, const bool) const;
typedef void (IMpsBcm::*bcm_setScale_func_t)(const bcm_channel_t&, const float) const;
typedef void (IMpsBcm::*bcm_setIdleEn_funct_t)(const bcm_channel_t&, const bool) const;

typedef std::map<int, std::pair<BcmW32_t, bcmThr_channel_t>> bcm_fmap_w32_t;
typedef std::map<int, std::pair<BcmW1_t,  bcmThr_channel_t>> bcm_fmap_w1_t;
typedef std::map<int, std::pair<bcm_setScale_func_t, bcm_channel_t>> bcm_scaleFuncMap_t;
typedef std::map<int, std::pair<bcm_setIdleEn_funct_t, bcm_channel_t>> bcm_setIdleEnMap_t;

// BLM data types
typedef void (IMpsBlm::*BlmW32_t)(const blmThr_channel_t&, const float) const;
typedef void (IMpsBlm::*BlmW1_t)(const blmThr_channel_t&, const bool) const;
typedef void (IMpsBlm::*blm_setScale_func_t)(const blm_channel_t&, const float) const;
typedef void (IMpsBlm::*blm_setIdleEn_funct_t)(const blm_channel_t&, const bool) const;

typedef std::map<int, std::pair<BlmW32_t, blmThr_channel_t>> blm_fmap_w32_t;
typedef std::map<int, std::pair<BlmW1_t,  blmThr_channel_t>> blm_fmap_w1_t;
typedef std::map<int, std::pair<blm_setScale_func_t, blm_channel_t>> blm_scaleFuncMap_t;
typedef std::map<int, std::pair<blm_setIdleEn_funct_t, blm_channel_t>> blm_setIdleEnMap_t;

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
    int                 rstCnt;
    int                 rstPll;
};

// Threshold table parameters data type
struct thr_tableParam_t
{
    int minEn;
    int maxEn;
    int min;
    int max;
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
    int  scaleFactor;
};

// Threhold parameter (information + table data) data type
struct  thr_param_t
{
    thr_chInfoParam_t info;
    thr_chParam_t     data;
};

// Application paramater map data type
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
        L2MPS(const char *portName, const uint16_t appId, const std::string recordPrefixMps, const std::array<std::string, numberOfBays> recordPrefixBay, std::string mpsRootPath);

        // Methods that we override from asynPortDriver
        virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
        virtual asynStatus writeUInt32Digital(asynUser *pasynUser, epicsUInt32 value, epicsUInt32 mask);
        virtual asynStatus writeFloat64 (asynUser *pasynUser, epicsFloat64 value);

        // MPS base info callback function
        static void setMpsCallback(mps_infoData_t info);
        void updateMpsParametrs(mps_infoData_t info);

        // App callback function
        template<typename T>
        static void setAppCallback(int bay, T data);
        template<typename T>
        void updateAppParameters(int bay, T data);

    private:
        const char *driverName_;               // Name of the driver (passed from st.cmd)
        const char *portName_;
        std::string recordPrefixMps_;
        std::array<std::string, numberOfBays> recordPrefixBay_;
        std::array<std::string, numberOfBays> amcType_;
        MpsNode node_;
        boost::any amc[numberOfBays];

        // BPM application fuction maps
        bpm_fmap_w32_t      fMapBpmW32;
        bpm_fmap_w1_t       fMapBpmW1;
        bpm_scaleFuncMap_t  fMapBpmWScale;
        bpm_setIdleEnMap_t  fMapBpmSetIdleEn;

        // BLEN application fuction maps
        blen_fmap_w32_t     fMapBlenW32;
        blen_fmap_w1_t      fMapBlenW1;
        blen_scaleFuncMap_t fMapBlenWScale;
        blen_setIdleEnMap_t fMapBlenSetIdleEn;

        // BCM application fuction maps
        bcm_fmap_w32_t      fMapBcmW32;
        bcm_fmap_w1_t       fMapBcmW1;
        bcm_scaleFuncMap_t  fMapBcmWScale;
        bcm_setIdleEnMap_t  fMapBcmSetIdleEn;

        // BLM application fuction maps
        blm_fmap_w32_t      fMapBlmW32;
        blm_fmap_w1_t       fMapBlmW1;
        blm_scaleFuncMap_t  fMapBlmWScale;
        blm_setIdleEnMap_t  fMapBlmSetIdleEn;

        // MPS base parameters
        mps_infoParam_t     mpsInfoParams;

        // Application parameters
        paramMap_t          paramMap;

        // BPM application init
        void InitBpmMaps(const int bay);
        void InitBlenMaps(const int bay);
        void InitBcmMaps(const int bay);
        void InitBlmMaps(const int bay);
};
