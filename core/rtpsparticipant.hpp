#pragma once
#include <boost/uuid.hpp>
#include <core/dds_glob_defines.hpp>
#include <core/udptransport.hpp>

namespace Core
{   
class NDDS_EXPORT RtpsParticipant : public std::enable_shared_from_this<RtpsParticipant>
{
public:
    RtpsParticipant(boost::asio::io_context &io_context, uint16_t domainId);
    
protected:
    static uint16_t GetPortById(uint16_t participantId) {return 7400 + participantId*250;}
    boost::asio::io_context& m_io_context;
    boost::uuids::uuid m_uuid;
    uint16_t m_domain_id;
    std::shared_ptr<Core::UdpTransport> m_udp_transport;

    ProtocolVersionType type;
    LocatorType m_defaul_unicast_locator, m_default_multicast_locator;

};

}   //  of namespace Core