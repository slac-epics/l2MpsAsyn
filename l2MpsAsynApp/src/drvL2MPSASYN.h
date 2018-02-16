#include <stdio.h>
#include <string.h>
#include <fstream>
#include <boost/any.hpp>
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

// MPS node parameters
#define appIdString             "APP_ID"
#define mpsEnString             "MPS_EN"
#define lcl1ModeString          "LCLS1_MODE"
#define byteCountString         "BYTE_COUNT"
#define digitalEnString         "DIGITAL_EN"
#define beamDestMaskString      "BEAM_DEST_MASK"
#define altDestMaskString       "ALT_DEST_MASK"
#define appTypeString           "APP_TYPE"
#define txLinkUpCntString       "TX_LINK_UP_CNT"
#define rxLinkUpCntString       "RX_LINK_UP_CNT"
#define txLinkUpString          "TX_LINK_UP"
#define rxLinkUpString          "RX_LINK_UP"
#define rollOverEnString        "ROLL_OVER_EN"
#define mpsSlotString           "MPS_SLOT"
#define pllLockedString         "PLL_LOCKED"
#define txPktSentCntString      "TX_PKT_SENT_CNT"
#define rxPktRcvdSentCntString  "RX_PKT_RCV_CNT"
#define mpsMsgCntString         "LAST_MSG_CNT"
#define mpsLastMsgAppIdString   "LAST_MSG_APPID"
#define mpsLastMsgTmstmpString  "LAST_MSG_TMSTMP"
#define mpsLastMsgLclsString    "LAST_MSG_LCLS"
#define mpsLastMsgByteString    "LAST_MSG_BYTE"
#define mpsSlatRstCntString     "SALT_RST_CNT"
#define mpsSlatRstPllString     "SALT_RST_PLL"

#define MAX_SIGNALS         (2)     // Max number of parameter list (number of bays)
#define NUM_PARAMS          (1500)  // Max number of paramters

// Number of AMC bays on a carrier
const uint8_t numberOfBays = 2;

// Number of RX Links
const std::size_t numberOfRxLinks = 14;

// Default MPS Root Path
const std::string defaultMpsRootPath("mmio/AmcCarrierCore/AppMps");

// BPM data types
typedef void (IMpsBpm::*BpmW32_t)(const bpmThr_channel_t&, const float) const;
typedef void (IMpsBpm::*BpmW1_t)(const bpmThr_channel_t&, const bool) const;
typedef void (IMpsBpm::*bpm_setScale_func_t)(const bpm_channel_t&, const float) const;

typedef std::map<int, std::pair<BpmW32_t, bpmThr_channel_t>> bpm_fmap_w32_t;
typedef std::map<int, std::pair<BpmW1_t,  bpmThr_channel_t>> bpm_fmap_w1_t;
typedef std::map<int, std::pair<bpm_setScale_func_t, bpm_channel_t>> bpm_scaleFuncMap_t;

// BLEN data types
typedef void (IMpsBlen::*BlenW32_t)(const blenThr_channel_t&, const float) const;
typedef void (IMpsBlen::*BlenW1_t)(const blenThr_channel_t&, const bool) const;
typedef void (IMpsBlen::*blen_setScale_func_t)(const blen_channel_t&, const float) const;

typedef std::map<int, std::pair<BlenW32_t, blenThr_channel_t>> blen_fmap_w32_t;
typedef std::map<int, std::pair<BlenW1_t,  blenThr_channel_t>> blen_fmap_w1_t;
typedef std::map<int, std::pair<blen_setScale_func_t, blen_channel_t>> blen_scaleFuncMap_t;

// BCM data types
typedef void (IMpsBcm::*BcmW32_t)(const bcmThr_channel_t&, const float) const;
typedef void (IMpsBcm::*BcmW1_t)(const bcmThr_channel_t&, const bool) const;
typedef void (IMpsBcm::*bcm_setScale_func_t)(const bcm_channel_t&, const float) const;

typedef std::map<int, std::pair<BcmW32_t, bcmThr_channel_t>> bcm_fmap_w32_t;
typedef std::map<int, std::pair<BcmW1_t,  bcmThr_channel_t>> bcm_fmap_w1_t;
typedef std::map<int, std::pair<bcm_setScale_func_t, bcm_channel_t>> bcm_scaleFuncMap_t;

// BLM data types
typedef void (IMpsBlm::*BlmW32_t)(const blmThr_channel_t&, const float) const;
typedef void (IMpsBlm::*BlmW1_t)(const blmThr_channel_t&, const bool) const;
typedef void (IMpsBlm::*blm_setScale_func_t)(const blm_channel_t&, const float) const;

typedef std::map<int, std::pair<BlmW32_t, blmThr_channel_t>> blm_fmap_w32_t;
typedef std::map<int, std::pair<BlmW1_t,  blmThr_channel_t>> blm_fmap_w1_t;
typedef std::map<int, std::pair<blm_setScale_func_t, blm_channel_t>> blm_scaleFuncMap_t;

struct thr_tableParam_t
{
    int minEn;
    int maxEn;
    int min;
    int max;
};

typedef std::map<thr_table_t, thr_tableParam_t> thr_chParam_t;

struct thr_chInfoParam_t
{
    int  ch;
    int  count;
    int  byteMap;
    int  idleEn;
    int  altEn;
    int  lcls1En;
};

struct  thr_paramMap_t
{
    thr_chInfoParam_t info;
    thr_chParam_t     data;  
};

// Paramater map data type
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
typedef std::map<boost::any, thr_paramMap_t, cmp> paramMap_t;


class L2MPS : public asynPortDriver {
    public:
        // Constructor
        L2MPS(const char *portName, const uint16_t appId, const std::string recordPrefixMps, const std::array<std::string, numberOfBays> recordPrefixBay, std::string mpsRootPath);

        // Methods that we override from asynPortDriver
        virtual asynStatus readInt32(asynUser *pasynUser, epicsInt32 *value);
        virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
        virtual asynStatus readUInt32Digital(asynUser *pasynUser, epicsUInt32 *value, epicsUInt32 mask);
        virtual asynStatus writeUInt32Digital(asynUser *pasynUser, epicsUInt32 value, epicsUInt32 mask);
        virtual asynStatus writeFloat64 (asynUser *pasynUser, epicsFloat64 value);
        virtual asynStatus readOctet(asynUser *pasynUser, char *value, size_t maxChars, size_t *nActual, int *eomReason);

        template<typename T>
        static void setCallback(int bay, T data);

        template<typename T>
        void updateParameters(int bay, T data);


    private:
        const char *driverName_;               // Name of the driver (passed from st.cmd)
        const char *portName_;
        std::string recordPrefixMps_;
        std::array<std::string, numberOfBays> recordPrefixBay_;
        std::array <std::string, numberOfBays> amcType_;
        MpsNode node_;
        boost::any amc[numberOfBays];

        // MPS node parameters
        int appIdValue_;
        int mpsEnValue_;
        int lcl1ModeValue_;
        int byteCountValue_;
        int digitalEnValue_;
        int beamDestMaskValue_;
        int altDestMaskValue_;
        int appTypeValue_;
        int rollOverEnValue_;
        int mpsSlotValue_;
        int pllLockedValue_;
        int txLinkUpValue_;
        int txLinkUpCntValue_;
        std::array<int, numberOfRxLinks> rxLinkUpValue_;
        std::array<int, numberOfRxLinks> rxLinkUpCntValue_;

        int txPktSentCntValue_;
        std::array<int, numberOfRxLinks> rxPktRcvdSentCntValue_;
        
        int mpsMsgCntValue_;
        int mpsLastMsgAppIdValue_;
        int mpsLastMsgTmstmpValue_;
        int mpsLastMsgLclsValue_;
        std::vector<int> mpsLastMsgByteValue_;
        int mpsSlatRstCntValue_;
        int mpsSlatRstPlltValue_;


        // BPM application fuction maps
        bpm_fmap_w32_t      fMapBpmW32;
        bpm_fmap_w1_t       fMapBpmW1;
        bpm_scaleFuncMap_t  fMapBpmWScale;

        // BLEN application fuction maps
        blen_fmap_w32_t     fMapBlenW32;
        blen_fmap_w1_t      fMapBlenW1;
        blen_scaleFuncMap_t fMapBlenWScale;

        // BCM application fuction maps
        bcm_fmap_w32_t      fMapBcmW32;
        bcm_fmap_w1_t       fMapBcmW1;
        bcm_scaleFuncMap_t  fMapBcmWScale;

        // BLM application fuction maps
        blm_fmap_w32_t      fMapBlmW32;
        blm_fmap_w1_t       fMapBlmW1;
        blm_scaleFuncMap_t  fMapBlmWScale;

        paramMap_t _paramMap;

        // BPM application init 
        void InitBpmMaps(const int bay);
        void InitBlenMaps(const int bay);
        void InitBcmMaps(const int bay);
        void InitBlmMaps(const int bay);
};
