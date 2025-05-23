#ifndef IEC104_SERVER_REDGROUP_H
#define IEC104_SERVER_REDGROUP_H

#include <string>
#include <vector>
#include <memory>
#include <algorithm>

#include <lib60870/cs104_slave.h>

class RedGroupCon
{
public:
    explicit RedGroupCon(const std::string& clientIp);
    RedGroupCon(const std::string& clientIp, const std::string& port, const std::string &pathLetter);
    ~RedGroupCon() = default;

    const std::string& ClientIP() const {return m_clientIp;};
    const std::string& Port() const {return m_port;};
    const std::string& PathLetter() const {return m_pathLetter;};
    const bool& isActive() const {return m_isActive;};

    void SetPort(const std::string& port) { m_port = port; };
    void SetPathLetter(const std::string& pathLetter) { m_pathLetter = pathLetter; };
    void SetActive(const bool& isActive) { m_isActive = isActive; };

private:
    /* configuration properties */
    std::string m_clientIp;
    std::string m_port;
    std::string m_pathLetter;
    bool m_isActive = false;
};

class IEC104ServerRedGroup
{
public:

    IEC104ServerRedGroup(const std::string& name, int index, CS104_RedundancyGroup cs104RedGroup);
    ~IEC104ServerRedGroup() = default;

    const std::string& Name() const {return m_name;};
    const CS104_RedundancyGroup& CS104RedGroup() const {return m_cs104RedGroup;};
    int Index() const {return m_index;};

    std::vector<std::shared_ptr<RedGroupCon>>& Connections() {return m_connections;};

    void AddConnection(std::shared_ptr<RedGroupCon> con);

    int GetMaxConnections() const {return m_maxConnections;};

    /// @brief Get the RedGroupCon object associated with an IP and, if provided, a PORT. 
    ///        If no PORT is provided, the empty value will be used. Meaning that the method
    ///         will return the first RedGroupCon with an empty PORT.
    /// @param ip The client IP
    /// @param port The port. Default value = ""
    /// @return std::shared_ptr<RedGroupCon>
    std::shared_ptr<RedGroupCon> GetRedGroupCon(const std::string& ip, const std::string &port="");

private:

    std::vector<std::shared_ptr<RedGroupCon>> m_connections;

    std::string m_name;
    int m_index;

    int m_maxConnections = 2;

    CS104_RedundancyGroup m_cs104RedGroup;
};


#endif /* IEC104_SERVER_REDGROUP_H */