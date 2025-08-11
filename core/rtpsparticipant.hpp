#pragma once
#include <core/udptransport.hpp>

namespace Core
{   
class RtpsParticipant : public std::enable_shared_from_this<RtpsParticipant>
{
public:
    RtpsParticipant(boost::asio::io_context &io_context, uint16_t domainId);
    
protected:
    static uint16_t GetPortById(uint16_t participantId);
    boost::asio::io_context& m_io_context;
    uint16_t m_domainId;
    std::shared_ptr<Core::UdpTransport> m_udp_transport;
};

}   //  of namespace Core