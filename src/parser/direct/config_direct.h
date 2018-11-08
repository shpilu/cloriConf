//
// direct configuration implementation
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//
#ifndef CLORIS_CONFIG_DIRECT_H_
#define CLORIS_CONFIG_DIRECT_H_

#include "../config_keeper.h"

namespace cloris {

class ConfigKeeperDirect : public ConfigKeeper {
public:
    ConfigKeeperDirect(ConfigImpl* impl);
    ~ConfigKeeperDirect() { }
    bool LoadConfig(const std::string& src, int format, std::string* err_msg);

private:
    ConfigKeeperDirect() = delete;
    bool LoadJoml(const std::string& raw_conf, int format, std::string* err_msg);
    bool LoadJson(const std::string& raw_conf, std::string* err_msg);
};

} // namespace cloris

#endif // CLORIS_CONFIG_DIRECT_H_
