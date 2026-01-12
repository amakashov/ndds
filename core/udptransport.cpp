#include <boost/asio/io_context.hpp>
#include <core/udptransport.hpp>
#include <boost/asio/ip/udp.hpp>
#include <iostream>

namespace bip = boost::asio::ip;

namespace Core
{
    UdpTransport::UdpTransport(boost::asio::io_context& context, uint16_t local_port, uint16_t multicast_port)
        : m_io_context(context), m_local_port(local_port), m_multicast_port(multicast_port),
        m_multicast_socket(m_io_context), 
        m_unicast_socket(m_io_context, bip::udp::endpoint(bip::udp::v4(), m_local_port)) 
    { 

    }

    void UdpTransport::StartReceiving()
    {
        doReceive();
    }

    void UdpTransport::StopReceiving()
    {
        m_unicast_socket.close();
        m_multicast_socket.close();
    }

    void UdpTransport::Send(const std::string& remote_ip, uint16_t remote_port, const std::vector<uint8_t>& data)
    {
        m_remote_endpoint = bip::udp::endpoint(bip::make_address(remote_ip), remote_port);
        m_unicast_socket.send_to(boost::asio::buffer(data), m_remote_endpoint);
    }
    
    void UdpTransport::SendMulticast(const std::vector<uint8_t>& data)
    {
        auto self = shared_from_this();
        auto buff = boost::asio::buffer(data);
        m_multicast_socket.async_send_to(buff, m_multicast_endpoint, 
            [self](const boost::system::error_code& ec, size_t bytes_transferred) 
            {
                if (ec)
                {
                    std::cout << "Error sending multicast message: " << ec.what() << std::endl;
                }
            });
    }

    bool UdpTransport::JoinMulticastGroup(const std::string& multicast_ip)
    {
        try
        {
            m_multicast_endpoint = bip::udp::endpoint(boost::asio::ip::make_address_v4(multicast_ip), m_multicast_port);
            m_multicast_socket.open(bip::udp::v4());
            m_multicast_socket.set_option(bip::udp::socket::reuse_address(true));
            #ifdef SO_REUSEPORT 
            m_multicast_socket.set_option(bip::socket_base::reuse_port(true));
            #endif
            m_multicast_socket.bind(bip::udp::endpoint(bip::address_v4::any(), m_multicast_port));
            m_multicast_socket.set_option(bip::multicast::join_group(m_multicast_endpoint.address()));
            m_multicast_socket.set_option(bip::multicast::enable_loopback(true));
            doMulticastReceive();
            return true;
        }
        catch(const std::exception& e)
        {
            std::cerr <<"Multicast trouble " << e.what() << '\n';
            return false;
        }
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

    void UdpTransport::doMulticastReceive()
    {
        auto self = shared_from_this();
        m_multicast_socket.async_receive_from(
            boost::asio::buffer(m_buffer), m_remote_endpoint,
            [self](boost::system::error_code ec, std::size_t bytes_recvd)
            {
                if (!ec)
                {
                    std::vector<uint8_t> data(self->m_buffer.data(), self->m_buffer.data() + bytes_recvd);
                    self->m_multicast_callback(self->m_multicast_endpoint, data);
                }
                else 
                {
                    std::cerr << "Multicast error: " << ec.message() << std::endl;
                }
                self->doMulticastReceive();
            });
    }

}