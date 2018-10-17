// cloriConf cpp header
// Copyright 2018 James Wei (weijianlhp@163.com)
// version: 1.0
// description: local file implementation

#ifndef CLORIS_CONFIG_LOCAL_H_
#define CLORIS_CONFIG_LOCAL_H_

#include "../../config_keeper.h"

namespace cloris {

class ConfigKeeperLocal : public ConfigKeeper {
public:
    ConfigKeeperLocal() = delete;
    ConfigKeeperLocal(ConfigImpl* impl);
    ~ConfigKeeperLocal() { }
    bool LoadConfig(const std::string& src, int format, std::string* err_msg);
};

} // namespace cloris

#endif // CLORIS_CONFIG_LOCAL_H_
