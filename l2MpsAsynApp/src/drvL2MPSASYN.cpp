/**
 *-----------------------------------------------------------------------------
 * Title      : LCLS-II MPS EPICS Module
 * ----------------------------------------------------------------------------
 * File       : drvL2MPSASYN.cpp
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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <getopt.h>
#include <sstream>
#include <boost/array.hpp>
#include <iostream>
#include <fstream>
#include <functional>
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

// Update single parameter status and severity
void L2MPS::updateAlarmParam(int list, int index, bool valid)
{
    int status   = 0;
    int severity = 0;

    if (!valid)
    {
        status   = 1;
        severity = 3;
    }
    setParamAlarmStatus(   list, index, status   );
    setParamAlarmSeverity( list, index, severity );
}

// Update single parameter value
template<typename T>
void L2MPS::updateIntegerParam(int list, int index, std::pair<bool, T> p)
{
    updateAlarmParam( list, index, p.first );
    setIntegerParam( list, index, static_cast<int>(p.second) );
}

template<typename T>
void L2MPS::updateStringParam(int list, int index, std::pair<bool, T> p)
{
    updateAlarmParam( list, index, p.first );
    setStringParam( list, index, p.second.c_str() );
}

template<typename T>
void L2MPS::updateUIntDigitalParam(int list, int index, std::pair<bool, T> p)
{
    updateAlarmParam( list, index, p.first );
    setUIntDigitalParam( list, index, static_cast<int>(p.second), 0x1, 0x1 );
}

template<typename T>
void L2MPS::updateDoubleParam(int list, int index, std::pair<bool, T> p)
{
    updateAlarmParam( list, index, p.first );
    setDoubleParam( list, index, p.second );
}

// Update an array of parameter values, status and severity
template<typename T>
void L2MPS::updateParamArray(int type, int list, const std::vector<int>& index, const std::pair< bool, std::vector<T> > p)
{
    if (p.second.size() > 0)
    {
        try
        {
            std::vector<int>::const_iterator        paramIt = index.begin();
            typename std::vector<T>::const_iterator dataIt  = p.second.begin();
            while ( ( paramIt != index.end() ) && ( dataIt != p.second.end() ) )
            {
                switch (type)
                {
                    case PARAM_TYPE_INT:
                        updateIntegerParam( list, *paramIt++, std::make_pair( p.first, *dataIt++ ) );
                        break;
                    case PARAM_TYPE_DIG:
                        updateUIntDigitalParam( list, *paramIt++, std::make_pair( p.first, *dataIt++ ) );
                        break;
                    default:
                        return;
                }
            }
        }
        catch(std::out_of_range& e) {}
    }
}

// MPS base info callback function
void L2MPS::updateMpsParametrs(mps_infoData_t info)
{
    updateIntegerParam( paramListMpsBase, mpsInfoParams.appId,            info.appId            );
    updateIntegerParam( paramListMpsBase, mpsInfoParams.version,          info.version          );
    updateIntegerParam( paramListMpsBase, mpsInfoParams.byteCount,        info.byteCount        );
    updateIntegerParam( paramListMpsBase, mpsInfoParams.beamDestMask,     info.beamDestMask     );
    updateIntegerParam( paramListMpsBase, mpsInfoParams.altDestMask,      info.altDestMask      );
    updateIntegerParam( paramListMpsBase, mpsInfoParams.msgCnt,           info.msgCnt           );
    updateIntegerParam( paramListMpsBase, mpsInfoParams.lastMsgAppId,     info.lastMsgAppId     );
    updateIntegerParam( paramListMpsBase, mpsInfoParams.lastMsgTimestamp, info.lastMsgTimestamp );
    updateIntegerParam( paramListMpsBase, mpsInfoParams.txLinkUpCnt,      info.txLinkUpCnt      );
    updateIntegerParam( paramListMpsBase, mpsInfoParams.rollOverEn,       info.rollOverEn       );
    updateIntegerParam( paramListMpsBase, mpsInfoParams.txPktSentCnt,     info.txPktSentCnt     );
    updateIntegerParam( paramListMpsBase, mpsInfoParams.txPktPeriod,      info.txPktPeriod      );
    updateIntegerParam( paramListMpsBase, mpsInfoParams.txPktPeriodMin,   info.txPktPeriodMin   );
    updateIntegerParam( paramListMpsBase, mpsInfoParams.txPktPeriodMax,   info.txPktPeriodMax   );
    updateIntegerParam( paramListMpsBase, mpsInfoParams.diagStrbCnt,      info.diagStrbCnt      );
    updateIntegerParam( paramListMpsBase, mpsInfoParams.pllLockCnt,       info.pllLockCnt       );
    updateIntegerParam( paramListMpsBase, mpsInfoParams.txEofeSentCnt,    info.txEofeSentCnt    );

    updateStringParam( paramListMpsBase, mpsInfoParams.appType,           info.appType );

    updateUIntDigitalParam( paramListMpsBase, mpsInfoParams.enable,      info.enable      );
    updateUIntDigitalParam( paramListMpsBase, mpsInfoParams.lcls1Mode,   info.lcls1Mode   );
    updateUIntDigitalParam( paramListMpsBase, mpsInfoParams.digitalEn,   info.digitalEn   );
    updateUIntDigitalParam( paramListMpsBase, mpsInfoParams.lastMsgLcls, info.lastMsgLcls );
    updateUIntDigitalParam( paramListMpsBase, mpsInfoParams.txLinkUp,    info.txLinkUp    );
    updateUIntDigitalParam( paramListMpsBase, mpsInfoParams.mpsSlot,     info.mpsSlot     );
    updateUIntDigitalParam( paramListMpsBase, mpsInfoParams.pllLocked,   info.pllLocked   );

    updateParamArray( PARAM_TYPE_INT, paramListMpsBase, mpsInfoParams.lastMsgByte,    info.lastMsgByte    );
    updateParamArray( PARAM_TYPE_DIG, paramListMpsBase, mpsInfoParams.rxLinkUp,       info.rxLinkUp       );
    updateParamArray( PARAM_TYPE_INT, paramListMpsBase, mpsInfoParams.rxLinkUpCnt,    info.rxLinkUpCnt    );
    updateParamArray( PARAM_TYPE_INT, paramListMpsBase, mpsInfoParams.rxPktRcvdCnt,   info.rxPktRcvdCnt   );
    updateParamArray( PARAM_TYPE_INT, paramListMpsBase, mpsInfoParams.rxPktPeriod,    info.rxPktPeriod    );
    updateParamArray( PARAM_TYPE_INT, paramListMpsBase, mpsInfoParams.rxPktPeriodMin, info.rxPktPeriodMin );
    updateParamArray( PARAM_TYPE_INT, paramListMpsBase, mpsInfoParams.rxPktPeriodMax, info.rxPktPeriodMax );
    updateParamArray( PARAM_TYPE_INT, paramListMpsBase, mpsInfoParams.rxErrDetCnt,    info.rxErrDetCnt    );

    callParamCallbacks(paramListMpsBase);
}

// Application callback functions
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

            setIntegerParam(        bay,    infoParam.ch,           infoData.ch          );
            updateIntegerParam(     bay,    infoParam.count,        infoData.count       );
            updateIntegerParam(     bay,    infoParam.byteMap,      infoData.byteMap     );
            updateUIntDigitalParam( bay,    infoParam.idleEn,       infoData.idleEn      );
            updateUIntDigitalParam( bay,    infoParam.altEn,        infoData.altEn       );
            updateUIntDigitalParam( bay,    infoParam.lcls1En,      infoData.lcls1En     );
            setDoubleParam(         bay,    infoParam.scaleSlope,   infoData.scaleSlope  );
            setDoubleParam(         bay,    infoParam.scaleOffset,  infoData.scaleOffset );

            thr_chData_t  data_thr  = (dataIt->second).data;
            thr_chParam_t param_thr = (paramIt->second).data;

            for (thr_chData_t::iterator data_thrIt = data_thr.begin(); data_thrIt != data_thr.end(); ++data_thrIt)
            {
                thr_table_t     data_thrCh = data_thrIt->first;
                thr_tableData_t data_data  = data_thrIt->second;

                thr_chParam_t::iterator param_thrIt = param_thr.find(data_thrCh);

                if (param_thrIt != param_thr.end())
                {
                    thr_tableParam_t    param_param = param_thrIt->second;

                    updateUIntDigitalParam( bay, param_param.minEn, data_data.minEn );
                    updateUIntDigitalParam( bay, param_param.maxEn, data_data.maxEn );
                    updateDoubleParam(      bay, param_param.min, data_data.min     );
                    updateDoubleParam(      bay, param_param.max, data_data.max     );
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
            asynInt32Mask | asynDrvUserMask | asynOctetMask | \
            asynUInt32DigitalMask | asynFloat64Mask,                                    // Interface Mask
            asynInt32Mask | asynUInt32DigitalMask | asynFloat64Mask | asynOctetMask,    // Interrupt Mask
            ASYN_MULTIDEVICE | ASYN_CANBLOCK,                                           // asynFlags
            1,                                                                          // Autoconnect
            0,                                                                          // Default priority
            0),                                                                         // Default stack size
        driverName_(DRIVER_NAME),
        portName_(portName),
        recordPrefixMps_(recordPrefixMps),
        recordPrefixBay_(recordPrefixBay)
{
    Path root = cpswGetRoot();

    try
    {
        if (mpsRootPath.empty())
        {
            mpsRootPath = defaultMpsRootPath;
            printf("Not MPS root was defined. Using default path: %s\n", mpsRootPath.c_str());
        }
        else
        {
            printf("Using MPS root path: %s\n", mpsRootPath.c_str());
        }

        Path mpsRoot = root->findByName(mpsRootPath.c_str());

        node_ = IMpsNode::create(mpsRoot);
        node_->setAppId(appId);
        node_->setEnable(false);
        std::string appType_ = node_->getAppType().second;

        // Set the MPS manager Application ID. It will be used
        // to identify this application.
        setMpsManagerAppId(appId);

        // Create parameters for the MPS node
        int index;

        // Integer variables
        createParam(2, "APP_ID", asynParamInt32, &index);
        mpsInfoParams.appId = index;

        createParam(2, "MPS_VER", asynParamInt32, &index);
        mpsInfoParams.version = index;

        createParam(2, "BYTE_COUNT", asynParamInt32, &index);
        mpsInfoParams.byteCount = index;

        createParam(2, "BEAM_DEST_MASK", asynParamInt32, &index);
        mpsInfoParams.beamDestMask = index;

        createParam(2, "ALT_DEST_MASK", asynParamInt32, &index);
        mpsInfoParams.altDestMask = index;

        createParam(2, "MSG_CNT", asynParamInt32, &index);
        mpsInfoParams.msgCnt = index;

        createParam(2, "LAST_MSG_APPID", asynParamInt32, &index);
        mpsInfoParams.lastMsgAppId = index;

        createParam(2, "LAST_MSG_TMSTMP", asynParamInt32, &index);
        mpsInfoParams.lastMsgTimestamp = index;

        createParam(2, "TX_LINK_UP_CNT", asynParamInt32, &index);
        mpsInfoParams.txLinkUpCnt = index;

        createParam(2, "ROLL_OVER_EN", asynParamInt32, &index);
        mpsInfoParams.rollOverEn = index;

        createParam(2, "TX_PKT_SENT_CNT", asynParamInt32, &index);
        mpsInfoParams.txPktSentCnt = index;

        createParam(2, "TX_PKT_PERIOD", asynParamInt32, &index);
        mpsInfoParams.txPktPeriod = index;

        createParam(2, "TX_PKT_PERIOD_MIN", asynParamInt32, &index);
        mpsInfoParams.txPktPeriodMin = index;

        createParam(2, "TX_PKT_PERIOD_MAX", asynParamInt32, &index);
        mpsInfoParams.txPktPeriodMax = index;

        createParam(2, "DIAG_STRB_CNT", asynParamInt32, &index);
        mpsInfoParams.diagStrbCnt = index;

        createParam(2, "PLL_LOCK_CNT", asynParamInt32, &index);
        mpsInfoParams.pllLockCnt = index;

        createParam(2, "TX_EOFE_SENT_CNT", asynParamInt32, &index);
        mpsInfoParams.txEofeSentCnt = index;

        // String variables
        createParam(2, "APP_TYPE", asynParamOctet, &index);
        mpsInfoParams.appType = index;

        // Digital variables
        createParam(2, "MPS_EN", asynParamUInt32Digital, &index);
        mpsInfoParams.enable = index;

        createParam(2, "LCLS1_MODE", asynParamUInt32Digital, &index);
        mpsInfoParams.lcls1Mode = index;

        createParam(2, "DIGITAL_EN", asynParamUInt32Digital, &index);
        mpsInfoParams.digitalEn = index;

        createParam(2, "LAST_MSG_LCLS", asynParamUInt32Digital, &index);
        mpsInfoParams.lastMsgLcls = index;

        createParam(2, "TX_LINK_UP", asynParamUInt32Digital, &index);
        mpsInfoParams.txLinkUp = index;

        createParam(2, "MPS_SLOT", asynParamUInt32Digital, &index);
        mpsInfoParams.mpsSlot = index;

        createParam(2, "PLL_LOCKED", asynParamUInt32Digital, &index);
        mpsInfoParams.pllLocked = index;

        createParam(2, "SALT_RST_CNT", asynParamUInt32Digital, &index);
        mpsInfoParams.rstCnt = index;

        createParam(2, "SALT_RST_PLL", asynParamUInt32Digital, &index);
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
                createParam(2, paramName.str().c_str(), asynParamInt32, &index);
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
                createParam(2, paramName.str().c_str(), asynParamUInt32Digital, &index);
                mpsInfoParams.rxLinkUp.push_back(index);

                paramName.str("");
                paramName << "RX_LINK_UP_CNT_" << i;
                createParam(2, paramName.str().c_str(), asynParamInt32, &index);
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
                createParam(2, paramName.str().c_str(), asynParamInt32, &index);
                mpsInfoParams.rxPktRcvdCnt.push_back(index);
            }
        }

        std::size_t rxPktPeriodSize = node_->getRxPktPeriodSize();
        if (rxPktPeriodSize > 0)
        {
            std::stringstream paramName;
            for (std::size_t i {0}; i < rxPktPeriodSize; ++i)
            {
                paramName.str("");
                paramName << "RX_PKT_PERIOD_" << i;
                createParam(2, paramName.str().c_str(), asynParamInt32, &index);
                mpsInfoParams.rxPktPeriod.push_back(index);
            }
        }

        std::size_t rxPktPeriodMinSize = node_->getRxPktPeriodMinSize();
        if (rxPktPeriodMinSize > 0)
        {
            std::stringstream paramName;
            for (std::size_t i {0}; i < rxPktPeriodMinSize; ++i)
            {
                paramName.str("");
                paramName << "RX_PKT_PERIOD_MIN_" << i;
                createParam(2, paramName.str().c_str(), asynParamInt32, &index);
                mpsInfoParams.rxPktPeriodMin.push_back(index);
            }
        }

        std::size_t rxPktPeriodMaxSize = node_->getRxPktPeriodMaxSize();
        if (rxPktPeriodMaxSize > 0)
        {
            std::stringstream paramName;
            for (std::size_t i {0}; i < rxPktPeriodMaxSize; ++i)
            {
                paramName.str("");
                paramName << "RX_PKT_PERIOD_MAX_" << i;
                createParam(2, paramName.str().c_str(), asynParamInt32, &index);
                mpsInfoParams.rxPktPeriodMax.push_back(index);
            }
        }

        std::size_t rxErrDetCntSize = node_->getRxErrDetCntSize();
        if (rxErrDetCntSize > 0)
        {
            std::stringstream paramName;
            for (std::size_t i {0}; i < rxErrDetCntSize; ++i)
            {
                paramName.str("");
                paramName << "RX_ERR_DET_CNT_" << i;
                createParam(2, paramName.str().c_str(), asynParamInt32, &index);
                mpsInfoParams.rxErrDetCnt.push_back(index);
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
                    amc[i] = IMpsBpm::create(mpsRoot, i);
                    InitBpmMaps(i);
                }
                else if (!appType_.compare("BLEN"))
                {
                    amc[i] = IMpsBlen::create(mpsRoot, i);
                    InitBlenMaps(i);
                }
                else if (!appType_.compare("BCM"))
                {
                    amc[i] = IMpsBcm::create(mpsRoot, i);
                    InitBcmMaps(i);
                }
                else if ((!appType_.compare("BLM")) | (!appType_.compare("MPS_6CH")) | (!appType_.compare("MPS_24CH")))
                {
                    amc[i] = IMpsBlm::create(mpsRoot, i);
                    InitBlmMaps(i);
                }
                else
                {
                    printf("ERROR: Application type %s not supported on bay %zu\n", appType_.c_str(), i);
                }
            }
        }

        // Start polling threads
        auto fp = std::bind(&L2MPS::updateMpsParametrs, this, std::placeholders::_1);
        node_->startPollThread(1, fp);

        for(std::size_t i {0}; i < numberOfBays; ++i)
        {
            if (!recordPrefixBay_[i].empty())
            {
                if (!appType_.compare("BPM"))
                {
                    auto fpa = std::bind(&L2MPS::updateAppParameters<std::map<bpm_channel_t, thr_ch_t>>, this, std::placeholders::_1, std::placeholders::_2);
                    boost::any_cast<MpsBpm>(amc[i])->startPollThread(1, fpa);
                }
                else if (!appType_.compare("BLEN"))
                {
                    auto fpa = std::bind(&L2MPS::updateAppParameters<std::map<blen_channel_t, thr_ch_t>>, this, std::placeholders::_1, std::placeholders::_2);
                    boost::any_cast<MpsBlen>(amc[i])->startPollThread(1, fpa);
                }
                else if (!appType_.compare("BCM"))
                {
                    auto fpa = std::bind(&L2MPS::updateAppParameters<std::map<bcm_channel_t, thr_ch_t>>, this, std::placeholders::_1, std::placeholders::_2);
                    boost::any_cast<MpsBcm>(amc[i])->startPollThread(1, fpa);
                }
                else if ((!appType_.compare("BLM")) | (!appType_.compare("MPS_6CH")) | (!appType_.compare("MPS_24CH")))
                {
                    auto fpa = std::bind(&L2MPS::updateAppParameters<std::map<blm_channel_t, thr_ch_t>>, this, std::placeholders::_1, std::placeholders::_2);
                    boost::any_cast<MpsBlm>(amc[i])->startPollThread(1, fpa);
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
            fMapBpmSetIdleEn.insert( std::make_pair( index, std::make_pair( &IMpsBpm::setIdleEn, thisBpmCh ) ) );

            createParam(bay, ("BPM_ALTEN" + pName.str()).c_str(), asynParamUInt32Digital, &index);
            thrParam.info.altEn = index;

            createParam(bay, ("BPM_LCLS1EN" + pName.str()).c_str(), asynParamUInt32Digital, &index);
            thrParam.info.lcls1En = index;

            createParam(bay, ("BPM_SCALESLOPE" + pName.str()).c_str(), asynParamFloat64, &index);
            thrParam.info.scaleSlope = index;
            fMapBpmWScaleSlope.insert( std::make_pair( index, std::make_pair( &IMpsBpm::setScaleSlope, thisBpmCh ) ) );

            createParam(bay, ("BPM_SCALEOFFSET" + pName.str()).c_str(), asynParamFloat64, &index);
            thrParam.info.scaleOffset = index;
            fMapBpmWScaleOffset.insert( std::make_pair( index, std::make_pair( &IMpsBpm::setScaleOffset, thisBpmCh ) ) );

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
    dbLoadRecords("db/mps_bpm.db", bpmDbParams.str().c_str());
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
            fMapBlenSetIdleEn.insert( std::make_pair( index, std::make_pair( &IMpsBlen::setIdleEn, thisBlenCh ) ) );

            createParam(bay, ("BLEN_ALTEN" + pName.str()).c_str(), asynParamUInt32Digital, &index);
            thrParam.info.altEn = index;

            createParam(bay, ("BLEN_LCLS1EN" + pName.str()).c_str(), asynParamUInt32Digital, &index);
            thrParam.info.lcls1En = index;

            createParam(bay, ("BLEN_SCALESLOPE" + pName.str()).c_str(), asynParamFloat64, &index);
            thrParam.info.scaleSlope = index;
            fMapBlenWScaleSlope.insert( std::make_pair( index, std::make_pair( &IMpsBlen::setScaleSlope, thisBlenCh ) ) );

            createParam(bay, ("BLEN_SCALEOFFSET" + pName.str()).c_str(), asynParamFloat64, &index);
            thrParam.info.scaleOffset = index;
            fMapBlenWScaleOffset.insert( std::make_pair( index, std::make_pair( &IMpsBlen::setScaleOffset, thisBlenCh ) ) );

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
    dbLoadRecords("db/mps_blen.db", blenDbParams.str().c_str());
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
            fMapBcmSetIdleEn.insert( std::make_pair( index, std::make_pair( &IMpsBcm::setIdleEn, thisBcmCh ) ) );

            createParam(bay, ("BCM_ALTEN" + pName.str()).c_str(), asynParamUInt32Digital, &index);
            thrParam.info.altEn = index;

            createParam(bay, ("BCM_LCLS1EN" + pName.str()).c_str(), asynParamUInt32Digital, &index);
            thrParam.info.lcls1En = index;

            createParam(bay, ("BCM_SCALESLOPE" + pName.str()).c_str(), asynParamFloat64, &index);
            thrParam.info.scaleSlope = index;
            fMapBcmWScaleSlope.insert( std::make_pair( index, std::make_pair( &IMpsBcm::setScaleSlope, thisBcmCh ) ) );

            createParam(bay, ("BCM_SCALEOFFSET" + pName.str()).c_str(), asynParamFloat64, &index);
            thrParam.info.scaleOffset = index;
            fMapBcmWScaleOffset.insert( std::make_pair( index, std::make_pair( &IMpsBcm::setScaleOffset, thisBcmCh ) ) );

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
    dbLoadRecords("db/mps_bcm.db", bcmDbParams.str().c_str());
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
            fMapBlmSetIdleEn.insert( std::make_pair( index, std::make_pair( &IMpsBlm::setIdleEn, thisBlmCh ) ) );

            createParam(bay, ("BLM_ALTEN" + pName.str()).c_str(), asynParamUInt32Digital, &index);
            thrParam.info.altEn = index;

            createParam(bay, ("BLM_LCLS1EN" + pName.str()).c_str(), asynParamUInt32Digital, &index);
            thrParam.info.lcls1En = index;

            createParam(bay, ("BLM_SCALESLOPE" + pName.str()).c_str(), asynParamFloat64, &index);
            thrParam.info.scaleSlope = index;
            fMapBlmWScaleSlope.insert( std::make_pair( index, std::make_pair( &IMpsBlm::setScaleSlope, thisBlmCh ) ) );

            createParam(bay, ("BLM_SCALEOFFSET" + pName.str()).c_str(), asynParamFloat64, &index);
            thrParam.info.scaleOffset = index;
            fMapBlmWScaleOffset.insert( std::make_pair( index, std::make_pair( &IMpsBlm::setScaleOffset, thisBlmCh ) ) );

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
    bool ret = false;  // return status from l2Mps API ( false on error)
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
                ret = node_->setBeamDestMask(value);
            }
            else if (function == mpsInfoParams.altDestMask)
            {
                ret = node_->setAltDestMask(value);
            }
            else if (function == mpsInfoParams.version)
            {
                  ret = node_->setVersion(value);
            }
            else
            {
                if ( 0 == asynPortDriver::writeInt32(pasynUser, value) )
                    ret = true;
            }
        }
        catch (CPSWError &e)
        {
            asynPrint(pasynUser, ASYN_TRACE_ERROR, "CPSW Error on %s writing parameter %s: %s\n", functionName, name, e.getInfo().c_str());
        }
        catch (std::runtime_error &e)
        {
            asynPrint(pasynUser, ASYN_TRACE_ERROR, "Runtime error on %s writing parameter %s: %s\n", functionName, name, e.what());
        }
    }
    else
    {
        if ( 0 == asynPortDriver::writeInt32(pasynUser, value) )
            ret = true;
    }

    return ( ret ) ? asynSuccess : asynError;
}

asynStatus L2MPS::writeUInt32Digital(asynUser *pasynUser, epicsUInt32 value, epicsUInt32 mask)
{
    int addr;
    int function = pasynUser->reason;
    bool ret = false;  // return status from l2Mps API ( false on error)
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
                ret = node_->setEnable(value & mask);
            }
            else if (function == mpsInfoParams.lcls1Mode)
            {
                ret = node_->setLcls1Mode(value & mask);
            }
            else if (function == mpsInfoParams.rstCnt)
            {
                ret = node_->resetSaltCnt();
            }
            else if (function == mpsInfoParams.rstPll)
            {
                ret = node_->resetSaltPll();
            }
            else
            {
                if ( 0 == asynPortDriver::writeUInt32Digital(pasynUser, value, mask) )
                    ret = true;
            }
        }
        catch (CPSWError &e)
        {
            asynPrint(pasynUser, ASYN_TRACE_ERROR, "CPSW Error on %s writing parameter %s: %s\n", functionName, name, e.getInfo().c_str());
        }
        catch (std::runtime_error &e)
        {
            asynPrint(pasynUser, ASYN_TRACE_ERROR, "Runtime error on %s writing parameter %s: %s\n", functionName, name, e.what());
        }
    }
    else if ((addr == paramListAppBay0) or (addr == paramListAppBay1))
    {
        try
        {
            bpm_fmap_w1_t::iterator bpm_it;
            bpm_setIdleEnMap_t::iterator bpm_idleIt;
            blen_fmap_w1_t::iterator blen_it;
            blen_setIdleEnMap_t::iterator blen_idleIt;
            bcm_fmap_w1_t::iterator bcm_it;
            bcm_setIdleEnMap_t::iterator bcm_idleIt;
            blm_fmap_w1_t::iterator blm_it;
            blm_setIdleEnMap_t::iterator blm_idleIt;

            // BPM parameters
            if ((bpm_it = fMapBpmW1.find(function)) != fMapBpmW1.end())
            {
                ret = ((*boost::any_cast<MpsBpm>(amc[addr])).*(bpm_it->second.first))(bpm_it->second.second, (value & mask));
            }
            else if ((bpm_idleIt = fMapBpmSetIdleEn.find(function)) != fMapBpmSetIdleEn.end())
            {
                ret = ((*boost::any_cast<MpsBpm>(amc[addr])).*(bpm_idleIt->second.first))(bpm_idleIt->second.second, value);
            }
            // BLEN parameters
            else if ((blen_it = fMapBlenW1.find(function)) != fMapBlenW1.end())
            {
                ret = ((*boost::any_cast<MpsBlen>(amc[addr])).*(blen_it->second.first))(blen_it->second.second, (value & mask));
            }
            else if ((blen_idleIt = fMapBlenSetIdleEn.find(function)) != fMapBlenSetIdleEn.end())
            {
                ret = ((*boost::any_cast<MpsBlen>(amc[addr])).*(blen_idleIt->second.first))(blen_idleIt->second.second, value);
            }
            // BCM parameters
            else if ((bcm_it = fMapBcmW1.find(function)) != fMapBcmW1.end())
            {
                ret = ((*boost::any_cast<MpsBcm>(amc[addr])).*(bcm_it->second.first))(bcm_it->second.second, (value & mask));
            }
            else if ((bcm_idleIt = fMapBcmSetIdleEn.find(function)) != fMapBcmSetIdleEn.end())
            {
                ret = ((*boost::any_cast<MpsBcm>(amc[addr])).*(bcm_idleIt->second.first))(bcm_idleIt->second.second, value);
            }
            // BLM parameters
            else if ((blm_it = fMapBlmW1.find(function)) != fMapBlmW1.end())
            {
                ret = ((*boost::any_cast<MpsBlm>(amc[addr])).*(blm_it->second.first))(blm_it->second.second, (value & mask));
            }
            else if ((blm_idleIt = fMapBlmSetIdleEn.find(function)) != fMapBlmSetIdleEn.end())
            {
                ret = ((*boost::any_cast<MpsBlm>(amc[addr])).*(blm_idleIt->second.first))(blm_idleIt->second.second, value);
            }
            else
            {
                if ( 0 == asynPortDriver::writeUInt32Digital(pasynUser, value, mask) )
                    ret = true;
            }
        }
        catch (CPSWError &e)
        {
            asynPrint(pasynUser, ASYN_TRACE_ERROR, "CPSW Error on %s writing parameter %s: %s\n", functionName, name, e.getInfo().c_str());
        }
        catch (std::runtime_error &e)
        {
            asynPrint(pasynUser, ASYN_TRACE_ERROR, "Runtime error on %s writing parameter %s: %s\n", functionName, name, e.what());
        }
    }
    else
    {
        if ( 0 == asynPortDriver::writeUInt32Digital(pasynUser, value, mask) )
            ret = true;
    }

    return ( ret ) ? asynSuccess : asynError;
}

asynStatus L2MPS::writeFloat64 (asynUser *pasynUser, epicsFloat64 value)
{
    int addr;
    int function = pasynUser->reason;
    bool ret = false;  // return status from l2Mps API ( false on error)
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
            bpm_scaleSlopeFuncMap_t::iterator bpm_scaleSlopeIt;
            bpm_scaleOffsetFuncMap_t::iterator bpm_scaleOffsetIt;

            blen_fmap_w32_t::iterator blen_it;
            blen_scaleSlopeFuncMap_t::iterator blen_scaleSlopeIt;
            blen_scaleOffsetFuncMap_t::iterator blen_scaleOffsetIt;

            bcm_fmap_w32_t::iterator bcm_it;
            bcm_scaleSlopeFuncMap_t::iterator bcm_scaleSlopeIt;
            bcm_scaleOffsetFuncMap_t::iterator bcm_scaleOffsetIt;

            blm_fmap_w32_t::iterator blm_it;
            blm_scaleSlopeFuncMap_t::iterator blm_scaleSlopeIt;
            blm_scaleOffsetFuncMap_t::iterator blm_scaleOffsetIt;

            // BPM parameters
            if ((bpm_it = fMapBpmW32.find(function)) != fMapBpmW32.end())
            {
                ret = ((*boost::any_cast<MpsBpm>(amc[addr])).*(bpm_it->second.first))(bpm_it->second.second, value);
            }
            else if ((bpm_scaleSlopeIt = fMapBpmWScaleSlope.find(function)) != fMapBpmWScaleSlope.end())
            {
                ret = ((*boost::any_cast<MpsBpm>(amc[addr])).*(bpm_scaleSlopeIt->second.first))(bpm_scaleSlopeIt->second.second, value);
            }
            else if ((bpm_scaleOffsetIt = fMapBpmWScaleOffset.find(function)) != fMapBpmWScaleOffset.end())
            {
                ret = ((*boost::any_cast<MpsBpm>(amc[addr])).*(bpm_scaleOffsetIt->second.first))(bpm_scaleOffsetIt->second.second, value);
            }
            // BLEN parameters
            else if ((blen_it = fMapBlenW32.find(function)) != fMapBlenW32.end())
            {
                ret = ((*boost::any_cast<MpsBlen>(amc[addr])).*(blen_it->second.first))(blen_it->second.second, value);
            }
            else if ((blen_scaleSlopeIt = fMapBlenWScaleSlope.find(function)) != fMapBlenWScaleSlope.end())
            {
                ret = ((*boost::any_cast<MpsBlen>(amc[addr])).*(blen_scaleSlopeIt->second.first))(blen_scaleSlopeIt->second.second, value);
            }
            else if ((blen_scaleOffsetIt = fMapBlenWScaleOffset.find(function)) != fMapBlenWScaleOffset.end())
            {
                ret = ((*boost::any_cast<MpsBlen>(amc[addr])).*(blen_scaleOffsetIt->second.first))(blen_scaleOffsetIt->second.second, value);
            }
            // BCM parameters
            else if ((bcm_it = fMapBcmW32.find(function)) != fMapBcmW32.end())
            {
                ret = ((*boost::any_cast<MpsBcm>(amc[addr])).*(bcm_it->second.first))(bcm_it->second.second, value);
            }
            else if ((bcm_scaleSlopeIt = fMapBcmWScaleSlope.find(function)) != fMapBcmWScaleSlope.end())
            {
                ret = ((*boost::any_cast<MpsBcm>(amc[addr])).*(bcm_scaleSlopeIt->second.first))(bcm_scaleSlopeIt->second.second, value);
            }
            else if ((bcm_scaleOffsetIt = fMapBcmWScaleOffset.find(function)) != fMapBcmWScaleOffset.end())
            {
                ret = ((*boost::any_cast<MpsBcm>(amc[addr])).*(bcm_scaleOffsetIt->second.first))(bcm_scaleOffsetIt->second.second, value);
            }
            // BLM parameters
            else if ((blm_it = fMapBlmW32.find(function)) != fMapBlmW32.end())
            {
                ret = ((*boost::any_cast<MpsBlm>(amc[addr])).*(blm_it->second.first))(blm_it->second.second, value);
            }
            else if ((blm_scaleSlopeIt = fMapBlmWScaleSlope.find(function)) != fMapBlmWScaleSlope.end())
            {
                ret = ((*boost::any_cast<MpsBlm>(amc[addr])).*(blm_scaleSlopeIt->second.first))(blm_scaleSlopeIt->second.second, value);
            }
            else if ((blm_scaleOffsetIt = fMapBlmWScaleOffset.find(function)) != fMapBlmWScaleOffset.end())
            {
                ret = ((*boost::any_cast<MpsBlm>(amc[addr])).*(blm_scaleOffsetIt->second.first))(blm_scaleOffsetIt->second.second, value);
            }
            else
            {
                if ( 0 == asynPortDriver::writeFloat64(pasynUser, value) )
                    ret = true;
            }
        }
        catch (CPSWError &e)
        {
            asynPrint(pasynUser, ASYN_TRACE_ERROR, "CPSW Error on %s writing parameter %s: %s\n", functionName, name, e.getInfo().c_str());
        }
        catch (std::runtime_error &e)
        {
            asynPrint(pasynUser, ASYN_TRACE_ERROR, "Runtime error on %s writing parameter %s: %s\n", functionName, name, e.what());
        }
    }
    else
    {
        if ( 0 == asynPortDriver::writeFloat64(pasynUser, value) )
            ret = true;
    }

    return ( ret ) ? asynSuccess : asynError;
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
    std::array<std::string, numberOfBays> recPreBay = {{ std::string(recordPrefixBay0), std::string(recordPrefixBay1) }};
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

    new L2MPS(portName, appID, recPreMps, recPreBay, mpsRoot);

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
