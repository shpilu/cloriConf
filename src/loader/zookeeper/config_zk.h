
#ifndef  CLORIS_CONFIG_ZOOKEEPER_H_
#define  CLORIS_CONFIG_ZOOKEEPER_H_

#include <thread>
#include <zookeeper/zookeeper.h>
#include "../config_keeper.h"

namespace cloris {

class ConfigImpl;

class ConfigZookeeper : public ConfigKeeper {
public:
    ConfigZookeeper() = delete;
    ConfigZookeeper(ConfigImpl* impl);
    ~ConfigZookeeper();

    bool LoadConfig(const std::string& src, int format, std::string* err_msg);
    void Start();
    void Stop();
    void Job();

    bool ScanZk(std::string* err_msg);
    bool ConnectZk(std::string* err_msg);
    void CloseZk();
    bool ScanZookeeper(std::string* err_msg);
private:
    zhandle_t* zh_;
    bool running_;
    std::string host_;
    std::string root_;
    int timeout_;
    int interval_;
    std::unique_ptr<std::thread> worker_thread_;
};

} // namespace cloris

#endif // CLORIS_CONFIG_ZOOKEEPER_H_

