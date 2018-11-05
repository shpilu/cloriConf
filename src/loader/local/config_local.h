//
// local configuration file implementation
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//
#ifndef CLORIS_CONFIG_LOCAL_H_
#define CLORIS_CONFIG_LOCAL_H_

#include "../config_keeper.h"

namespace cloris {

class ConfigKeeperLocal : public ConfigKeeper {
public:
    ConfigKeeperLocal(ConfigImpl* impl);
    ~ConfigKeeperLocal() { }
    bool LoadConfig(const std::string& filename, int format, std::string* err_msg);

private:
    ConfigKeeperLocal() = delete;
    bool LoadJoml(const std::string& filename, int format, std::string* err_msg);
    bool LoadJson(const std::string& filename, std::string* err_msg);
};

} // namespace cloris

#endif // CLORIS_CONFIG_LOCAL_H_
