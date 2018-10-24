
#ifndef  CLORIS_CONFIG_ZOOKEEPER_H_
#define  CLORIS_CONFIG_ZOOKEEPER_H_

#include <thread>
#include <zookeeper/zookeeper.h>
#include "../config_keeper.h"

namespace cloris {

class ConfigImpl;

class ConfigZookeeper : public ConfigKeeper {
public:
    ConfigZookeeper(ConfigImpl* impl);
    ~ConfigZookeeper();
    bool LoadConfig(const std::string& conf_file, int format, std::string* err_msg);

private:
    ConfigZookeeper() = delete;
    void Start();
    void Stop();
    void Job();
    bool ScanZk(std::string* err_msg);
    bool ConnectZk(std::string* err_msg);
    void CloseZk();
    bool SyncConfigFromZk(std::string* err_msg);

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

