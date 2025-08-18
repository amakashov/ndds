#include "core/udptransport.hpp"
#include <core/rtpsparticipant.hpp>
#include <iostream>

namespace Core
{    
RtpsParticipant::RtpsParticipant(boost::asio::io_context &io_context, uint16_t domainId, uint16_t participantId)
    : m_io_context(io_context), m_domain_id(domainId), m_participant_id(participantId),
    m_udp_transport(std::make_shared<Core::UdpTransport>(m_io_context, 
        GetLocalPortById(m_domain_id, m_participant_id), GetMulticastPortById(m_domain_id))) 
    {
        m_guid = generateGuid();
        std::cout << guidToString(m_guid.prefix) << std::endl;
    }   

    bool RtpsParticipant::JoinMulticastGroup(std::string const & ip)
    {
        if (m_udp_transport)
        {
            auto callback = [](const boost::asio::ip::udp::endpoint& ep, const std::vector<uint8_t>& buff) 
            {
                std::cout << "From " << ep.address().to_string() << ":" << ep.port() << " received ";
                for (auto v : buff)
                    std::cout << v << " ";
                std::cout << std::endl;
            };
            m_udp_transport->SetMulticastHandle(callback);
            return  m_udp_transport->JoinMulticastGroup(ip);
        }
        else return false;
    }

    void RtpsParticipant::SendUnicast(std::vector<uint8_t> const & buffer, std::array<uint8_t, 12> guid)
    {

    }

    std::vector<uint8_t> RtpsParticipant::buildSpdpPacket()
    {
        std::vector<uint8_t> packet{'R','T','P','S'};
        packet.push_back((static_cast<uint16_t>(version) >> 8) & 0xFF);
        packet.push_back(static_cast<uint16_t>(version) & 0xFF);
        //      TODO Здесь должен быть vendorID, и его надо бы сделать константой
        packet.push_back(0);
        packet.push_back(0);
        packet.insert(packet.end(), m_guid.prefix.begin(), m_guid.prefix.end());

        return packet;

    }

    void RtpsParticipant::Shutdown()
    {
        if (m_udp_transport)
        {
            m_udp_transport->StopReceiving();
        }
    }
}