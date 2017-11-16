/*!
 * @file
 *   @brief ST16 Controller Utilities
 *   @author Gus Grubba <mavlink@grubba.com>
 *
 */

#pragma once

#include "TyphoonHCommon.h"
#include "m4def.h"

#include <string>
#include <vector>

uint8_t crc8(uint8_t* buffer, int len);

#define DEFAULT_TX_MAX_CHANNEL			24

typedef  struct  TableDeviceLocalInfo{
    uint8_t 	index;
    uint16_t    mode;
    uint16_t    nodeId;
    uint8_t 	parseIndex;
    uint8_t 	extAddr;
    uint16_t    panId;
    uint16_t    txAddr;
} TableDeviceLocalInfo_t;

typedef  struct  TableDeviceChannelInfo{
    uint8_t index;
    uint8_t aNum;
    uint8_t aBits;
    uint8_t trNum;
    uint8_t trBits;
    uint8_t swNum;
    uint8_t swBits;
    uint8_t replyChannelNum;
    uint8_t replyChannelBits;
    uint8_t requestChannelNum;
    uint8_t requestChannelBits;
    uint8_t extraNum;
    uint8_t extraBits;
    uint8_t analogType;
    uint8_t trimType;
    uint8_t switchType;
    uint8_t replyChannelType;
    uint8_t requestChannelType;
    uint8_t extraType;
} TableDeviceChannelInfo_t;

typedef  struct  TableDeviceChannelNumInfo{
    uint8_t index;
    uint8_t channelMap[DEFAULT_TX_MAX_CHANNEL];
} TableDeviceChannelNumInfo_t;

typedef enum {
    ChannelNumAanlog = 1,
    ChannelNumTrim,
    ChannelNumSwitch,
    ChannelNumMonitor,
    ChannelNumExtra,
} ChannelNumType_t;

typedef enum {
    CalibrationHwIndexJ1 = 0,
    CalibrationHwIndexJ2,
    CalibrationHwIndexJ3,
    CalibrationHwIndexJ4,
    CalibrationHwIndexK1,
    CalibrationHwIndexK2,
    CalibrationHwIndexK3,
    CalibrationHwIndexMax
} CalibrationHwIndex_t;


//-----------------------------------------------------------------------------
//-- Accessor class to handle data structure in an "Yuneec" way
class m4Packet
{
public:
    m4Packet(QByteArray data_)
        : data(data_)
    {
    }
    int type()
    {
        return data[2] & 0xFF;
    }
    int commandID()
    {
        return data[9] & 0xFF;
    }
    QByteArray commandValues()
    {
        return data.mid(10); //-- Data - header = payload
    }
    QByteArray passthroughValues()
    {
        return data.mid(9); //-- Data - header = payload
    }
    QByteArray data;
};

//-----------------------------------------------------------------------------
class SwitchChanged {
public:
    /**
     * Hardware ID
     */
    int hwId;
    /**
     * Old status of hardware
     */
    int oldState;
    /**
     * New status of hardware
     */
    int newState;
};

#define m4CommandHeaderLen sizeof(m4CommandHeader)

//-----------------------------------------------------------------------------
// Base Yuneec Protocol Command
class m4Command
{
public:
    m4Command(int id, int type = Yuneec::COMMAND_TYPE_NORMAL)
    {
        data.fill(0, Yuneec::COMMAND_BODY_EXCLUDE_VALUES_LENGTH);
        data[2] = (uint8_t)type;
        data[9] = (uint8_t)id;
    }
    virtual ~m4Command() {}
    QByteArray pack(QByteArray payload = QByteArray())
    {
        if(payload.size()) {
            data.append(payload);
        }
        QByteArray command;
        command.resize(3);
        command[0] = 0x55;
        command[1] = 0x55;
        command[2] = (uint8_t)data.size() + 1;
        command.append(data);
        uint8_t crc = crc8((uint8_t*)data.data(), data.size());
        command.append(crc);
        return command;
    }
    QByteArray data;
};

//-----------------------------------------------------------------------------
// Base Yuneec Protocol Command
class m4PassThroughCommand
{
public:
    m4PassThroughCommand()
    {
        data.fill(0, Yuneec::COMMAND_BODY_EXCLUDE_VALUES_LENGTH - 1);
        data[2] = (uint8_t)Yuneec::COMMAND_TYPE_PASS_THROUGH;
    }
    virtual ~m4PassThroughCommand() {}
    QByteArray pack(QByteArray payload = QByteArray())
    {
        if(payload.size()) {
            data.append(payload);
        }
        QByteArray command;
        command.resize(3);
        command[0] = 0x55;
        command[1] = 0x55;
        command[2] = (uint8_t)data.size() + 1;
        command.append(data);
        uint8_t crc = crc8((uint8_t*)data.data(), data.size());
        command.append(crc);
        return command;
    }
    QByteArray data;
};

//-----------------------------------------------------------------------------
// Base Yuneec Protocol Message
class m4Message
{
public:
    m4Message(int id, int type = 0)
    {
        data.fill(0, 8);
        data[2] = (uint8_t)type;
        data[3] = (uint8_t)id;
    }
    virtual ~m4Message() {}
    QByteArray pack()
    {
        QByteArray command;
        command.resize(3);
        command[0] = 0x55;
        command[1] = 0x55;
        command[2] = (uint8_t)data.size() + 1;
        command.append(data);
        uint8_t crc = crc8((uint8_t*)data.data(), data.size());
        command.append(crc);
        return command;
    }
    QByteArray data;
};
