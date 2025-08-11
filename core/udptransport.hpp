#pragma once

#include <array>
#include <boost/asio.hpp>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace Core
{
    int const MAX_BUFFER = 2048;
    class UdpTransport : public std::enable_shared_from_this<UdpTransport>
    {
        using ReceiveCallback = std::function<void(const boost::asio::ip::udp::endpoint&, const std::vector<uint8_t>&)>;
    public:
        UdpTransport(boost::asio::io_context& io_context, uint16_t local_port);
        void SetHandle(ReceiveCallback callback) {m_receive_callback = std::move(callback);}

        void StartReceiving();
        void StopReceiving();
        void Send(const std::string& remote_ip, uint16_t remote_port, const std::vector<uint8_t>& data);
        void JoinMulticastGroup(const std::string& multicast_ip, uint16_t multicast_port);

    protected:

        void doReceive();

        boost::asio::io_context& m_io_context;
        uint16_t m_local_port;
        boost::asio::ip::udp::socket m_unicast_socket;
        // boost::asio::ip::udp::socket m_multicast_socket;
        boost::asio::ip::udp::endpoint m_remote_endpoint;
        std::array<uint8_t, MAX_BUFFER> m_buffer;
        ReceiveCallback m_receive_callback;
    };
}
