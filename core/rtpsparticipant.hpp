#pragma once
#include <boost/asio/steady_timer.hpp>
#include <boost/uuid.hpp>
#include <core/dds_glob_defines.hpp>
#include <core/udptransport.hpp>
#include <cstdint>
#include <iomanip>
#include <memory>
#include <unordered_map>

namespace Core
{ 

struct DiscoveredParticipant {
    std::array<uint8_t, 12> guid_prefix{};
    std::string name;
    uint32_t builtin_endpoints{};
    std::chrono::steady_clock::time_point last_seen;
};

class NDDS_EXPORT RtpsParticipant : public std::enable_shared_from_this<RtpsParticipant>
{
public:
    RtpsParticipant(boost::asio::io_context &io_context, uint16_t domain_id, uint16_t participant_id);
    bool JoinMulticastGroup(std::string const & ip);

    void SendUnicast(std::vector<uint8_t> const & buffer, std::array<uint8_t, 12> guid);

    void Shutdown();
    
protected:
    static uint16_t GetLocalPortById(uint16_t domain_id, uint16_t participant_id) 
    {
        return base_port + domain_offset*domain_id + participant_offset*participant_id;
    }
    static uint16_t GetMulticastPortById(uint16_t domain_id) 
    {
        return base_port + domain_offset*domain_id;
    }

    struct Guid 
    {
        std::array<uint8_t, 12> prefix;
        uint32_t entity_id;
    };

    Guid generateGuid() 
    {
        Guid g;
        std::random_device rd;
        for (auto &b : g.prefix)
         b = rd() & 0xFF;
        g.entity_id = 0x000001c1;
        return g;
    }

    std::string guidToString(const std::array<uint8_t, 12>& prefix) {
        std::ostringstream oss;
        oss<< std::setw(2) << std::setfill('0');
        for (size_t i = 0; i < prefix.size(); i++) 
        {
            oss << std::hex << (int)prefix[i];
            if (i != prefix.size()-1) oss << ":";
        }
        return oss.str();
    }
    
    std::vector<uint8_t> buildSpdpPacket();

    boost::asio::io_context& m_io_context;
    Guid m_guid;
    uint16_t m_domain_id;
    uint16_t m_participant_id;
    std::shared_ptr<Core::UdpTransport> m_udp_transport;
    std::shared_ptr<boost::asio::steady_timer> m_spdp_timer;

    std::unordered_map<std::string, DiscoveredParticipant> m_participants;

    ProtocolVersionType version;
    LocatorType m_default_unicast_locator, m_default_multicast_locator;

    uint16_t const static base_port = 7400;
    uint16_t const static domain_offset = 250;
    uint16_t const static participant_offset = 2;
};

}   //  of namespace Core