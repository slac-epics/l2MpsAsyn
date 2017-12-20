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
typedef std::map<std::string, std::pair<BpmR32_t, bpm_channel_t>> bpm_fmap_r32_t;
typedef std::map<std::string, std::pair<BpmW32_t, bpm_channel_t>> bpm_fmap_w32_t;
typedef std::map<std::string, std::pair<BpmR1_t,  bpm_channel_t>> bpm_fmap_r1_t;
typedef std::map<std::string, std::pair<BpmW1_t,  bpm_channel_t>> bpm_fmap_w1_t;

// BLEN data types
typedef std::map<std::string, std::pair<BlenR32_t, blen_channel_t>> blen_fmap_r32_t;
typedef std::map<std::string, std::pair<BlenW32_t, blen_channel_t>> blen_fmap_w32_t;
typedef std::map<std::string, std::pair<BlenR1_t,  blen_channel_t>> blen_fmap_r1_t;
typedef std::map<std::string, std::pair<BlenW1_t,  blen_channel_t>> blen_fmap_w1_t;

// BCM data types
typedef std::map<std::string, std::pair<BcmR32_t, bcm_channel_t>> bcm_fmap_r32_t;
typedef std::map<std::string, std::pair<BcmW32_t, bcm_channel_t>> bcm_fmap_w32_t;
typedef std::map<std::string, std::pair<BcmR1_t,  bcm_channel_t>> bcm_fmap_r1_t;
typedef std::map<std::string, std::pair<BcmW1_t,  bcm_channel_t>> bcm_fmap_w1_t;

// BLM data types
typedef std::map<std::string, std::pair<BlmR32_t, blm_channel_t>> blm_fmap_r32_t;
typedef std::map<std::string, std::pair<BlmW32_t, blm_channel_t>> blm_fmap_w32_t;
typedef std::map<std::string, std::pair<BlmR1_t,  blm_channel_t>> blm_fmap_r1_t;
typedef std::map<std::string, std::pair<BlmW1_t,  blm_channel_t>> blm_fmap_w1_t;

class L2MPS : public asynPortDriver {
    public:
        // Constructor
        L2MPS(const char *portName, const uint16_t appId, const std::string recordPrefixMps, const std::array<std::string, numberOfBays> recordPrefixBay, std::string mpsRootPath);

        // Methods that we override from asynPortDriver
        virtual asynStatus readInt32(asynUser *pasynUser, epicsInt32 *value);
        virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
        virtual asynStatus readUInt32Digital(asynUser *pasynUser, epicsUInt32 *value, epicsUInt32 mask);
        virtual asynStatus writeUInt32Digital(asynUser *pasynUser, epicsUInt32 value, epicsUInt32 mask);
        virtual asynStatus readOctet(asynUser *pasynUser, char *value, size_t maxChars, size_t *nActual, int *eomReason);

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
        bpm_fmap_r32_t  fMapBpmR32;
        bpm_fmap_w32_t  fMapBpmW32;
        bpm_fmap_r1_t   fMapBpmR1;
        bpm_fmap_w1_t   fMapBpmW1;

        // BLEN application fuction maps
        blen_fmap_r32_t  fMapBlenR32;
        blen_fmap_w32_t  fMapBlenW32;
        blen_fmap_r1_t   fMapBlenR1;
        blen_fmap_w1_t   fMapBlenW1;

        // BCM application fuction maps
        bcm_fmap_r32_t  fMapBcmR32;
        bcm_fmap_w32_t  fMapBcmW32;
        bcm_fmap_r1_t   fMapBcmR1;
        bcm_fmap_w1_t   fMapBcmW1;

        // BLM application fuction maps
        blm_fmap_r32_t  fMapBlmR32;
        blm_fmap_w32_t  fMapBlmW32;
        blm_fmap_r1_t   fMapBlmR1;
        blm_fmap_w1_t   fMapBlmW1;

        // BPM application init 
        void InitBpmMaps(const int bay);
        void InitBlenMaps(const int bay);
        void InitBcmMaps(const int bay);
        void InitBlmMaps(const int bay);

        // BPM parameter creators
        template <typename T>
        void createBpmParam(const std::string param, const int bay, const bpm_channel_t ch, T pFuncR);
        template <typename T, typename U>
        void createBpmParam(const std::string param, const int bay, const bpm_channel_t ch, T pFuncR, U pFuncW);        

        // BLEN parameter creators
        template <typename T>
        void createBlenParam(const std::string param, const int bay, const blen_channel_t ch, T pFuncR);
        template <typename T, typename U>
        void createBlenParam(const std::string param, const int bay, const blen_channel_t ch, T pFuncR, U pFuncW);

        // BCM parameter creators
        template <typename T>
        void createBcmParam(const std::string param, const int bay, const bcm_channel_t ch, T pFuncR);
        template <typename T, typename U>
        void createBcmParam(const std::string param, const int bay, const bcm_channel_t ch, T pFuncR, U pFuncW);

        // BLM parameter creators
        template <typename T>
        void createBlmParam(const std::string param, const int bay, const blm_channel_t ch, T pFuncR);
        template <typename T, typename U>
        void createBlmParam(const std::string param, const int bay, const blm_channel_t ch, T pFuncR, U pFuncW);
};

