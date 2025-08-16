#include <core/rtpsparticipant.hpp>
#include <iostream>

namespace Core
{    
RtpsParticipant::RtpsParticipant(boost::asio::io_context &io_context, uint16_t domainId)
    : m_io_context(io_context), m_uuid(boost::uuids::random_generator()()), m_domain_id(domainId),
    m_udp_transport(std::make_shared<Core::UdpTransport>(m_io_context, GetPortById(domainId))) 
    {
        std::vector<uint8_t> guid(m_uuid.begin(), m_uuid.end());
        std::cout << "UUID=" << m_uuid<< std::hex << " " ;
        for (auto v : guid)
            std::cout << +v;
        std::cout <<std::dec << std::endl; 
    }
        
}