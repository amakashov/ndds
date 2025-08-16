#include <iostream>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <fstream>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>

#include "utils/configparser.hpp"
#include "core/rtpsparticipant.hpp"
#include <boost/asio/executor_work_guard.hpp>

using namespace nlohmann;
using namespace std;


int main(int, char**)
{
    cout << "Node sample started" << endl;
    auto j3 = json::parse(R"({"happy": true, "pi": 3.141, "arr" : [1,2,3,5], "obj" : {"first" :1, "second" : 2}})");
    cout << j3 << endl;
    for (auto & a : j3)
        cout << a << " is object? "<< a.is_object() <<  endl;

    unordered_map<string, double> freqs {{"begin", -1.1}, {"end", 12.05}, {"middle", -2.3}};
    json j_umap(freqs);
    j3["coords"] = j_umap;
    j3["type"] = "serial";

    ofstream of("test.json");
    if (of.is_open())
        of << j3 << endl;
    of.close();

    std::ifstream ifs("test.json");
    if (ifs.is_open())
    {
        ConfigParser parser(ifs);
        parser.ParseStream();
    }
    ifs.close();

    std::string serialized = j3.dump();
    cout << "Serialized string" << serialized << endl;
    stringstream istrm(serialized);

    if (istrm.good())
    {
        ConfigParser parser(istrm);
        parser.ParseStream();        
    }

    ifs.open("device.json");
    if (ifs.is_open())
    {
        ConfigParser parser(ifs);
        parser.ParseStream();
        auto devConf = parser.getSubConfig("test_device");
        if (!ConfigParser::checkSanity(devConf))
        {
            cout << "Insane config " << devConf << endl;
        }
        auto ipConf = ConfigParser::getIpCofig(devConf);
        cout << "Sender: " << ipConf.sender.address << " port " << ipConf.sender.port << endl;
        cout << "Receiver: " << ipConf.receiver.address << " port " << ipConf.receiver.port << endl;
        cout << "Frequency: " << ipConf.frequency << endl;
    }
    else cout << "No file for device found" << endl;

    
    boost::asio::io_context io_context;
    // Core::RtpsParticipant partnt(io_context, 1);
    
    cout << "Startin UdpTransport part" << endl;

    Core::UdpTransport udp1(io_context, 56789);
    Core::UdpTransport udp2(io_context, 56788);
    udp1.JoinMulticastGroup("239.100.0.1");
    udp2.JoinMulticastGroup("239.100.0.1");
    string str1 = "Message from first socket";
    string str2 = "Message from second socket";
    auto multi_callback = [](const boost::asio::ip::udp::endpoint& ep, const std::vector<uint8_t>& data)
    {
        std::cout << "Received from: " << ep.address().to_string() << " port " << ep.port() << " : ";
        for (auto v : data)
            cout << v << " ";
        cout << endl;
    };
    udp1.SetMulticastHandle(multi_callback);
    udp2.SetMulticastHandle(multi_callback);
    auto buf1 = std::vector<uint8_t>(str1.begin(), str1.end());
    auto buf2 = std::vector<uint8_t>(str2.begin(), str2.end());

    udp1.SendMulticast(buf1);
    udp2.SendMulticast(buf2);

    std::thread th1([&]()
    {
        boost::asio::executor_work_guard<boost::asio::io_context::executor_type> guard 
            = boost::asio::make_work_guard(io_context);

        io_context.run();
    });
    th1.join();

    return 0;
}
