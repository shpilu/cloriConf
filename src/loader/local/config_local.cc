//
// local configuration file implementation
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//
#include <vector>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include "../../internal/def.h"
#include "../joml.h"
#include "../../config_impl.h"
#include "config_local.h"

#ifdef ENABLE_JSON
    #include "../json.h"
#endif

namespace cloris {

using namespace std::placeholders;

ConfigKeeperLocal::ConfigKeeperLocal(ConfigImpl* impl) 
    : ConfigKeeper(impl) {
}

bool ConfigKeeperLocal::LoadConfig(const std::string& filename, int format, std::string* err_msg) {
    if (format & FMT_JSON) {
        return this->LoadJson(filename, err_msg);
    } else {
        // use joml as default data format
        return this->LoadJoml(filename, format, err_msg);
    }
}

bool ConfigKeeperLocal::LoadJson(const std::string& filename, std::string* err_msg) {
#ifdef ENABLE_JSON
    ConfigInserter handler = std::bind(&ConfigImpl::Insert, this->impl(), _1, _2, _3, 0);
    return ParseJsonConfig(filename, true, handler, err_msg);
#else 
    if (err_msg) {
        *err_msg = "json format is not support yet, please recompile cloriConf with ENABLE_JSON=ON";
    }
    return false;
#endif
}

bool ConfigKeeperLocal::LoadJoml(const std::string& filename, int format, std::string* err_msg) {
    std::string buf;
    bool ret(true);
    std::ifstream fl(filename);
    std::vector<TraceNode> vec_trace;
    
    if (!fl.is_open()) {
        if (err_msg) {
            *err_msg = "failed to open file " + filename;
        }
        return false;
    }

    std::vector<std::string> comments; 
    LoadCommentChars(format, comments);

    ConfigInserter handler = std::bind(&ConfigImpl::Insert, this->impl(), _1, _2, _3, 0);
    while (getline(fl, buf)) {
        if (!PurgeLine(buf, err_msg, comments) || !ParseLine(buf, vec_trace, handler, err_msg)) {
            ret = false;
            break;
        }
    }
    fl.close();

    if (!ret && err_msg) {
        *err_msg = "[parse_config_failed]" + (*err_msg);
    }
    impl()->FlushWatcher();
    return ret;
}

} // namespace cloris
