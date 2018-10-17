// cloriConf cpp header
// version: 1.0
// description: direct config implementation
// Copyright 2018 James Wei (weijianlhp@163.com)

#ifndef CLORIS_CONFIG_DIRECT_H_
#define CLORIS_CONFIG_DIRECT_H_

#include "../../config_keeper.h"

namespace cloris {

class ConfigKeeperDirect : public ConfigKeeper {
public:
    ConfigKeeperDirect() = delete;
    ConfigKeeperDirect(ConfigImpl* impl);
    ~ConfigKeeperDirect() { }
    bool LoadConfig(const std::string& src, int format, std::string* err_msg);
};

} // namespace cloris

#endif // CLORIS_CONFIG_DIRECT_H_
