#include <boost/asio/io_context.hpp>
#include <core/udptransport.hpp>
#include <boost/asio/ip/udp.hpp>
#include <iostream>

namespace bip = boost::asio::ip;

namespace Core
{
    UdpTransport::UdpTransport(boost::asio::io_context& context, uint16_t port)
        : m_io_context(context), m_local_port(port), 
        m_unicast_socket(m_io_context, bip::udp::endpoint(bip::udp::v4(), m_local_port)) { }

    void UdpTransport::StartReceiving()
    {
        doReceive();
    }

    void UdpTransport::StopReceiving()
    {
        m_unicast_socket.close();
    }

    void UdpTransport::Send(const std::string& remote_ip, uint16_t remote_port, const std::vector<uint8_t>& data)
    {
        m_remote_endpoint = bip::udp::endpoint(bip::make_address(remote_ip), remote_port);
        m_unicast_socket.send_to(boost::asio::buffer(data), m_remote_endpoint);
    }

    void UdpTransport::JoinMulticastGroup(const std::string& multicast_ip, uint16_t multicast_port)
    {
        bip::udp::endpoint multicast_endpoint(boost::asio::ip::make_address(multicast_ip), multicast_port);
        m_unicast_socket.set_option(bip::multicast::join_group(multicast_endpoint.address()));
    }

    void UdpTransport::doReceive()
    {
        auto self = shared_from_this();
        m_unicast_socket.async_receive_from(
            boost::asio::buffer(m_buffer), m_remote_endpoint,
            [self](boost::system::error_code ec, std::size_t bytes_recvd)
            {
                if (!ec)
                {
                    std::vector<uint8_t> data(self->m_buffer.data(), self->m_buffer.data() + bytes_recvd);
                    self->m_receive_callback(self->m_remote_endpoint, data);
                }
                else 
                {
                    std::cerr << "Receive error: " << ec.message() << std::endl;
                }
                self->doReceive();
            });
    }

}