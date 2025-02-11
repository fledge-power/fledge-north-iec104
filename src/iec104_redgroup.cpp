#include "iec104_redgroup.hpp"

using namespace std;

RedGroupCon::RedGroupCon(const string& clientIp) : m_clientIp(clientIp), m_port(""), m_pathLetter("") {}

RedGroupCon::RedGroupCon(const string& clientIp, const std::string& port, const std::string& pathLetter)
    : m_clientIp(clientIp), m_port(port), m_pathLetter(pathLetter)  {}

IEC104ServerRedGroup::IEC104ServerRedGroup(const std::string& name, int index, CS104_RedundancyGroup cs104RedGroup) : m_name(name), m_index(index), m_cs104RedGroup(cs104RedGroup) {}

void IEC104ServerRedGroup::AddConnection(std::shared_ptr<RedGroupCon> con)
{
    m_connections.push_back(con);
}

std::shared_ptr<RedGroupCon> IEC104ServerRedGroup::GetRedGroupCon(const std::string& ip, const std::string &port)
{
    std::shared_ptr<RedGroupCon> currentConnection = nullptr;
    auto connIt = std::find_if(m_connections.begin(), m_connections.end(), [&ip, &port](const std::shared_ptr<RedGroupCon>& redGroupCon) {
        return redGroupCon->ClientIP() == ip && redGroupCon->Port() == port;
    });

    if (connIt != m_connections.end()) {
        currentConnection = *connIt;
    }

    return currentConnection;
}