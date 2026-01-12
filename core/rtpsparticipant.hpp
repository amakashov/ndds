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
#include <iostream>

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

    void SendMulticast(std::vector<uint8_t> const &buffer);

    void Shutdown();

    void SchedulePeriodicSpdpMessages();

  protected:
    static uint16_t GetLocalPortById(uint16_t domain_id, uint16_t participant_id) 
    {
        std::cout << "GetLocalPortById called with domain_id=" << domain_id << " participant_id=" << participant_id;
        std::cout << " returning port " << base_port + domain_offset*domain_id + participant_offset*participant_id << std::endl;
        return base_port + domain_offset*domain_id + participant_offset*participant_id;
    }
    static uint16_t GetMulticastPortById(uint16_t domain_id) 
    {
        std::cout<< "GetMulticastPortById called with domain_id=" << domain_id << " returning port " << base_port + domain_offset*domain_id << std::endl;
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

    void SendSpdpMessage();

    void ScheduleSpdpMessage() {}

    void HandleSpdpTimeout(const boost::system::error_code& ec) {}

    void StartSpdpTimer() {}

    void StopSpdpTimer() {}

    void PrintDiscoveredParticipants() {}

    void RemoveStaleParticipants(){}

    void HandleStaleParticipantsTimeout(const boost::system::error_code& ec) {}

    void StartStaleParticipantsTimer() {}

    void StopStaleParticipantsTimer() {}

    void ResetStaleParticipantsTimer() {}

    void onMulticastReceive(const boost::asio::ip::udp::endpoint &ep,
                            const std::vector<uint8_t> &data);

    void onUnicastReceive(const boost::asio::ip::udp::endpoint &ep,
                          const std::vector<uint8_t> &data);

    boost::asio::io_context& m_io_context;
    Guid m_guid;
    uint16_t m_domain_id;
    uint16_t m_participant_id;
    std::shared_ptr<Core::UdpTransport> m_udp_transport;
    std::shared_ptr<boost::asio::steady_timer> m_spdp_timer;

    std::map<std::array<uint8_t, 12>, DiscoveredParticipant> m_participants;

    boost::asio::chrono::seconds m_spdp_interval{1};
    boost::asio::chrono::seconds m_participant_liveliness_duration{5};
    boost::asio::chrono::seconds m_stale_participants_check_interval{1};


    ProtocolVersionType version;
    LocatorType m_default_unicast_locator, m_default_multicast_locator;

    uint16_t const static base_port = 7400;
    uint16_t const static domain_offset = 250;
    uint16_t const static participant_offset = 2;

    std::vector<uint8_t> m_spdp_message;
};

}   //  of namespace Core