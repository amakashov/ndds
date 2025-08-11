#include "configparser.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <iostream>
#include "comm_structs.hpp"

bool ConfigParser::ParseStream() 
{
    m_data = nlohmann::json::parse(m_stream, nullptr, false); 
    if (m_data.empty())
        return false;
    return true;
}

nlohmann::basic_json<> ConfigParser::getSubConfig(std::string name) const
{
    if (m_data.contains(name))
    {
        auto data = m_data.at(name);
        data["_name"] = name;
        return data;
    }
    return nlohmann::basic_json<>();
}

bool ConfigParser::checkSanity(nlohmann::basic_json<> const & config)
{
    if (!config.contains("type"))
        return false;
    auto type = config.at("type");
    if (!type.is_string())
        return false;
    if (type.template get<std::string>() != "udp_socket")
        return false;
    return true;
}

IPConnection ConfigParser::getIpCofig(nlohmann::basic_json<> const & config)
{
    IPConnection connInfo;
    auto parsePart = [](nlohmann::basic_json<> const & config, IPConnection::IPNode & node) 
    {
        if (config.contains("address"))
        {
            node.address = config.at("address").template get<std::string>();
        }
        if (config.contains("port"))
        {
            node.port = config.at("port").template get<int>();
        }
    };
    if (config.contains("sender"))
    {
        auto subConf = config.at("sender");
        parsePart(subConf, connInfo.sender);
    }
    if (config.contains("receiver"))
    {
        auto subConf = config.at("receiver");
        parsePart(subConf, connInfo.receiver);
    }  
    if (config.contains("frequency"))
    {
        if (config.at("frequency").is_number())
            connInfo.frequency = config.at("frequency").template get<float>(); 
    }  
    return connInfo;
}
