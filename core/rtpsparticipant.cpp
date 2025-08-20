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
        m_guid = GenerateGuid();
        std::cout << GuidToString(m_guid.prefix) << std::endl;
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
    RtpsParticipant::Guid RtpsParticipant::GenerateGuid() 
    {
        Guid g;
        std::random_device rd;
        for (auto &b : g.prefix)
            b = rd() & 0xFF;
        g.entity_id = 0x000001c1;
        return g;
    }
    std::string RtpsParticipant::GuidToString(const std::array<uint8_t, 12> &prefix) 
    {
        std::ostringstream oss;
        oss << std::setw(2) << std::setfill('0');
        for (size_t i = 0; i < prefix.size(); i++) 
        {
            oss << std::hex << (int)prefix[i];
            if (i != prefix.size() - 1)
              oss << ":";
        }
        return oss.str();
    }
    bool
    RtpsParticipant::AddParticipant(std::array<uint8_t, 12> const &guid_prefix,
                                    std::string const &name,
                                    uint32_t builtin_endpoints) 
    {
        auto it = m_participants.find(guid_prefix);
        if (it != m_participants.end()) 
        {
            it->second.name = name;
            it->second.builtin_endpoints = builtin_endpoints;
            it->second.last_seen = std::chrono::steady_clock::now();
            return true;
        } 
        else 
        {
            DiscoveredParticipant dp{guid_prefix, name, builtin_endpoints,
                                     std::chrono::steady_clock::now()};
            m_participants[guid_prefix] = dp;
            return false;
        }
    }
    
    bool RtpsParticipant::UpdateParticipant(
        std::array<uint8_t, 12> const &guid_prefix, std::string const &name,
        uint32_t builtin_endpoints) 
    {
        auto it = m_participants.find(guid_prefix);
        if (it != m_participants.end()) 
        {
            it->second.name = name;
            it->second.builtin_endpoints = builtin_endpoints;
            it->second.last_seen = std::chrono::steady_clock::now();
            return true;
        }
        return false;
    }
    } // namespace Core