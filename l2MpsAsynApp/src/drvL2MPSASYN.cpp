#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <getopt.h>
#include <sstream>
#include <boost/array.hpp>
#include <iostream>
#include <fstream>
#include <arpa/inet.h>
#include <math.h>
#include <epicsTypes.h>
#include <epicsTime.h>
#include <epicsThread.h>
#include <epicsString.h>
#include <epicsTimer.h>
#include <epicsMutex.h>
#include <epicsEvent.h>
#include <iocsh.h>

#include <dbAccess.h>
#include <dbStaticLib.h>

#include "drvL2MPSASYN.h"
#include "asynPortDriver.h"
#include <epicsExport.h>

#include <cpsw_api_builder.h>
#include <cpsw_api_user.h>
#include <yaml-cpp/yaml.h>

#include "yamlLoader.h"

L2MPS *pL2MPS;

// BPM parameter creators
template <>
void L2MPS::createBpmParam(const std::string param, const int bay, const bpm_channel_t ch, BpmR32_t pFuncR)
{
    int index;
    std::stringstream pName;
    pName.str("");
    pName << param << "_" << bay << ch[0] << ch[1]  << ch[2]  << ch[3];
    
    createParam(bay, pName.str().c_str(), asynParamInt32, &index);

    fMapBpmR32.insert( std::make_pair( pName.str(), std::make_pair( pFuncR, ch ) ) );
}

template <>
void L2MPS::createBpmParam(const std::string param, const int bay, const bpm_channel_t ch, BpmR32_t pFuncR, BpmW32_t pFuncW)
{
    int index;
    std::stringstream pName;
    pName.str("");
    pName << param << "_" << bay << ch[0] << ch[1]  << ch[2]  << ch[3];
    
    createParam(bay, pName.str().c_str(), asynParamInt32, &index);

    fMapBpmR32.insert( std::make_pair( pName.str(), std::make_pair( pFuncR, ch ) ) );
    fMapBpmW32.insert( std::make_pair( pName.str(), std::make_pair( pFuncW, ch ) ) );
}

template <>
void L2MPS::createBpmParam(const std::string param, const int bay, const bpm_channel_t ch, BpmR1_t pFuncR)
{
    int index;
    std::stringstream pName;
    pName.str("");
    pName << param << "_" << bay << ch[0] << ch[1]  << ch[2]  << ch[3];
    
    createParam(bay, pName.str().c_str(), asynParamUInt32Digital, &index);

    fMapBpmR1.insert( std::make_pair( pName.str(), std::make_pair( pFuncR, ch ) ) );
}

template <>
void L2MPS::createBpmParam(const std::string param, const int bay, const bpm_channel_t ch, BpmR1_t pFuncR, BpmW1_t pFuncW)
{
    int index;
    std::stringstream pName;
    pName.str("");
    pName << param << "_" << bay << ch[0] << ch[1]  << ch[2]  << ch[3];
    
    createParam(bay, pName.str().c_str(), asynParamUInt32Digital, &index);

    fMapBpmR1.insert( std::make_pair( pName.str(), std::make_pair( pFuncR, ch ) ) );
    fMapBpmW1.insert( std::make_pair( pName.str(), std::make_pair( pFuncW, ch ) ) );
}

// BLEN parameter creators
template <>
void L2MPS::createBlenParam(const std::string param, const int bay, const blen_channel_t ch, BlenR32_t pFuncR)
{
    int index;
    std::stringstream pName;
    pName.str("");
    pName << param << "_" << bay << "0" << ch[0] << ch[1]  << ch[2];
    
    createParam(bay, pName.str().c_str(), asynParamInt32, &index);

    fMapBlenR32.insert( std::make_pair( pName.str(), std::make_pair( pFuncR, ch ) ) );
}

template <>
void L2MPS::createBlenParam(const std::string param, const int bay, const blen_channel_t ch, BlenR32_t pFuncR, BlenW32_t pFuncW)
{
    int index;
    std::stringstream pName;
    pName.str("");
    pName << param << "_" << bay << "0" << ch[0] << ch[1]  << ch[2];
    
    createParam(bay, pName.str().c_str(), asynParamInt32, &index);

    fMapBlenR32.insert( std::make_pair( pName.str(), std::make_pair( pFuncR, ch ) ) );
    fMapBlenW32.insert( std::make_pair( pName.str(), std::make_pair( pFuncW, ch ) ) );
}

template <>
void L2MPS::createBlenParam(const std::string param, const int bay, const blen_channel_t ch, BlenR1_t pFuncR)
{
    int index;
    std::stringstream pName;
    pName.str("");
    pName << param << "_" << bay << "0" << ch[0] << ch[1]  << ch[2];
    
    createParam(bay, pName.str().c_str(), asynParamUInt32Digital, &index);

    fMapBlenR1.insert( std::make_pair( pName.str(), std::make_pair( pFuncR, ch ) ) );
}

template <>
void L2MPS::createBlenParam(const std::string param, const int bay, const blen_channel_t ch, BlenR1_t pFuncR, BlenW1_t pFuncW)
{
    int index;
    std::stringstream pName;
    pName.str("");
    pName << param << "_" << bay << "0" << ch[0] << ch[1]  << ch[2];
    
    createParam(bay, pName.str().c_str(), asynParamUInt32Digital, &index);

    fMapBlenR1.insert( std::make_pair( pName.str(), std::make_pair( pFuncR, ch ) ) );
    fMapBlenW1.insert( std::make_pair( pName.str(), std::make_pair( pFuncW, ch ) ) );
}

// BCM parameter creators
template <>
void L2MPS::createBcmParam(const std::string param, const int bay, const bcm_channel_t ch, BcmR32_t pFuncR)
{
    int index;
    std::stringstream pName;
    pName.str("");
    pName << param << "_" << bay << ch[0] << ch[1]  << ch[2] << ch[3];
    
    createParam(bay, pName.str().c_str(), asynParamInt32, &index);

    fMapBcmR32.insert( std::make_pair( pName.str(), std::make_pair( pFuncR, ch ) ) );
}

template <>
void L2MPS::createBcmParam(const std::string param, const int bay, const bcm_channel_t ch, BcmR32_t pFuncR, BcmW32_t pFuncW)
{
    int index;
    std::stringstream pName;
    pName.str("");
    pName << param << "_" << bay << ch[0] << ch[1]  << ch[2] << ch[3];
    
    createParam(bay, pName.str().c_str(), asynParamInt32, &index);

    fMapBcmR32.insert( std::make_pair( pName.str(), std::make_pair( pFuncR, ch ) ) );
    fMapBcmW32.insert( std::make_pair( pName.str(), std::make_pair( pFuncW, ch ) ) );
}

template <>
void L2MPS::createBcmParam(const std::string param, const int bay, const bcm_channel_t ch, BcmR1_t pFuncR)
{
    int index;
    std::stringstream pName;
    pName.str("");
    pName << param << "_" << bay << ch[0] << ch[1]  << ch[2] << ch[3];
    
    createParam(bay, pName.str().c_str(), asynParamUInt32Digital, &index);

    fMapBcmR1.insert( std::make_pair( pName.str(), std::make_pair( pFuncR, ch ) ) );
}

template <>
void L2MPS::createBcmParam(const std::string param, const int bay, const bcm_channel_t ch, BcmR1_t pFuncR, BcmW1_t pFuncW)
{
    int index;
    std::stringstream pName;
    pName.str("");
    pName << param << "_" << bay << ch[0] << ch[1]  << ch[2] << ch[3];
    
    createParam(bay, pName.str().c_str(), asynParamUInt32Digital, &index);

    fMapBcmR1.insert( std::make_pair( pName.str(), std::make_pair( pFuncR, ch ) ) );
    fMapBcmW1.insert( std::make_pair( pName.str(), std::make_pair( pFuncW, ch ) ) );
}

// BLEN callback functon
void L2MPS::setBlmCB(int bay, blm_dataMap_t data)
{
    pL2MPS->BlmCB(bay, data);
}

void L2MPS::BlmCB(int bay, blm_dataMap_t data)
{
    for (blm_dataMap_t::iterator data_blmIt = data.begin(); data_blmIt != data.end(); ++data_blmIt)
    {
        // Process the Threshold info
        blm_paramMap_t::iterator param_blmIt = _blmParamMap.find(data_blmIt->first);
        if (param_blmIt != _blmParamMap.end())
        {   
            thr_chInfoData_t thrInfo = (data_blmIt->second).info;
            setIntegerParam(bay,     (param_blmIt->second).ch,        thrInfo.ch);    
            setIntegerParam(bay,     (param_blmIt->second).count,     thrInfo.count);    
            setIntegerParam(bay,     (param_blmIt->second).byteMap,   thrInfo.byteMap);    
            setUIntDigitalParam(bay, (param_blmIt->second).idleEn,   thrInfo.idleEn,   0xFFFFFFFF, 0x1);
            setUIntDigitalParam(bay, (param_blmIt->second).altEn,    thrInfo.altEn,    0xFFFFFFFF, 0x1);
            setUIntDigitalParam(bay, (param_blmIt->second).lcls1En,  thrInfo.lcls1En,  0xFFFFFFFF, 0x1);

            blm_channel_t data_blmCh = data_blmIt->first;
            thr_chData_t  data_thr   = (data_blmIt->second).data;

            blm_channel_t param_blmCh = param_blmIt->first;
            thr_chParam_t param_thr   = (param_blmIt->second).data;

            for (thr_chData_t::iterator data_thrIt = data_thr.begin(); data_thrIt != data_thr.end(); ++data_thrIt)
            {
                thr_table_t     data_thrCh = data_thrIt->first;
                thr_tableData_t data_data  = data_thrIt->second;

                thr_chParam_t::iterator param_thrIt = param_thr.find(data_thrCh);

                if (param_thrIt != param_thr.end())
                {
                    thr_table_t         param_thrCh = param_thrIt->first;
                    thr_tableParam_t    param_param = param_thrIt->second;

                    setUIntDigitalParam(bay, param_param.minEn, data_data.minEn, 0xFFFFFFFF, 0x1);
                    setUIntDigitalParam(bay, param_param.maxEn, data_data.maxEn, 0xFFFFFFFF, 0x1);
                    setIntegerParam(bay, param_param.min, data_data.min);    
                    setIntegerParam(bay, param_param.max, data_data.max);    
                }
            }

            callParamCallbacks(bay);
        }
    } 
}
 
L2MPS::L2MPS(const char *portName, const uint16_t appId, const std::string recordPrefixMps, const std::array<std::string, numberOfBays> recordPrefixBay,  std::string mpsRootPath)
    : asynPortDriver(
            portName,
            MAX_SIGNALS,
            NUM_PARAMS,
            asynInt32Mask | asynDrvUserMask | asynInt16ArrayMask | asynInt32ArrayMask | asynOctetMask | \
            asynFloat64ArrayMask | asynUInt32DigitalMask | asynFloat64Mask,                             // Interface Mask
            asynInt16ArrayMask | asynInt32ArrayMask | asynInt32Mask | asynUInt32DigitalMask,            // Interrupt Mask
            ASYN_MULTIDEVICE | ASYN_CANBLOCK,                                                           // asynFlags
            1,                                                                                          // Autoconnect
            0,                                                                                          // Default priority
            0),                                                                                         // Default stack size
        portName_(portName),
        driverName_(DRIVER_NAME),
        recordPrefixMps_(recordPrefixMps),
        recordPrefixBay_(recordPrefixBay)
{
    Path root = cpswGetRoot();

    try
    {
        if (mpsRootPath.empty())
        {
            mpsRootPath = defaultMpsRootPath;
            printf("Not mps root was defined. Using default path: %s\n", mpsRootPath.c_str());
        }
        else
        {
            printf("Using MPS root path: %s\n", mpsRootPath.c_str());
        }

        Path mpsRoot = root->findByName(mpsRootPath.c_str());

        node_ = MpsNodeFactory::create(mpsRoot);
        node_->setAppId(appId);
        std::string appType_ = node_->getAppType();

        // Create parameters fpor the MPS node
        createParam(appIdString,            asynParamInt32,         &appIdValue_)       ;
        createParam(byteCountString,        asynParamInt32,         &byteCountValue_    );
        createParam(beamDestMaskString,     asynParamInt32,         &beamDestMaskValue_ );
        createParam(altDestMaskString,      asynParamInt32,         &altDestMaskValue_  );
        createParam(mpsEnString,            asynParamUInt32Digital, &mpsEnValue_        );
        createParam(lcl1ModeString,         asynParamUInt32Digital, &lcl1ModeValue_     );
        createParam(digitalEnString,        asynParamUInt32Digital, &digitalEnValue_    );
        createParam(appTypeString,          asynParamOctet,         &appTypeValue_      );
        createParam(rollOverEnString,       asynParamInt32,         &rollOverEnValue_   );
        createParam(mpsSlotString,          asynParamUInt32Digital, &mpsSlotValue_      );
        createParam(pllLockedString,        asynParamUInt32Digital, &pllLockedValue_    );
        createParam(txLinkUpString,         asynParamUInt32Digital, &txLinkUpValue_     );
        createParam(txLinkUpCntString,      asynParamInt32,         &txLinkUpCntValue_  );

        createParam(txPktSentCntString,     asynParamInt32,         &txPktSentCntValue_  );

        createParam(mpsMsgCntString,        asynParamInt32,         &mpsMsgCntValue_        );
        createParam(mpsLastMsgAppIdString,  asynParamInt32,         &mpsLastMsgAppIdValue_  );
        createParam(mpsLastMsgTmstmpString, asynParamInt32,         &mpsLastMsgTmstmpValue_ );
        createParam(mpsLastMsgLclsString,   asynParamUInt32Digital, &mpsLastMsgLclsValue_   );
        createParam(mpsSlatRstCntString,    asynParamUInt32Digital, &mpsSlatRstCntValue_    );
        createParam(mpsSlatRstPllString,    asynParamUInt32Digital, &mpsSlatRstPlltValue_   );

        for (std::size_t i {0}; i < numberOfRxLinks; ++i)
        {
            std::stringstream paramName;
            paramName.str("");
            paramName << rxLinkUpString << "_" << i;
            createParam(paramName.str().c_str(), asynParamUInt32Digital, &rxLinkUpValue_[i]);

            paramName.str("");
            paramName << rxLinkUpCntString << "_" << i;
            createParam(paramName.str().c_str(), asynParamInt32, &rxLinkUpCntValue_[i]);

            paramName.str("");
            paramName << rxPktRcvdSentCntString << "_" << i;
            createParam(paramName.str().c_str(), asynParamInt32, &rxPktRcvdSentCntValue_[i]);
        }

        for (std::size_t i {0}; i < node_->getLastMsgByteSize(); ++i)
        {
            int paramIndex;
            std::stringstream paramName;
            paramName.str("");
            paramName << mpsLastMsgByteString << "_" << i;
            createParam(paramName.str().c_str(), asynParamInt32, &paramIndex);
            mpsLastMsgByteValue_.push_back(paramIndex);
        }
        
        std::string dbParams = "P=" + std::string(recordPrefixMps_) + ",PORT=" + std::string(portName_);
        dbLoadRecords("db/mps.db", dbParams.c_str());

        for(std::size_t i {0}; i < numberOfBays; ++i)
        {
            if (!recordPrefixBay_[i].empty())
            {
                // std::string dbParams = "P=" + std::string(recordPrefix_[i]) + ",PORT=" + std::string(portName_);
                // dbLoadRecords("db/mps.db", dbParams.c_str());

                if (!appType_.compare("BPM"))
                {
                    amc[i] = MpsBpmFactory::create(mpsRoot, i);
                    InitBpmMaps(i);
                }
                else if (!appType_.compare("BLEN"))
                {
                    amc[i] = MpsBpmFactory::create(mpsRoot, i);
                    InitBlenMaps(i);                    
                }
                else if (!appType_.compare("BCM"))
                {
                    amc[i] = MpsBcmFactory::create(mpsRoot, i);
                    InitBcmMaps(i);                    
                }
                else if ((!appType_.compare("BLM")) | (!appType_.compare("MPS_6CH")) | (!appType_.compare("MPS_24CH")))
                {
                    amc[i] = MpsBlmFactory::create(mpsRoot, i, &setBlmCB);
                    InitBlmMaps(i);
                }
            }
        }
    }
    catch (CPSWError &e)
    {
        printf("CPSW error: %s not found!\n", e.getInfo().c_str());
    }

}

void L2MPS::InitBpmMaps(const int bay)
{

    // for (int i = 0; i < numBpmChs; ++i)
    // {
    //     createBpmParam(std::string("BPM_THRNUM"),   bay, std::array<int,4>{{i, 0, 0, 0}}, &IMpsBpm::getCh);
    //     createBpmParam(std::string("BPM_THRCNT"),   bay, std::array<int,4>{{i, 0, 0, 0}}, &IMpsBpm::getThrCount);
    //     createBpmParam(std::string("BPM_BYTEMAP"),  bay, std::array<int,4>{{i, 0, 0, 0}}, &IMpsBpm::getByteMap);

    //     createBpmParam(std::string("BPM_IDLEEN"),   bay, std::array<int,4>{{i,0, 0, 0}},  &IMpsBpm::getIdleEn);
    //     createBpmParam(std::string("BPM_ALTEN"),    bay, std::array<int,4>{{i,0, 0, 0}},  &IMpsBpm::getAltEn);
    //     createBpmParam(std::string("BPM_LCLS1EN"),  bay, std::array<int,4>{{i,0, 0, 0}},  &IMpsBpm::getLcls1En);

    //     for (int j = 0; j < numThrTables; ++j)
    //     {
    //         for (int k = 0; k < numThrLimits; ++k)
    //         {
    //             for (int m = 0; m < numThrCounts[j]; ++m)
    //             {
    //                 createBpmParam(std::string("BPM_THR"),      bay, std::array<int,4>{{i, j, k, m}}, &IMpsBpm::getThreshold,   &IMpsBpm::setThreshold);
    //                 createBpmParam(std::string("BPM_THREN"),    bay, std::array<int,4>{{i, j, k, m}}, &IMpsBpm::getThresholdEn, &IMpsBpm::setThresholdEn);
    //             }
    //         }
    //     }
    // }

    // std::stringstream bpmDbParams;
    // bpmDbParams.str("");
    // bpmDbParams << "P=" << std::string(recordPrefixBay_[bay]);
    // bpmDbParams << ",PORT=" << std::string(portName_);
    // bpmDbParams << ",BAY=" << bay;
    // dbLoadRecords("db/bpm.db", bpmDbParams.str().c_str());
}

void L2MPS::InitBlenMaps(const int bay)
{

    // createBlenParam(std::string("BLEN_THRNUM"),   bay, std::array<int,3>{{0, 0, 0}}, &IMpsBlen::getCh);
    // createBlenParam(std::string("BLEN_THRCNT"),   bay, std::array<int,3>{{0, 0, 0}}, &IMpsBlen::getThrCount);
    // createBlenParam(std::string("BLEN_BYTEMAP"),  bay, std::array<int,3>{{0, 0, 0}}, &IMpsBlen::getByteMap);

    // createBlenParam(std::string("BLEN_IDLEEN"),   bay, std::array<int,3>{{0, 0, 0}},  &IMpsBlen::getIdleEn);
    // createBlenParam(std::string("BLEN_ALTEN"),    bay, std::array<int,3>{{0, 0, 0}},  &IMpsBlen::getAltEn);
    // createBlenParam(std::string("BLEN_LCLS1EN"),  bay, std::array<int,3>{{0, 0, 0}},  &IMpsBlen::getLcls1En);

    // for (int j = 0; j < numThrTables; ++j)
    // {
    //     for (int k = 0; k < numThrLimits; ++k)
    //     {
    //         for (int m = 0; m < numThrCounts[j]; ++m)
    //         {
    //             createBlenParam(std::string("BLEN_THR"),      bay, std::array<int,3>{{j, k, m}}, &IMpsBlen::getThreshold,   &IMpsBlen::setThreshold);
    //             createBlenParam(std::string("BLEN_THREN"),    bay, std::array<int,3>{{j, k, m}}, &IMpsBlen::getThresholdEn, &IMpsBlen::setThresholdEn);
    //         }
    //     }
    // }
    
    // std::stringstream blenDbParams;
    // blenDbParams.str("");
    // blenDbParams << "P=" << std::string(recordPrefixBay_[bay]);
    // blenDbParams << ",PORT=" << std::string(portName_);
    // blenDbParams << ",BAY=" << bay;
    // dbLoadRecords("db/blen.db", blenDbParams.str().c_str());
}

void L2MPS::InitBcmMaps(const int bay)
{

    // for (int i = 0; i < numBcmChs; ++i)
    // {
    //     createBcmParam(std::string("BCM_THRNUM"),   bay, std::array<int,4>{{i, 0, 0, 0}}, &IMpsBcm::getCh);
    //     createBcmParam(std::string("BCM_THRCNT"),   bay, std::array<int,4>{{i, 0, 0, 0}}, &IMpsBcm::getThrCount);
    //     createBcmParam(std::string("BCM_BYTEMAP"),  bay, std::array<int,4>{{i, 0, 0, 0}}, &IMpsBcm::getByteMap);

    //     createBcmParam(std::string("BCM_IDLEEN"),   bay, std::array<int,4>{{i,0, 0, 0}},  &IMpsBcm::getIdleEn);
    //     createBcmParam(std::string("BCM_ALTEN"),    bay, std::array<int,4>{{i,0, 0, 0}},  &IMpsBcm::getAltEn);
    //     createBcmParam(std::string("BCM_LCLS1EN"),  bay, std::array<int,4>{{i,0, 0, 0}},  &IMpsBcm::getLcls1En);

    //     for (int j = 0; j < numThrTables; ++j)
    //     {
    //         for (int k = 0; k < numThrLimits; ++k)
    //         {
    //             for (int m = 0; m < numThrCounts[j]; ++m)
    //             {
    //                 createBcmParam(std::string("BCM_THR"),      bay, std::array<int,4>{{i, j, k, m}}, &IMpsBcm::getThreshold,   &IMpsBcm::setThreshold);
    //                 createBcmParam(std::string("BCM_THREN"),    bay, std::array<int,4>{{i, j, k, m}}, &IMpsBcm::getThresholdEn, &IMpsBcm::setThresholdEn);
    //             }
    //         }
    //     }
    // }

    // std::stringstream bcmDbParams;
    // bcmDbParams.str("");
    // bcmDbParams << "P=" << std::string(recordPrefixBay_[bay]);
    // bcmDbParams << ",PORT=" << std::string(portName_);
    // bcmDbParams << ",BAY=" << bay;
    // dbLoadRecords("db/bcm.db", bcmDbParams.str().c_str());
}

void L2MPS::InitBlmMaps(const int bay)
{
    int index;
    std::stringstream pName;


    for (int i = 0; i < numBlmChs; ++i)
    {
        for(int j = 0; j < numBlmIntChs; ++j)
        {
            blm_channel_t thisBlmCh = blm_channel_t{{i, j}};

            thr_paramMap_t thrParamMap;

            pName.str("");
            pName << "_" << bay << i << j;

            createParam(bay, ("BLM_THRNUM" + pName.str()).c_str(), asynParamInt32, &index);
            thrParamMap.ch = index;

            createParam(bay, ("BLM_THRCNT" + pName.str()).c_str(), asynParamInt32, &index);
            thrParamMap.count = index;

            createParam(bay, ("BLM_BYTEMAP" + pName.str()).c_str(), asynParamInt32, &index);
            thrParamMap.byteMap = index;

            createParam(bay, ("BLM_IDLEEN" + pName.str()).c_str(), asynParamUInt32Digital, &index);
            thrParamMap.idleEn = index;

            createParam(bay, ("BLM_IDLEEN" + pName.str()).c_str(), asynParamUInt32Digital, &index);
            thrParamMap.altEn = index;

            createParam(bay, ("BLM_LCLS1EN" + pName.str()).c_str(), asynParamUInt32Digital, &index);
            thrParamMap.lcls1En = index;


            thr_chParam_t thrChParamMap;
            for (int k = 0; k < numThrTables; ++k)
            {
                    for (int n = 0; n < numThrCounts[k]; ++n)
                    {
                        thr_table_t thisThrTable = thr_table_t{{k,n }};               
                        blmThr_channel_t args = {thisBlmCh, thisThrTable};

                        thr_tableParam_t tp;

                        pName.str("");
                        pName << "_" << bay << i << j  << k << n;

                        createParam(bay, ("BLM_THRMIN" + pName.str()).c_str(), asynParamInt32, &index);
                        tp.min = index;
                        fMapBlmW32.insert( std::make_pair( index, std::make_pair( &IMpsBlm::setThresholdMin,  args) ) );

                        createParam(bay, ("BLM_THRMAX" + pName.str()).c_str(), asynParamInt32, &index);
                        tp.max = index;
                        fMapBlmW32.insert( std::make_pair( index, std::make_pair( &IMpsBlm::setThresholdMax, args) ) );

                        createParam(bay, ("BLM_THRMINEN" + pName.str()).c_str(), asynParamUInt32Digital, &index);
                        tp.minEn = index;
                        fMapBlmW1.insert( std::make_pair( index, std::make_pair( &IMpsBlm::setThresholdMinEn, args ) ) );

                        createParam(bay, ("BLM_THRMAXEN" + pName.str()).c_str(), asynParamUInt32Digital, &index);
                        tp.maxEn = index;
                        fMapBlmW1.insert( std::make_pair( index, std::make_pair( &IMpsBlm::setThresholdMaxEn, args ) ) );

                        thr_table_t    tt = thr_table_t{{k, n}};
                        thrChParamMap.insert(std::make_pair(tt, tp));
                    }
            }
            thrParamMap.data = thrChParamMap;
            _blmParamMap.insert(std::make_pair( thisBlmCh, thrParamMap ));

        }
    }

    std::stringstream blenDbParams;
    blenDbParams.str("");
    blenDbParams << "P=" << std::string(recordPrefixBay_[bay]);
    blenDbParams << ",PORT=" << std::string(portName_);
    blenDbParams << ",BAY=" << bay;
    dbLoadRecords("db/blm.db", blenDbParams.str().c_str());
}

asynStatus L2MPS::readInt32(asynUser *pasynUser, epicsInt32 *value)
{
    int addr;
    int function = pasynUser->reason;
    int status = 0;
    static const char *functionName = "readInt32";
    const char *name;

    this->getAddress(pasynUser, &addr);
    
    getParamName(addr, function, &name);

    try
    {
        // Rx Link Up Counter iterator
        std::array<int, numberOfRxLinks>::iterator it;

        // Last message byte iterator
        std::vector<int>::iterator mpsLastMsgByte_it;

        bpm_fmap_r32_t::iterator bpm_it;
        blen_fmap_r32_t::iterator blen_it;
        bcm_fmap_r32_t::iterator bcm_it;
        // blm_fmap_r32_t::iterator blm_it;

        // BPM parameters
        if ((bpm_it = fMapBpmR32.find(name)) != fMapBpmR32.end())
        {
            *value = ((*boost::any_cast<MpsBpm>(amc[addr])).*(bpm_it->second.first))(bpm_it->second.second);
        }
        // BLEN parameters
        else if ((blen_it = fMapBlenR32.find(name)) != fMapBlenR32.end())
        {
            *value = ((*boost::any_cast<MpsBlen>(amc[addr])).*(blen_it->second.first))(blen_it->second.second);   
        }
        // BCM parameters
        else if ((bcm_it = fMapBcmR32.find(name)) != fMapBcmR32.end())
        {
            *value = ((*boost::any_cast<MpsBcm>(amc[addr])).*(bcm_it->second.first))(bcm_it->second.second);   
        }
        // // BLM parameters
        // else if ((blm_it = fMapBlmR32.find(name)) != fMapBlmR32.end())
        // {
        //     *value = ((*boost::any_cast<MpsBlm>(amc[addr])).*(blm_it->second.first))(blm_it->second.second);   
        // }
        // MPS node parameters
        else if (function == appIdValue_)   
        {   
            *value = (epicsInt32)node_->getAppId();
        }
        else if (function == byteCountValue_)
        {
            *value = (epicsInt32)node_->getByteCount();
        }
        else if (function == beamDestMaskValue_)
        {
            *value = (epicsInt32)node_->getBeamDestMask();
        }
        else if (function == altDestMaskValue_)
        {      
            *value = (epicsInt32)node_->getAltDestMask();         
        }
        else if (function == txLinkUpCntValue_)
        {
            *value = (epicsInt32)node_->getTxLinkUpCnt();
        }
        else if (function == txPktSentCntValue_)
        {
            *value = (epicsInt32)node_->getTxPktSentCnt();
        }
        else if ((it = std::find(rxLinkUpCntValue_.begin(), rxLinkUpCntValue_.end(), function)) != rxLinkUpCntValue_.end())
        {
            *value = (epicsInt32)node_->getRxLinkUpCnt(it - rxLinkUpCntValue_.begin());   
        }
        else if ((it = std::find(rxPktRcvdSentCntValue_.begin(), rxPktRcvdSentCntValue_.end(), function)) != rxPktRcvdSentCntValue_.end())
        {
            *value = (epicsInt32)node_->getRxPktRcvdSentCnt(it - rxPktRcvdSentCntValue_.begin());   
        }
        else if (function == rollOverEnValue_)
        {
            *value = (epicsInt32)node_->getRollOverEn();
        }
        else if (function == mpsMsgCntValue_)
        {
            *value = (epicsInt32)node_->getMpsMsgCount();
        }
        else if (function == mpsLastMsgAppIdValue_)
        {
            *value = (epicsInt32)node_->getLastMsgAppId();
        }
        else if (function == mpsLastMsgTmstmpValue_)
        {
            *value = (epicsInt32)node_->getLastMsgTimeStamp();
        }
        else if ((mpsLastMsgByte_it = std::find(mpsLastMsgByteValue_.begin(), mpsLastMsgByteValue_.end(), function)) != mpsLastMsgByteValue_.end())
        {
            *value = (epicsInt32)node_->getLastMsgByte(mpsLastMsgByte_it - mpsLastMsgByteValue_.begin());   
        }
        else
        {    
            status = asynPortDriver::readInt32(pasynUser, value);
        }
    }
    catch (CPSWError &e)
    {
        *value = 0;
        status = -1;
        asynPrint(pasynUser, ASYN_TRACE_ERROR, "CPSW Error on %s reading parameter %s: %s\n", functionName, name, e.getInfo().c_str());
    }  
    catch (std::runtime_error &e)
    {
        *value = 0;
        status = -1;
        asynPrint(pasynUser, ASYN_TRACE_ERROR, "Runtime error on %s reading parameter %s: %s\n", functionName, name, e.what());
    } 

    return (status == 0) ? asynSuccess : asynError;
}

asynStatus L2MPS::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
    int addr;
    int function = pasynUser->reason;
    int status=0;
    const char *name;

    this->getAddress(pasynUser, &addr);

    static const char *functionName = "writeInt32";

    this->getAddress(pasynUser, &addr);

    getParamName(addr, function, &name);

    try
    {
        bpm_fmap_w32_t::iterator bpm_it;
        blen_fmap_w32_t::iterator blen_it;
        bcm_fmap_w32_t::iterator bcm_it;
        blm_fmap_w32_t::iterator blm_it;
        // blm_fmap_w32_t_new::iterator blm_it_new;

        // BPM parameters
        if ((bpm_it = fMapBpmW32.find(name)) != fMapBpmW32.end())
        {
            ((*boost::any_cast<MpsBpm>(amc[addr])).*(bpm_it->second.first))(bpm_it->second.second, value);
        }
        // BLEN parameters
        else if ((blen_it = fMapBlenW32.find(name)) != fMapBlenW32.end())
        {
            ((*boost::any_cast<MpsBlen>(amc[addr])).*(blen_it->second.first))(blen_it->second.second, value);
        }
        // BCM parameters
        else if ((bcm_it = fMapBcmW32.find(name)) != fMapBcmW32.end())
        {
            ((*boost::any_cast<MpsBcm>(amc[addr])).*(bcm_it->second.first))(bcm_it->second.second, value);
        }
        // BLM parameters
        else if ((blm_it = fMapBlmW32.find(function)) != fMapBlmW32.end())
        {
            ((*boost::any_cast<MpsBlm>(amc[addr])).*(blm_it->second.first))(blm_it->second.second, value);
        }
// else if ((blm_it_new = fMapBlmW32_new.find(function)) != fMapBlmW32_new.end())
// {
//     ((*boost::any_cast<MpsBlm>(amc[addr])).*(blm_it_new->second.first))(blm_it_new->second.second, value);
// }
        // MPS node parameters
        else if (function == beamDestMaskValue_)
        {
            node_->setBeamDestMask(value);
        }
        else if (function == altDestMaskValue_)
        {      
            node_->setAltDestMask(value);
        }
        else
        {
            status == asynPortDriver::writeInt32(pasynUser, value);
        }
    }
    catch (CPSWError &e)
    {
        status = -1;
        asynPrint(pasynUser, ASYN_TRACE_ERROR, "CPSW Error on %s writting parameter %s: %s\n", functionName, name, e.getInfo().c_str());
    }  
    catch (std::runtime_error &e)
    {
        status = -1;
        asynPrint(pasynUser, ASYN_TRACE_ERROR, "Runtime error on %s writting parameter %s: %s\n", functionName, name, e.what());
    }

    return (status == 0) ? asynSuccess : asynError;
}

asynStatus L2MPS::readUInt32Digital(asynUser *pasynUser, epicsUInt32 *value, epicsUInt32 mask)
{
    int addr;
    int function = pasynUser->reason;
    int status=0;
    const char *name;

    this->getAddress(pasynUser, &addr);

    getParamName(addr, function, &name);

    static const char *functionName = "readUInt32Digital";

    try
    {
        // Rx Link Up Counter iterator
        std::array<int, numberOfRxLinks>::iterator it;

        bpm_fmap_r1_t::iterator bpm_it;
        blen_fmap_r1_t::iterator blen_it;
        bcm_fmap_r1_t::iterator bcm_it;        
        // blm_fmap_r1_t::iterator blm_it;


        // BPM parameters        
        if ((bpm_it = fMapBpmR1.find(name)) != fMapBpmR1.end())
        {
            *value = ((*boost::any_cast<MpsBpm>(amc[addr])).*(bpm_it->second.first))(bpm_it->second.second);
        }
        // BLEN parameters
        else if ((blen_it = fMapBlenR1.find(name)) != fMapBlenR1.end())
        {
            *value = ((*boost::any_cast<MpsBlen>(amc[addr])).*(blen_it->second.first))(blen_it->second.second);
        }
        // BCM parameters
        else if ((bcm_it = fMapBcmR1.find(name)) != fMapBcmR1.end())
        {
            *value = ((*boost::any_cast<MpsBcm>(amc[addr])).*(bcm_it->second.first))(bcm_it->second.second);
        }
        // BLM parameters
        // else if ((blm_it = fMapBlmR1.find(name)) != fMapBlmR1.end())
        // {
        //     *value = ((*boost::any_cast<MpsBlm>(amc[addr])).*(blm_it->second.first))(blm_it->second.second);
        // }        
        // MPS node parameters        
        else if(function == mpsEnValue_)
        {
            *value = (epicsUInt32)(node_->getEnable() & mask);
            setUIntDigitalParam(addr, function, *value, mask);
        }
        else if (function == lcl1ModeValue_)
        {
            *value = (epicsUInt32)(node_->getLcls1Mode() & mask);
            setUIntDigitalParam(addr, function, *value, mask);
        }
        else if (function == digitalEnValue_)
        {
            *value = (epicsUInt32)(node_->getDigitalEnable() & mask);
            setUIntDigitalParam(addr, function, *value, mask);
        } 
        else if (function == txLinkUpValue_)
        {
            *value = (epicsUInt32)(node_->getTxLinkUp() & mask);
            setUIntDigitalParam(addr, function, *value, mask);  
        } 
        else if ((it = std::find(rxLinkUpValue_.begin(), rxLinkUpValue_.end(), function)) != rxLinkUpValue_.end())
        {
            *value = (epicsUInt32)(node_->getRxLinkUp(it - rxLinkUpValue_.begin()) & mask);
            setUIntDigitalParam(addr, function, *value, mask);  
        } 
        else if (function == mpsSlotValue_)
        {
            *value = (epicsUInt32)(node_->getMpsSlot() & mask);
            setUIntDigitalParam(addr, function, *value, mask);  
        } 
        else if (function == pllLockedValue_)
        {
            *value = (epicsUInt32)(node_->getPllLocked() & mask);
            setUIntDigitalParam(addr, function, *value, mask);  
        }
        else if (function == mpsLastMsgLclsValue_)
        {
            *value = (epicsUInt32)(node_->getLastMsgLcls() & mask);
            setUIntDigitalParam(addr, function, *value, mask);              
        }
        else
        {
            status = asynPortDriver::readUInt32Digital(pasynUser, value, mask);
        }
    }
    catch (CPSWError &e)
    {
        *value = 0;
        status = -1;
        asynPrint(pasynUser, ASYN_TRACE_ERROR, "CPSW Error on %s reading parameter %s: %s\n", functionName, name, e.getInfo().c_str());           
    }
    catch (std::runtime_error &e)
    {
        *value = 0;
        status = -1;
        asynPrint(pasynUser, ASYN_TRACE_ERROR, "Runtime error on %s reading parameter %s: %s\n", functionName, name, e.what());
    }

    return (status == 0) ? asynSuccess : asynError;
}

asynStatus L2MPS::writeUInt32Digital(asynUser *pasynUser, epicsUInt32 value, epicsUInt32 mask)
{
    int addr;
    int function = pasynUser->reason;
    int status=0;
    const char *name;

    this->getAddress(pasynUser, &addr);

    getParamName(addr, function, &name);

    static const char *functionName = "writeUInt32Digital";

    try
    {
        bpm_fmap_w1_t::iterator bpm_it;
        blen_fmap_w1_t::iterator blen_it;
        bcm_fmap_w1_t::iterator bcm_it;
        blm_fmap_w1_t::iterator blm_it;

        // BPM parameters        
        if ((bpm_it = fMapBpmW1.find(name)) != fMapBpmW1.end())
        {
            ((*boost::any_cast<MpsBpm>(amc[addr])).*(bpm_it->second.first))(bpm_it->second.second, (value & mask));
        }
        // BLEN parameters        
        else if ((blen_it = fMapBlenW1.find(name)) != fMapBlenW1.end())
        {
            ((*boost::any_cast<MpsBlen>(amc[addr])).*(blen_it->second.first))(blen_it->second.second, (value & mask));
        }
        // BCM parameters        
        else if ((bcm_it = fMapBcmW1.find(name)) != fMapBcmW1.end())
        {
            ((*boost::any_cast<MpsBcm>(amc[addr])).*(bcm_it->second.first))(bcm_it->second.second, (value & mask));
        }
        // BLM parameters        
        else if ((blm_it = fMapBlmW1.find(function)) != fMapBlmW1.end())
        {
            ((*boost::any_cast<MpsBlm>(amc[addr])).*(blm_it->second.first))(blm_it->second.second, (value & mask));
        }
        // MPS node parameters        
        else if(function == mpsEnValue_)
        {
            node_->setEnable(value & mask);
        }
        else if (function == lcl1ModeValue_)
        {
            node_->setLcls1Mode(value & mask);
        }
        else if (function == mpsSlatRstCntValue_)
        {
            node_->resetSaltCnt();
        }
        else if (function == mpsSlatRstPlltValue_)
        {
            node_->resetSaltPll();
        }
        else
        {
            status = asynPortDriver::writeUInt32Digital(pasynUser, value, mask);
        }
    }
    catch (CPSWError &e)
    {
        status = -1;
        asynPrint(pasynUser, ASYN_TRACE_ERROR, "CPSW Error on %s writting parameter %s: %s\n", functionName, name, e.getInfo().c_str());           
    }
    catch (std::runtime_error &e)
    {
        status = -1;
        asynPrint(pasynUser, ASYN_TRACE_ERROR, "Runtime error on %s writting parameter %s: %s\n", functionName, name, e.what());
    }

    return (status == 0) ? asynSuccess : asynError;
}

asynStatus  L2MPS::readOctet(asynUser *pasynUser, char *value, size_t maxChars, size_t *nActual, int *eomReason)
{
    int function = pasynUser->reason;
    int status=0;

    if (function == appTypeValue_)
    {
        std::string appT = node_->getAppType();
        strcpy(value, appT.c_str());
        *nActual = appT.length();
    }
    else
    {
        status = asynPortDriver::readOctet(pasynUser, value, maxChars, nActual, eomReason);
    }
    
    return (status == 0) ? asynSuccess : asynError;
}

// + L2MPSASYNConfig //
extern "C" int L2MPSASYNConfig(const char *portName, const int appID, const char *recordPrefixMps, const char *recordPrefixBay0, const char *recordPrefixBay1, const char* mpsRoot)
{
    int status = 0;
    
    if (0 == strlen(recordPrefixMps))
    {
        printf("  ERROR: The MSP prefix must be defined!\n");
        return asynError;
    }

    std::string recPreMps = std::string(recordPrefixMps);
    std::array<std::string, numberOfBays> recPreBay = {std::string(recordPrefixBay0), std::string(recordPrefixBay1) };
    if (!recPreBay[0].compare(recPreBay[1]))
    {
        printf("  ERROR: record prefixes must be different. Just the first one will be used\n");
        recPreBay[1] = std::string("");
    }

    if ((appID < 0) | (appID > 1023))
    {
        printf("  ERROR: Invalid AppID!. It must be an unsigned 10-bit number\n");
        return asynError;
    }

    pL2MPS = new L2MPS(portName, appID, recPreMps, recPreBay, mpsRoot);

    return (status==0) ? asynSuccess : asynError;
}

static const iocshArg confArg0 =    { "portName",         iocshArgString};
static const iocshArg confArg1 =    { "AppID",            iocshArgInt};
static const iocshArg confArg2 =    { "recordPrefixMps",  iocshArgString};
static const iocshArg confArg3 =    { "recordPrefixBay0", iocshArgString};
static const iocshArg confArg4 =    { "recordPrefixBay1", iocshArgString};
static const iocshArg confArg5 =    { "mpsRootPath",      iocshArgString};


static const iocshArg * const confArgs[] = {
    &confArg0,
    &confArg1,
    &confArg2,
    &confArg3,
    &confArg4,
    &confArg5
};

static const iocshFuncDef configFuncDef = {"L2MPSASYNConfig",6,confArgs};

static void configCallFunc(const iocshArgBuf *args)
{
    L2MPSASYNConfig(args[0].sval, args[1].ival, args[2].sval, args[3].sval, args[4].sval, args[5].sval);
}
// - L2MPSASYNConfig //

void drvL2MPSASYNRegister(void)
{
    iocshRegister(&configFuncDef, configCallFunc);
}

extern "C" {
    epicsExportRegistrar(drvL2MPSASYNRegister);
}
