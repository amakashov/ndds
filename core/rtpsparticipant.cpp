#include <core/rtpsparticipant.hpp>

namespace Core
{    
RtpsParticipant::RtpsParticipant(boost::asio::io_context &io_context,
    uint16_t domainId)
    : m_io_context(io_context),
    m_udp_transport(std::make_shared<Core::UdpTransport>(m_io_context, GetPortById(domainId))) {}

static uint16_t RtpsParticipant::GetPortById(uint16_t participantId) {return 7400 + participantId*250;}
        
}