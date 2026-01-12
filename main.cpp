#include <iostream>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <fstream>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>

#include "core/udptransport.hpp"
#include "utils/configparser.hpp"
#include "core/rtpsparticipant.hpp"
#include <boost/asio/executor_work_guard.hpp>
#include <vector>

using namespace nlohmann;
using namespace std;


int main(int, char**)
{
    // cout << "Node sample started" << endl;
    // auto j3 = json::parse(R"({"happy": true, "pi": 3.141, "arr" : [1,2,3,5], "obj" : {"first" :1, "second" : 2}})");
    // cout << j3 << endl;
    // for (auto & a : j3)
    //     cout << a << " is object? "<< a.is_object() <<  endl;

    // unordered_map<string, double> freqs {{"begin", -1.1}, {"end", 12.05}, {"middle", -2.3}};
    // json j_umap(freqs);
    // j3["coords"] = j_umap;
    // j3["type"] = "serial";

    // ofstream of("test.json");
    // if (of.is_open())
    //     of << j3 << endl;
    // of.close();

    // std::ifstream ifs("test.json");
    // if (ifs.is_open())
    // {
    //     ConfigParser parser(ifs);
    //     parser.ParseStream();
    // }
    // ifs.close();

    // std::string serialized = j3.dump();
    // cout << "Serialized string" << serialized << endl;
    // stringstream istrm(serialized);

    // if (istrm.good())
    // {
    //     ConfigParser parser(istrm);
    //     parser.ParseStream();        
    // }

    // ifs.open("device.json");
    // if (ifs.is_open())
    // {
    //     ConfigParser parser(ifs);
    //     parser.ParseStream();
    //     auto devConf = parser.getSubConfig("test_device");
    //     if (!ConfigParser::checkSanity(devConf))
    //     {
    //         cout << "Insane config " << devConf << endl;
    //     }
    //     auto ipConf = ConfigParser::getIpCofig(devConf);
    //     cout << "Sender: " << ipConf.sender.address << " port " << ipConf.sender.port << endl;
    //     cout << "Receiver: " << ipConf.receiver.address << " port " << ipConf.receiver.port << endl;
    //     cout << "Frequency: " << ipConf.frequency << endl;
    // }
    // else cout << "No file for device found" << endl;

    
    boost::asio::io_context io_context;
    Core::RtpsParticipant partnt1(io_context, 0, 1);
    Core::RtpsParticipant partnt2(io_context, 0, 2);

    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> guard 
            = boost::asio::make_work_guard(io_context);

    boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
    signals.async_wait([&](const boost::system::error_code&, int){
        std::cout << "\nSignal received, shutting down...\n";
        io_context.stop();
    });
    
    cout << "Startin UdpTransport part" << endl;

    shared_ptr<Core::UdpTransport> udp1 = make_shared<Core::UdpTransport>(io_context, 56789);
    shared_ptr<Core::UdpTransport> udp2 = make_shared<Core::UdpTransport>(io_context, 56788);

    
    string str1 = "Message from first socket";
    string str2 = "Message from second socket";
    auto multi_callback = [](const boost::asio::ip::udp::endpoint& ep, const std::vector<uint8_t>& data)
    {
        std::cout << "Received from: " << ep.address().to_string() << " port " << ep.port() << " : ";
        for (auto v : data)
            cout << v << " ";
        cout << endl;
    };
    udp1->SetMulticastHandle(multi_callback);
    udp2->SetMulticastHandle(multi_callback);

    std::thread th1([&]()
    {
        io_context.run();
    });
    
    if (partnt1.JoinMulticastGroup("239.100.0.1"));
    // partnt1.SchedulePeriodicSpdpMessages();
    if (partnt2.JoinMulticastGroup("239.100.0.1"));
        // partnt2.SchedulePeriodicSpdpMessages();
    
    auto buffer = std::vector<uint8_t>{'R','T','P','S','P','a','r','t'};
    
    // udp1->JoinMulticastGroup("239.100.0.1");
    // udp2->JoinMulticastGroup("239.100.0.1");
    // auto buf1 = std::vector<uint8_t>(str1.begin(), str1.end());
    // auto buf2 = std::vector<uint8_t>(str2.begin(), str2.end());
    
    // udp1->SendMulticast(buf1);
    // udp2->SendMulticast(buf2);
    partnt1.SendMulticast(buffer);
    partnt2.SendMulticast(buffer);
    partnt2.SchedulePeriodicSpdpMessages();

    // cout << "Before join" << endl;
    th1.join();
    udp1->StopReceiving();
    udp2->StopReceiving();
    partnt1.Shutdown();
    partnt2.Shutdown();
    guard.reset();

    return 0;
}
