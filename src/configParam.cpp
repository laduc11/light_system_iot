#include <string>
#include <map>

using namespace std;

#define     own_address     0x0001

struct Config
{
public:
    static map<string, size_t>  getConfig() {
        map<string, size_t> config;
        config["own_address"] = own_address;
        config["baud_rate"] = 0x0031;
        // bla bla
        return config;
    }
};

