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

// MPS base info callback function
void L2MPS::updateMpsParametrs(mps_infoData_t info)
{
    setIntegerParam(paramListMpsBase, mpsInfoParams.appId,            info.appId            );
    setIntegerParam(paramListMpsBase, mpsInfoParams.version,          info.version          );
    setIntegerParam(paramListMpsBase, mpsInfoParams.byteCount,        info.byteCount        );
    setIntegerParam(paramListMpsBase, mpsInfoParams.beamDestMask,     info.beamDestMask     );
    setIntegerParam(paramListMpsBase, mpsInfoParams.altDestMask,      info.altDestMask      );
    setIntegerParam(paramListMpsBase, mpsInfoParams.msgCnt,           info.msgCnt           );
    setIntegerParam(paramListMpsBase, mpsInfoParams.lastMsgAppId,     info.lastMsgAppId     );
    setIntegerParam(paramListMpsBase, mpsInfoParams.lastMsgTimestamp, info.lastMsgTimestamp );
    setIntegerParam(paramListMpsBase, mpsInfoParams.txLinkUpCnt,      info.txLinkUpCnt      );
    setIntegerParam(paramListMpsBase, mpsInfoParams.rollOverEn,       info.rollOverEn       );
    setIntegerParam(paramListMpsBase, mpsInfoParams.txPktSentCnt,     info.txPktSentCnt     );

    setStringParam(paramListMpsBase, mpsInfoParams.appType, info.appType.c_str());

    setUIntDigitalParam(paramListMpsBase, mpsInfoParams.enable,      info.enable,      0x1, 0x1 );
    setUIntDigitalParam(paramListMpsBase, mpsInfoParams.lcls1Mode,   info.lcls1Mode,   0x1, 0x1 );
    setUIntDigitalParam(paramListMpsBase, mpsInfoParams.digitalEn,   info.digitalEn,   0x1, 0x1 );
    setUIntDigitalParam(paramListMpsBase, mpsInfoParams.lastMsgLcls, info.lastMsgLcls, 0x1, 0x1 );
    setUIntDigitalParam(paramListMpsBase, mpsInfoParams.txLinkUp,    info.txLinkUp,    0x1, 0x1 );
    setUIntDigitalParam(paramListMpsBase, mpsInfoParams.mpsSlot,     info.mpsSlot,     0x1, 0x1 );
    setUIntDigitalParam(paramListMpsBase, mpsInfoParams.pllLocked,   info.pllLocked,   0x1, 0x1 );

    if (info.lastMsgByte.size() > 0)
    {
        try
        {
            std::vector<int>::iterator    paramIt = mpsInfoParams.lastMsgByte.begin();
            std::vector<uint8_t>::iterator dataIt = info.lastMsgByte.begin();
            while ((paramIt != mpsInfoParams.lastMsgByte.end()) && (dataIt != info.lastMsgByte.end()))
                setIntegerParam(paramListMpsBase, *paramIt++, *dataIt++);
        }
        catch(std::out_of_range& e) {}
    }

    if (info.rxLinkUp.size() > 0)
    {
        try
        {
            std::vector<int>::iterator  paramIt = mpsInfoParams.rxLinkUp.begin();
            std::vector<bool>::iterator dataIt = info.rxLinkUp.begin();
            while ((paramIt != mpsInfoParams.rxLinkUp.end()) && (dataIt != info.rxLinkUp.end()))
                setUIntDigitalParam(paramListMpsBase, *paramIt++, *dataIt++, 0x1, 0x1);
        }
        catch(std::out_of_range& e) {}
    }

    if (info.rxLinkUpCnt.size() > 0)
    {
        try
        {
            std::vector<int>::iterator    paramIt = mpsInfoParams.rxLinkUpCnt.begin();
            std::vector<uint32_t>::iterator dataIt = info.rxLinkUpCnt.begin();
            while ((paramIt != mpsInfoParams.rxLinkUpCnt.end()) && (dataIt != info.rxLinkUpCnt.end()))
                setIntegerParam(paramListMpsBase, *paramIt++, *dataIt++);
        }
        catch(std::out_of_range& e) {}
    }

    if (info.rxPktRcvdCnt.size() > 0)
    {
        try
        {
            std::vector<int>::iterator    paramIt = mpsInfoParams.rxPktRcvdCnt.begin();
            std::vector<uint32_t>::iterator dataIt = info.rxPktRcvdCnt.begin();
            while ((paramIt != mpsInfoParams.rxPktRcvdCnt.end()) && (dataIt != info.rxPktRcvdCnt.end()))
                setIntegerParam(paramListMpsBase, *paramIt++, *dataIt++);
        }
        catch(std::out_of_range& e) {}
    }

    callParamCallbacks(paramListMpsBase);
}

void L2MPS::setMpsCallback(mps_infoData_t info)
{
    pL2MPS->updateMpsParametrs(info);
}

// App callback functions
template<typename T>
void L2MPS::updateAppParameters(int bay, T data)
{
    typename T::iterator dataIt;
    for (dataIt = data.begin(); dataIt != data.end(); ++dataIt)
    {
        // Process the Threshold info
        paramMap_t::iterator paramIt = paramMap.find(dataIt->first);

        if (paramIt != paramMap.end())
        {
            thr_chInfoData_t  infoData  = (dataIt->second).info;
            thr_chInfoParam_t infoParam = (paramIt->second).info;

            setIntegerParam(    bay,    infoParam.ch,           infoData.ch      );
            setIntegerParam(    bay,    infoParam.count,        infoData.count   );
            setIntegerParam(    bay,    infoParam.byteMap,      infoData.byteMap );
            setUIntDigitalParam(bay,    infoParam.idleEn,       infoData.idleEn,   0xFFFFFFFF, 0x1   );
            setUIntDigitalParam(bay,    infoParam.altEn,        infoData.altEn,    0xFFFFFFFF, 0x1   );
            setUIntDigitalParam(bay,    infoParam.lcls1En,      infoData.lcls1En,  0xFFFFFFFF, 0x1   );
            setDoubleParam(     bay,    infoParam.scaleFactor,  infoData.scaleFactor );

            thr_chData_t  data_thr  = (dataIt->second).data;
            thr_chParam_t param_thr = (paramIt->second).data;

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
                    setDoubleParam(bay, param_param.min, data_data.min);
                    setDoubleParam(bay, param_param.max, data_data.max);
                }
            }

            callParamCallbacks(bay);
        }
    }
}

template<typename T>
void L2MPS::setAppCallback(int bay, T data)
{
    pL2MPS->updateAppParameters<T>(bay, data);
}

L2MPS::L2MPS(const char *portName, const uint16_t appId, const std::string recordPrefixMps, const std::array<std::string, numberOfBays> recordPrefixBay,  std::string mpsRootPath)
    : asynPortDriver(
            portName,
            MAX_SIGNALS,
            NUM_PARAMS,
            asynInt32Mask | asynDrvUserMask | asynOctetMask | \
            asynUInt32DigitalMask | asynFloat64Mask,                                    // Interface Mask
            asynInt32Mask | asynUInt32DigitalMask | asynFloat64Mask | asynOctetMask,    // Interrupt Mask
            ASYN_MULTIDEVICE | ASYN_CANBLOCK,                                           // asynFlags
            1,                                                                          // Autoconnect
            0,                                                                          // Default priority
            0),                                                                         // Default stack size
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
        int index;

        // Integer variables
        createParam(2, "APP_ID",   asynParamInt32,  &index);
        mpsInfoParams.appId = index;

        createParam(2, "MPS_VER",   asynParamInt32,  &index);
        mpsInfoParams.version = index;

        createParam(2, "BYTE_COUNT",   asynParamInt32,  &index);
        mpsInfoParams.byteCount = index;

        createParam(2, "BEAM_DEST_MASK", asynParamInt32,  &index);
        mpsInfoParams.beamDestMask = index;

        createParam(2, "ALT_DEST_MASK",  asynParamInt32,  &index);
        mpsInfoParams.altDestMask = index;

        createParam(2, "MSG_CNT",   asynParamInt32,  &index);
        mpsInfoParams.msgCnt = index;

        createParam(2, "LAST_MSG_APPID",   asynParamInt32,  &index);
        mpsInfoParams.lastMsgAppId = index;

        createParam(2, "LAST_MSG_TMSTMP",   asynParamInt32,  &index);
        mpsInfoParams.lastMsgTimestamp = index;

        createParam(2, "TX_LINK_UP_CNT",   asynParamInt32,  &index);
        mpsInfoParams.txLinkUpCnt = index;

        createParam(2, "ROLL_OVER_EN",   asynParamInt32,  &index);
        mpsInfoParams.rollOverEn = index;

        createParam(2, "TX_PKT_SENT_CNT",   asynParamInt32,  &index);
        mpsInfoParams.txPktSentCnt = index;

        // String variables
        createParam(2, "APP_TYPE",   asynParamOctet,  &index);
        mpsInfoParams.appType = index;

        // Digital variables
        createParam(2, "MPS_EN",   asynParamUInt32Digital,  &index);
        mpsInfoParams.enable = index;

        createParam(2, "LCLS1_MODE",   asynParamUInt32Digital,  &index);
        mpsInfoParams.lcls1Mode = index;

        createParam(2, "DIGITAL_EN",   asynParamUInt32Digital,  &index);
        mpsInfoParams.digitalEn = index;

        createParam(2, "LAST_MSG_LCLS",   asynParamUInt32Digital,  &index);
        mpsInfoParams.lastMsgLcls = index;

        createParam(2, "TX_LINK_UP",   asynParamUInt32Digital,  &index);
        mpsInfoParams.txLinkUp = index;

        createParam(2, "MPS_SLOT",   asynParamUInt32Digital,  &index);
        mpsInfoParams.mpsSlot = index;

        createParam(2, "PLL_LOCKED",   asynParamUInt32Digital,  &index);
        mpsInfoParams.pllLocked = index;

        createParam(2, "SALT_RST_CNT",   asynParamUInt32Digital,  &index);
        mpsInfoParams.rstCnt = index;

        createParam(2, "SALT_RST_PLL",   asynParamUInt32Digital,  &index);
        mpsInfoParams.rstPll = index;


        // Vector of integer variables
        std::size_t lastMsgByteSize = node_->getLastMsgByteSize();
        if (lastMsgByteSize > 0)
        {
            std::stringstream paramName;
            for (std::size_t i {0}; i < lastMsgByteSize; ++i)
            {
                paramName.str("");
                paramName << "LAST_MSG_BYTE_" << i;
                createParam(2, paramName.str().c_str(),   asynParamInt32,  &index);
                mpsInfoParams.lastMsgByte.push_back(index);
            }
        }

        std::size_t rxLinkUpCntSize = node_->getRxLinkUpCntSize();
        if (rxLinkUpCntSize > 0)
        {
            std::stringstream paramName;
            for (std::size_t i {0}; i < rxLinkUpCntSize; ++i)
            {
                paramName.str("");
                paramName << "RX_LINK_UP_" << i;
                createParam(2, paramName.str().c_str(),   asynParamUInt32Digital,  &index);
                mpsInfoParams.rxLinkUp.push_back(index);

                paramName.str("");
                paramName << "RX_LINK_UP_CNT_" << i;
                createParam(2, paramName.str().c_str(),   asynParamInt32,  &index);
                mpsInfoParams.rxLinkUpCnt.push_back(index);
            }
        }

        std::size_t rxPktRcvdCntSize = node_->getRxPktRcvdCntSize();
        if (rxPktRcvdCntSize > 0)
        {
            std::stringstream paramName;
            for (std::size_t i {0}; i < rxPktRcvdCntSize; ++i)
            {
                paramName.str("");
                paramName << "RX_PKT_RCV_CNT_" << i;
                createParam(2, paramName.str().c_str(),   asynParamInt32,  &index);
                mpsInfoParams.rxPktRcvdCnt.push_back(index);
            }
        }

        std::string dbParams = "P=" + std::string(recordPrefixMps_) + ",PORT=" + std::string(portName_);
        dbLoadRecords("db/mps.db", dbParams.c_str());

        for(std::size_t i {0}; i < numberOfBays; ++i)
        {
            if (!recordPrefixBay_[i].empty())
            {
                if (!appType_.compare("BPM"))
                {
                    amc[i] = MpsBpmFactory::create(mpsRoot, i);
                    InitBpmMaps(i);
                }
                else if (!appType_.compare("BLEN"))
                {
                    amc[i] = MpsBlenFactory::create(mpsRoot, i);
                    InitBlenMaps(i);
                }
                else if (!appType_.compare("BCM"))
                {
                    amc[i] = MpsBcmFactory::create(mpsRoot, i);
                    InitBcmMaps(i);
                }
                else if ((!appType_.compare("BLM")) | (!appType_.compare("MPS_6CH")) | (!appType_.compare("MPS_24CH")))
                {
                    amc[i] = MpsBlmFactory::create(mpsRoot, i);
                    InitBlmMaps(i);
                }
            }
        }

        // Start polling threads
        node_->startPollThread(1, &setMpsCallback);

        for(std::size_t i {0}; i < numberOfBays; ++i)
        {
            if (!recordPrefixBay_[i].empty())
            {
                if (!appType_.compare("BPM"))
                {
                    boost::any_cast<MpsBpm>(amc[i])->startPollThread(1, &setAppCallback);
                }
                else if (!appType_.compare("BLEN"))
                {
                    boost::any_cast<MpsBlen>(amc[i])->startPollThread(1, &setAppCallback);
                }
                else if (!appType_.compare("BCM"))
                {
                    boost::any_cast<MpsBcm>(amc[i])->startPollThread(1, &setAppCallback);
                }
                else if ((!appType_.compare("BLM")) | (!appType_.compare("MPS_6CH")) | (!appType_.compare("MPS_24CH")))
                {
                    boost::any_cast<MpsBlm>(amc[i])->startPollThread(1, &setAppCallback);
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
    int index;
    std::stringstream pName;

    for (int i = 0; i < numBpmChs; ++i)
    {
            bpm_channel_t thisBpmCh = i;

            thr_param_t thrParam;

            pName.str("");
            pName << "_" << i;

            createParam(bay, ("BPM_THRNUM" + pName.str()).c_str(), asynParamInt32, &index);
            thrParam.info.ch = index;

            createParam(bay, ("BPM_THRCNT" + pName.str()).c_str(), asynParamInt32, &index);
            thrParam.info.count = index;

            createParam(bay, ("BPM_BYTEMAP" + pName.str()).c_str(), asynParamInt32, &index);
            thrParam.info.byteMap = index;

            createParam(bay, ("BPM_IDLEEN" + pName.str()).c_str(), asynParamUInt32Digital, &index);
            thrParam.info.idleEn = index;

            createParam(bay, ("BPM_ALTEN" + pName.str()).c_str(), asynParamUInt32Digital, &index);
            thrParam.info.altEn = index;

            createParam(bay, ("BPM_LCLS1EN" + pName.str()).c_str(), asynParamUInt32Digital, &index);
            thrParam.info.lcls1En = index;

            createParam(bay, ("BPM_SCALE" + pName.str()).c_str(), asynParamFloat64, &index);
            thrParam.info.scaleFactor = index;
            fMapBpmWScale.insert( std::make_pair( index, std::make_pair( &IMpsBpm::setScaleFactor, thisBpmCh ) ) );

            thr_chParam_t thrChParamMap;
            for (int k = 0; k < numThrTables; ++k)
            {
                for (int n = 0; n < numThrCounts[k]; ++n)
                {
                    thr_table_t thisThrTable = thr_table_t{{k,n }};
                    bpmThr_channel_t args = {thisBpmCh, thisThrTable};

                    thr_tableParam_t tp;

                    pName.str("");
                    pName << "_" << i << k << n;

                    createParam(bay, ("BPM_THRMIN" + pName.str()).c_str(), asynParamFloat64, &index);
                    tp.min = index;
                    fMapBpmW32.insert( std::make_pair( index, std::make_pair( &IMpsBpm::setThresholdMin,  args) ) );

                    createParam(bay, ("BPM_THRMAX" + pName.str()).c_str(), asynParamFloat64, &index);
                    tp.max = index;
                    fMapBpmW32.insert( std::make_pair( index, std::make_pair( &IMpsBpm::setThresholdMax, args) ) );

                    createParam(bay, ("BPM_THRMINEN" + pName.str()).c_str(), asynParamUInt32Digital, &index);
                    tp.minEn = index;
                    fMapBpmW1.insert( std::make_pair( index, std::make_pair( &IMpsBpm::setThresholdMinEn, args ) ) );

                    createParam(bay, ("BPM_THRMAXEN" + pName.str()).c_str(), asynParamUInt32Digital, &index);
                    tp.maxEn = index;
                    fMapBpmW1.insert( std::make_pair( index, std::make_pair( &IMpsBpm::setThresholdMaxEn, args ) ) );

                    thr_table_t    tt = thr_table_t{{k, n}};
                    thrChParamMap.insert(std::make_pair(tt, tp));
                }
            }
            thrParam.data = thrChParamMap;
            paramMap.insert(std::make_pair( thisBpmCh, thrParam ));
    }

    std::stringstream bpmDbParams;
    bpmDbParams.str("");
    bpmDbParams << "P=" << std::string(recordPrefixBay_[bay]);
    bpmDbParams << ",PORT=" << std::string(portName_);
    bpmDbParams << ",BAY=" << bay;
    dbLoadRecords("db/bpm.db", bpmDbParams.str().c_str());
}

void L2MPS::InitBlenMaps(const int bay)
{
    int index;
    std::stringstream pName;

    for (int i = 0; i < numBlenChs; ++i)
    {
            blen_channel_t thisBlenCh = i;

            thr_param_t thrParam;

            pName.str("");
            pName << "_" << i;

            createParam(bay, ("BLEN_THRNUM" + pName.str()).c_str(), asynParamInt32, &index);
            thrParam.info.ch = index;

            createParam(bay, ("BLEN_THRCNT" + pName.str()).c_str(), asynParamInt32, &index);
            thrParam.info.count = index;

            createParam(bay, ("BLEN_BYTEMAP" + pName.str()).c_str(), asynParamInt32, &index);
            thrParam.info.byteMap = index;

            createParam(bay, ("BLEN_IDLEEN" + pName.str()).c_str(), asynParamUInt32Digital, &index);
            thrParam.info.idleEn = index;

            createParam(bay, ("BLEN_ALTEN" + pName.str()).c_str(), asynParamUInt32Digital, &index);
            thrParam.info.altEn = index;

            createParam(bay, ("BLEN_LCLS1EN" + pName.str()).c_str(), asynParamUInt32Digital, &index);
            thrParam.info.lcls1En = index;

            createParam(bay, ("BLEN_SCALE" + pName.str()).c_str(), asynParamFloat64, &index);
            thrParam.info.scaleFactor = index;
            fMapBlenWScale.insert( std::make_pair( index, std::make_pair( &IMpsBlen::setScaleFactor, thisBlenCh ) ) );

            thr_chParam_t thrChParamMap;
            for (int k = 0; k < numThrTables; ++k)
            {
                for (int n = 0; n < numThrCounts[k]; ++n)
                {
                    thr_table_t thisThrTable = thr_table_t{{k,n }};
                    blenThr_channel_t args = {thisBlenCh, thisThrTable};

                    thr_tableParam_t tp;

                    pName.str("");
                    pName << "_" << i << k << n;

                    createParam(bay, ("BLEN_THRMIN" + pName.str()).c_str(), asynParamFloat64, &index);
                    tp.min = index;
                    fMapBlenW32.insert( std::make_pair( index, std::make_pair( &IMpsBlen::setThresholdMin,  args) ) );

                    createParam(bay, ("BLEN_THRMAX" + pName.str()).c_str(), asynParamFloat64, &index);
                    tp.max = index;
                    fMapBlenW32.insert( std::make_pair( index, std::make_pair( &IMpsBlen::setThresholdMax, args) ) );

                    createParam(bay, ("BLEN_THRMINEN" + pName.str()).c_str(), asynParamUInt32Digital, &index);
                    tp.minEn = index;
                    fMapBlenW1.insert( std::make_pair( index, std::make_pair( &IMpsBlen::setThresholdMinEn, args ) ) );

                    createParam(bay, ("BLEN_THRMAXEN" + pName.str()).c_str(), asynParamUInt32Digital, &index);
                    tp.maxEn = index;
                    fMapBlenW1.insert( std::make_pair( index, std::make_pair( &IMpsBlen::setThresholdMaxEn, args ) ) );

                    thr_table_t    tt = thr_table_t{{k, n}};
                    thrChParamMap.insert(std::make_pair(tt, tp));
                }
            }
            thrParam.data = thrChParamMap;
            paramMap.insert(std::make_pair( thisBlenCh, thrParam ));
    }

    std::stringstream blenDbParams;
    blenDbParams.str("");
    blenDbParams << "P=" << std::string(recordPrefixBay_[bay]);
    blenDbParams << ",PORT=" << std::string(portName_);
    blenDbParams << ",BAY=" << bay;
    dbLoadRecords("db/blen.db", blenDbParams.str().c_str());
}

void L2MPS::InitBcmMaps(const int bay)
{
    int index;
    std::stringstream pName;

    for (int i = 0; i < numBcmChs; ++i)
    {
            bcm_channel_t thisBcmCh = i;

            thr_param_t thrParam;

            pName.str("");
            pName << "_" << i;

            createParam(bay, ("BCM_THRNUM" + pName.str()).c_str(), asynParamInt32, &index);
            thrParam.info.ch = index;

            createParam(bay, ("BCM_THRCNT" + pName.str()).c_str(), asynParamInt32, &index);
            thrParam.info.count = index;

            createParam(bay, ("BCM_BYTEMAP" + pName.str()).c_str(), asynParamInt32, &index);
            thrParam.info.byteMap = index;

            createParam(bay, ("BCM_IDLEEN" + pName.str()).c_str(), asynParamUInt32Digital, &index);
            thrParam.info.idleEn = index;

            createParam(bay, ("BCM_ALTEN" + pName.str()).c_str(), asynParamUInt32Digital, &index);
            thrParam.info.altEn = index;

            createParam(bay, ("BCM_LCLS1EN" + pName.str()).c_str(), asynParamUInt32Digital, &index);
            thrParam.info.lcls1En = index;

            createParam(bay, ("BCM_SCALE" + pName.str()).c_str(), asynParamFloat64, &index);
            thrParam.info.scaleFactor = index;
            fMapBcmWScale.insert( std::make_pair( index, std::make_pair( &IMpsBcm::setScaleFactor, thisBcmCh ) ) );

            thr_chParam_t thrChParamMap;
            for (int k = 0; k < numThrTables; ++k)
            {
                for (int n = 0; n < numThrCounts[k]; ++n)
                {
                    thr_table_t thisThrTable = thr_table_t{{k,n }};
                    bcmThr_channel_t args = {thisBcmCh, thisThrTable};

                    thr_tableParam_t tp;

                    pName.str("");
                    pName << "_" << i << k << n;

                    createParam(bay, ("BCM_THRMIN" + pName.str()).c_str(), asynParamFloat64, &index);
                    tp.min = index;
                    fMapBcmW32.insert( std::make_pair( index, std::make_pair( &IMpsBcm::setThresholdMin,  args) ) );

                    createParam(bay, ("BCM_THRMAX" + pName.str()).c_str(), asynParamFloat64, &index);
                    tp.max = index;
                    fMapBcmW32.insert( std::make_pair( index, std::make_pair( &IMpsBcm::setThresholdMax, args) ) );

                    createParam(bay, ("BCM_THRMINEN" + pName.str()).c_str(), asynParamUInt32Digital, &index);
                    tp.minEn = index;
                    fMapBcmW1.insert( std::make_pair( index, std::make_pair( &IMpsBcm::setThresholdMinEn, args ) ) );

                    createParam(bay, ("BCM_THRMAXEN" + pName.str()).c_str(), asynParamUInt32Digital, &index);
                    tp.maxEn = index;
                    fMapBcmW1.insert( std::make_pair( index, std::make_pair( &IMpsBcm::setThresholdMaxEn, args ) ) );

                    thr_table_t    tt = thr_table_t{{k, n}};
                    thrChParamMap.insert(std::make_pair(tt, tp));
                }
            }
            thrParam.data = thrChParamMap;
            paramMap.insert(std::make_pair( thisBcmCh, thrParam ));
    }

    std::stringstream bcmDbParams;
    bcmDbParams.str("");
    bcmDbParams << "P=" << std::string(recordPrefixBay_[bay]);
    bcmDbParams << ",PORT=" << std::string(portName_);
    bcmDbParams << ",BAY=" << bay;
    dbLoadRecords("db/bcm.db", bcmDbParams.str().c_str());
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

            thr_param_t thrParam;

            pName.str("");
            pName << "_" << i << j;

            createParam(bay, ("BLM_THRNUM" + pName.str()).c_str(), asynParamInt32, &index);
            thrParam.info.ch = index;

            createParam(bay, ("BLM_THRCNT" + pName.str()).c_str(), asynParamInt32, &index);
            thrParam.info.count = index;

            createParam(bay, ("BLM_BYTEMAP" + pName.str()).c_str(), asynParamInt32, &index);
            thrParam.info.byteMap = index;

            createParam(bay, ("BLM_IDLEEN" + pName.str()).c_str(), asynParamUInt32Digital, &index);
            thrParam.info.idleEn = index;

            createParam(bay, ("BLM_ALTEN" + pName.str()).c_str(), asynParamUInt32Digital, &index);
            thrParam.info.altEn = index;

            createParam(bay, ("BLM_LCLS1EN" + pName.str()).c_str(), asynParamUInt32Digital, &index);
            thrParam.info.lcls1En = index;

            createParam(bay, ("BLM_SCALE" + pName.str()).c_str(), asynParamFloat64, &index);
            thrParam.info.scaleFactor = index;
            fMapBlmWScale.insert( std::make_pair( index, std::make_pair( &IMpsBlm::setScaleFactor, thisBlmCh ) ) );

            thr_chParam_t thrChParamMap;
            for (int k = 0; k < numThrTables; ++k)
            {
                    for (int n = 0; n < numThrCounts[k]; ++n)
                    {
                        thr_table_t thisThrTable = thr_table_t{{k,n }};
                        blmThr_channel_t args = {thisBlmCh, thisThrTable};

                        thr_tableParam_t tp;

                        pName.str("");
                        pName << "_" << i << j  << k << n;

                        createParam(bay, ("BLM_THRMIN" + pName.str()).c_str(), asynParamFloat64, &index);
                        tp.min = index;
                        fMapBlmW32.insert( std::make_pair( index, std::make_pair( &IMpsBlm::setThresholdMin,  args) ) );

                        createParam(bay, ("BLM_THRMAX" + pName.str()).c_str(), asynParamFloat64, &index);
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
            thrParam.data = thrChParamMap;
            paramMap.insert(std::make_pair( thisBlmCh, thrParam ));
        }
    }
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

    if (addr == paramListMpsBase)
    {
        try
        {
            // // MPS node parameters
            if (function == mpsInfoParams.beamDestMask)
            {
              node_->setBeamDestMask(value);
            }
            else if (function == mpsInfoParams.altDestMask)
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
    }
    else
    {
        status == asynPortDriver::writeInt32(pasynUser, value);
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

    // MPS node parameters
    if (addr == paramListMpsBase)
    {
        try
        {
            if(function == mpsInfoParams.enable)
            {
                node_->setEnable(value & mask);
            }
            else if (function == mpsInfoParams.lcls1Mode)
            {
                node_->setLcls1Mode(value & mask);
            }
            else if (function == mpsInfoParams.rstCnt)
            {
                node_->resetSaltCnt();
            }
            else if (function == mpsInfoParams.rstPll)
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
    }
    else if ((addr == paramListAppBay0) or (addr == paramListAppBay1))
    {
        try
        {
            bpm_fmap_w1_t::iterator bpm_it;
            blen_fmap_w1_t::iterator blen_it;
            bcm_fmap_w1_t::iterator bcm_it;
            blm_fmap_w1_t::iterator blm_it;

            // BPM parameters
            if ((bpm_it = fMapBpmW1.find(function)) != fMapBpmW1.end())
            {
              ((*boost::any_cast<MpsBpm>(amc[addr])).*(bpm_it->second.first))(bpm_it->second.second, (value & mask));
            }
            // BLEN parameters
            else if ((blen_it = fMapBlenW1.find(function)) != fMapBlenW1.end())
            {
              ((*boost::any_cast<MpsBlen>(amc[addr])).*(blen_it->second.first))(blen_it->second.second, (value & mask));
            }
            // BCM parameters
            else if ((bcm_it = fMapBcmW1.find(function)) != fMapBcmW1.end())
            {
              ((*boost::any_cast<MpsBcm>(amc[addr])).*(bcm_it->second.first))(bcm_it->second.second, (value & mask));
            }
            // BLM parameters
            else if ((blm_it = fMapBlmW1.find(function)) != fMapBlmW1.end())
            {
              ((*boost::any_cast<MpsBlm>(amc[addr])).*(blm_it->second.first))(blm_it->second.second, (value & mask));
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
    }
    else
    {
        status = asynPortDriver::writeUInt32Digital(pasynUser, value, mask);
    }

    return (status == 0) ? asynSuccess : asynError;
}

asynStatus L2MPS::writeFloat64 (asynUser *pasynUser, epicsFloat64 value)
{
    int addr;
    int function = pasynUser->reason;
    int status=0;
    const char *name;

    this->getAddress(pasynUser, &addr);

    static const char *functionName = "writeFloat64";

    this->getAddress(pasynUser, &addr);

    getParamName(addr, function, &name);

    if ((addr == paramListAppBay0) or (addr == paramListAppBay1))
    {
        try
        {
            bpm_fmap_w32_t::iterator bpm_it;
            bpm_scaleFuncMap_t::iterator bpm_scaleIt;

            blen_fmap_w32_t::iterator blen_it;
            blen_scaleFuncMap_t::iterator blen_scaleIt;

            bcm_fmap_w32_t::iterator bcm_it;
            bcm_scaleFuncMap_t::iterator bcm_scaleIt;

            blm_fmap_w32_t::iterator blm_it;
            blm_scaleFuncMap_t::iterator blm_scaleIt;

            // BPM parameters
            if ((bpm_it = fMapBpmW32.find(function)) != fMapBpmW32.end())
            {
                ((*boost::any_cast<MpsBpm>(amc[addr])).*(bpm_it->second.first))(bpm_it->second.second, value);
            }
            else if ((bpm_scaleIt = fMapBpmWScale.find(function)) != fMapBpmWScale.end())
            {
                ((*boost::any_cast<MpsBpm>(amc[addr])).*(bpm_scaleIt->second.first))(bpm_scaleIt->second.second, value);
            }
            // BLEN parameters
            else if ((blen_it = fMapBlenW32.find(function)) != fMapBlenW32.end())
            {
                ((*boost::any_cast<MpsBlen>(amc[addr])).*(blen_it->second.first))(blen_it->second.second, value);
            }
            else if ((blen_scaleIt = fMapBlenWScale.find(function)) != fMapBlenWScale.end())
            {
                ((*boost::any_cast<MpsBlen>(amc[addr])).*(blen_scaleIt->second.first))(blen_scaleIt->second.second, value);
            }
            // BCM parameters
            else if ((bcm_it = fMapBcmW32.find(function)) != fMapBcmW32.end())
            {
                ((*boost::any_cast<MpsBcm>(amc[addr])).*(bcm_it->second.first))(bcm_it->second.second, value);
            }
            else if ((bcm_scaleIt = fMapBcmWScale.find(function)) != fMapBcmWScale.end())
            {
                ((*boost::any_cast<MpsBcm>(amc[addr])).*(bcm_scaleIt->second.first))(bcm_scaleIt->second.second, value);
            }
            // BLM parameters
            else if ((blm_it = fMapBlmW32.find(function)) != fMapBlmW32.end())
            {
                ((*boost::any_cast<MpsBlm>(amc[addr])).*(blm_it->second.first))(blm_it->second.second, value);
            }
            else if ((blm_scaleIt = fMapBlmWScale.find(function)) != fMapBlmWScale.end())
            {
                ((*boost::any_cast<MpsBlm>(amc[addr])).*(blm_scaleIt->second.first))(blm_scaleIt->second.second, value);
            }
            else
            {
                status = asynPortDriver::writeFloat64(pasynUser, value);
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
    }
    else
    {
        status = asynPortDriver::writeFloat64(pasynUser, value);
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
