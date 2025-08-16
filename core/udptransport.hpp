#pragma once

#include <array>
#include <boost/asio.hpp>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <core/dds_glob_defines.hpp>

namespace Core
{
    int const MAX_BUFFER = 2048;
    class NDDS_EXPORT UdpTransport : public std::enable_shared_from_this<UdpTransport>
    {
        using ReceiveCallback = std::function<void(const boost::asio::ip::udp::endpoint&, const std::vector<uint8_t>&)>;
    public:
        UdpTransport(boost::asio::io_context& io_context, uint16_t local_port);
        void SetHandle(ReceiveCallback callback) {m_receive_callback = std::move(callback);}
        void SetMulticastHandle(ReceiveCallback callback) {m_multicast_callback = std::move(callback);}

        void StartReceiving();
        void StopReceiving();
        void Send(const std::string& remote_ip, uint16_t remote_port, const std::vector<uint8_t>& data);
        void SendMulticast(std::vector<uint8_t> const & data);
 
        void JoinMulticastGroup(const std::string& multicast_ip);

    protected:

        void doReceive();
        void doMulticastReceive();

        boost::asio::io_context& m_io_context;
        uint16_t m_local_port;
        uint16_t const m_multicast_port = 7400;
        boost::asio::ip::udp::socket m_unicast_socket;
        boost::asio::ip::udp::socket m_multicast_socket;
        boost::asio::ip::udp::endpoint m_remote_endpoint;
        boost::asio::ip::udp::endpoint m_multicast_endpoint;

        std::array<uint8_t, MAX_BUFFER> m_buffer;
        ReceiveCallback m_receive_callback;
        ReceiveCallback m_multicast_callback;
    };
}
