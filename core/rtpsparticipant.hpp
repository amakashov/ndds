#pragma once
#include <boost/asio/steady_timer.hpp>
#include <boost/uuid.hpp>
#include <core/dds_glob_defines.hpp>
#include <core/udptransport.hpp>
#include <cstdint>
#include <iomanip>
#include <memory>
#include <unordered_map>
#include <map>

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

    Guid GenerateGuid();

    std::string GuidToString(const std::array<uint8_t, 12> &prefix);

    std::vector<uint8_t> buildSpdpPacket();

    bool AddParticipant(std::array<uint8_t, 12> const &guid_prefix,
                        std::string const &name, uint32_t builtin_endpoints);

    bool UpdateParticipant(std::array<uint8_t, 12> const &guid_prefix,
                           std::string const &name, uint32_t builtin_endpoints);

    boost::asio::io_context& m_io_context;
    Guid m_guid;
    uint16_t m_domain_id;
    uint16_t m_participant_id;
    std::shared_ptr<Core::UdpTransport> m_udp_transport;
    std::shared_ptr<boost::asio::steady_timer> m_spdp_timer;

    std::map<std::array<uint8_t, 12>, DiscoveredParticipant> m_participants;

    ProtocolVersionType version;
    LocatorType m_default_unicast_locator, m_default_multicast_locator;

    uint16_t const static base_port = 7400;
    uint16_t const static domain_offset = 250;
    uint16_t const static participant_offset = 2;
};

}   //  of namespace Core