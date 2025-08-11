#include <iostream>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <fstream>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>

#include "utils/configparser.hpp"

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

    return 0;
}
