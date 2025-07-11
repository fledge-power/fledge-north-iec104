#ifndef _IEC104SERVER_H
#define _IEC104SERVER_H

/*
 * Fledge IEC 104 north plugin.
 *
 * Copyright (c) 2020, RTE (https://www.rte-france.com)
 *
 * Released under the Apache 2.0 Licence
 *
 * Authors: Akli Rahmoun <akli.rahmoun at rte-france.com>, Michael Zillgith <michael.zillgith@mz-automation.de>
 */

// clang-format off

#include <plugin_api.h>
#include <vector>
#include <map>
#include <mutex>
#include <thread>
#include <memory>

#include "lib60870/cs104_slave.h"
#include "lib60870/cs101_information_objects.h"

#include "iec104_config.hpp"

// clang-format on

class Reading;
class ConfigCategory;
class IEC104DataPoint;
class Datapoint;
class DatapointValue;
class IEC104ServerRedGroup;
class RedGroupCon;

class InformationObject_RAII {
    public:

    explicit InformationObject_RAII(InformationObject io): m_io(io) {}
    InformationObject_RAII& operator=(InformationObject_RAII&&) = delete;
    ~InformationObject_RAII() {
        if (m_io) {
            InformationObject_destroy(m_io);
        }
    }
    InformationObject m_io = nullptr;
};

class IEC104OutstandingCommand
{
public:

    IEC104OutstandingCommand(CS101_ASDU asdu, IMasterConnection connection, int cmdExecTimeout, bool isSelect);
    ~IEC104OutstandingCommand();

    bool isMatching(int typeId, int ca, int ioa);
    bool isSentFromConnection(IMasterConnection connection);
    bool hasTimedOut(uint64_t currentTime);
    bool isSelect();

    void sendActCon(bool negative);
    void sendActTerm(bool negative);

    int CA() {return m_ca;};
    int IOA() {return m_ioa;};
    int TypeId() {return m_typeId;};

private:

    CS101_ASDU m_receivedAsdu = nullptr;

    IMasterConnection m_connection = nullptr;

    int m_typeId = 0;
    int m_ca = 0;
    int m_ioa = 0;

    bool m_isSelect = false;

    int m_cmdExecTimeout = 0;

    uint64_t m_commandRcvdTime = 0;
    uint64_t m_nextTimeout = 0;

    int m_state = 0; /* 0 - idle/complete, 1 - waiting for ACT-CON, 2 - waiting for ACT-TERM */
};

class IEC104Server
{
public:
    IEC104Server();
    ~IEC104Server();
    
    void setJsonConfig(const std::string& stackConfig,
                                 const std::string& dataExchangeConfig,
                                const std::string& tlsConfig);

    void configure(const ConfigCategory* conf);
    bool startSlave();
    uint32_t send(const std::vector<Reading*>& readings);
    void stop();

    int ActConTimeout() {return m_actConTimeout;};
    int ActTermTimeout() {return m_actTermTimeout;};

    void ActConTimeout(int value) {m_actConTimeout = value;};
    void ActTermTimeout(int value) {m_actTermTimeout = value;};

    void registerControl(int (* operation)(char *operation, int paramCount, char* names[], char *parameters[], ControlDestination destination, ...));

    int operation(char *operation, int paramCount, char *names[], char *parameters[]);

    inline const std::string& getServiceName() const { return m_service_name; }
    inline void setServiceName(const std::string& serviceName) { m_service_name = serviceName; }

    IEC104Config* Config(){ return m_config; };

private:

    std::vector<IEC104OutstandingCommand*> m_outstandingCommands;
    std::mutex m_outstandingCommandsLock;
    std::recursive_mutex m_connectionEventsLock; // Lock used in audits, based on connections events from lib60870
    std::map<int, std::map<int, IEC104DataPoint*>> m_exchangeDefinitions;
    
    IEC104DataPoint* m_getDataPoint(int ca, int ioa, int typeId);
    void m_enqueueSpontDatapoint(IEC104DataPoint* dp, CS101_CauseOfTransmission cot, IEC60870_5_TypeID typeId);
    void m_updateDataPoint(IEC104DataPoint* dp, IEC60870_5_TypeID typeId, DatapointValue* value, CP56Time2a ts, uint8_t quality);

    bool checkIfSouthConnected();

    bool checkTimestamp(CP56Time2a timestamp);
    bool checkIfCmdTimeIsValid(int typeId, InformationObject io);
    void addToOutstandingCommands(CS101_ASDU asdu, IMasterConnection connection, bool isSelect);
    bool forwardCommand(CS101_ASDU asdu, InformationObject command, IMasterConnection connection);
    void removeOutstandingCommands(IMasterConnection connection);
    void removeAllOutstandingCommands();
    void handleActCon(int type, int ca, int ioa, bool isNegative);
    void handleActTerm(int type, int ca, int ioa, bool isNegative);
    bool requestSouthConnectionStatus();
    void updateSouthMonitoringInstance(Datapoint* dp, IEC104Config::SouthPluginMonitor* southPluginMonitor);
    bool validateCommand(IMasterConnection connection, CS101_ASDU asdu);

    static void printCP56Time2a(CP56Time2a time);
    static void rawMessageHandler(void* parameter, IMasterConnection connection,
                                  uint8_t* msg, int msgSize, bool sent);
    static bool clockSyncHandler(void* parameter, IMasterConnection connection,
                                 CS101_ASDU asdu, CP56Time2a newTime);

    void sendInterrogationResponse(IMasterConnection connection, CS101_ASDU asdu, int ca, int qoi);

    static bool interrogationHandler(void* parameter,
                                     IMasterConnection connection,
                                     CS101_ASDU asdu, uint8_t qoi);
    static bool asduHandler(void* parameter, IMasterConnection connection,
                            CS101_ASDU asdu);
    static bool connectionRequestHandler(void* parameter,
                                         const char* ipAddress);
    static void connectionEventHandler(void* parameter, IMasterConnection con,
                                       CS104_PeerConnectionEvent event);

    /**
     * @brief Send an audit for the connection status of a specific connection
     */
    void sendConnectionStatusAudit(const std::string& auditType, const std::string& redGroupIndex, const std::string& pathLetter);

    /**
     * @brief Send an audit for the global connection status
     */
    void sendGlobalStatusAudit(const std::string& auditType);

    /**
     * @brief Send initial audits and cap the total amounts of connections in lib60870
     */
    void sendInitialAudits();

    /**
     * @brief Verify if any conection from any of any of the configured redundancy groups is connected
     * @return true if one of the defined connection is established, else false
     */
    bool isAnyConnectionEstablished();


    CS104_Slave m_slave{};
    TLSConfiguration m_tlsConfig = nullptr;
    CS101_AppLayerParameters alParams = nullptr;
    IEC104Config* m_config = nullptr;

    int m_actConTimeout = 1000;
    int m_actTermTimeout = 1000;

    int (*m_oper)(char *operation, int paramCount, char* names[], char* parameters[], ControlDestination destination, ...) = nullptr;

    bool m_started = false;
    std::thread* m_monitoringThread = nullptr;
    void _monitoringThread();

    bool createTLSConfiguration();
    std::string m_service_name;    // Service name used to generate audits
    std::string m_last_connection_audit;      // Last audit sent. Prevent from sending the same audit multiple times
    std::string m_last_global_audit;      // Last global audit sent. Prevent from sending the same audit multiple times

    bool m_initSocketFinished = false; // In AISC, true if the operation "north_status" : "init_socket_finished" has been sent.
};

#endif