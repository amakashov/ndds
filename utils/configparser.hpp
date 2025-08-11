#pragma once

#include <istream>
#include <nlohmann/json.hpp>
#include <string>

#include <core/dds_glob_defines.hpp>
#include <utils/comm_structs.hpp>

class NDDS_EXPORT ConfigParser
{
public:
    ConfigParser(std::istream& stream) : m_stream(stream){}
    bool ParseStream();
    nlohmann::basic_json<> getSubConfig(std::string name) const;

    static bool checkSanity(nlohmann::basic_json<> const & config);

    static IPConnection getIpCofig(nlohmann::basic_json<> const & config);

  protected:
    std::istream& m_stream;
    nlohmann::json m_data;
};