/*
 * Fledge IEC 104 north plugin utility functions.
 *
 * Copyright (c) 2022, RTE (https://www.rte-france.com)
 *
 * Released under the Apache 2.0 Licence
 *
 * Author: Michael Zillgith (michael.zillgith at mz-automation.de)
 * 
 */

#ifndef _IEC104_UTILITY_H
#define _IEC104_UTILITY_H

#include <string>
#include <logger.h>
#include <audit_logger.h>

#define PLUGIN_NAME "iec104"

namespace Iec104Utility {

    static const std::string PluginName = PLUGIN_NAME;

    /*
     * Log helper function that will log both in the Fledge syslog file and in stdout for unit tests
     */
    template<class... Args>
    void log_debug(const std::string& format, Args&&... args) {  
        #ifdef UNIT_TEST
        printf(std::string(format).append("\n").c_str(), std::forward<Args>(args)...);
        fflush(stdout);
        #endif
        Logger::getLogger()->debug(format.c_str(), std::forward<Args>(args)...);
    }

    template<class... Args>
    void log_info(const std::string& format, Args&&... args) {    
        #ifdef UNIT_TEST
        printf(std::string(format).append("\n").c_str(), std::forward<Args>(args)...);
        fflush(stdout);
        #endif
        Logger::getLogger()->info(format.c_str(), std::forward<Args>(args)...);
    }

    template<class... Args>
    void log_warn(const std::string& format, Args&&... args) { 
        #ifdef UNIT_TEST  
        printf(std::string(format).append("\n").c_str(), std::forward<Args>(args)...);
        fflush(stdout);
        #endif
        Logger::getLogger()->warn(format.c_str(), std::forward<Args>(args)...);
    }

    template<class... Args>
    void log_error(const std::string& format, Args&&... args) {   
        #ifdef UNIT_TEST
        printf(std::string(format).append("\n").c_str(), std::forward<Args>(args)...);
        fflush(stdout);
        #endif
        Logger::getLogger()->error(format.c_str(), std::forward<Args>(args)...);
    }

    template<class... Args>
    void log_fatal(const std::string& format, Args&&... args) {  
        #ifdef UNIT_TEST
        printf(std::string(format).append("\n").c_str(), std::forward<Args>(args)...);
        fflush(stdout);
        #endif
        Logger::getLogger()->fatal(format.c_str(), std::forward<Args>(args)...);
    }


    inline std::string m_addQuotes(const std::string& str, bool addQuotes) {
        if (addQuotes) {
            return std::string("\"") + str + "\"";
        }
        else {
           return str;
        }
    }

    inline std::string m_toJsonMessage(const std::string& str) {
        return std::string("{\"message\":") + str + std::string("}");
    }

    /*
     * Log helper function that will log in the Fledge audit logs
     */

    inline void audit_fail(const std::string& code, const std::string& data, bool addQuotes = true) {
        // Audits accept pure json string, so if we want a simple text messages we need to add quotes to it
        std::string message = m_addQuotes(data, addQuotes);
        AuditLogger::auditLog(code.c_str(), "FAILURE", m_toJsonMessage(message).c_str());
    }

    inline void audit_success(const std::string& code, const std::string& data, bool addQuotes = true) {
        // Audits accept pure json string, so if we want a simple text messages we need to add quotes to it
        std::string message = m_addQuotes(data, addQuotes);
        AuditLogger::auditLog(code.c_str(), "SUCCESS", m_toJsonMessage(message).c_str());
    }

    inline void audit_warn(const std::string& code, const std::string& data, bool addQuotes = true) {
        // Audits accept pure json string, so if we want a simple text messages we need to add quotes to it
        std::string message = m_addQuotes(data, addQuotes);
        AuditLogger::auditLog(code.c_str(), "WARNING", m_toJsonMessage(message).c_str());
    }

    inline void audit_info(const std::string& code, const std::string& data, bool addQuotes = true) {
        // Audits accept pure json string, so if we want a simple text messages we need to add quotes to it
        std::string message = m_addQuotes(data, addQuotes);
        AuditLogger::auditLog(code.c_str(), "INFORMATION", m_toJsonMessage(message).c_str());
    }
}

#endif /* _IEC104_UTILITY_H */