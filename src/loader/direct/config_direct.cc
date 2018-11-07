//
// direct config implementation, use JOML format
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//
#include <vector>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include "../joml.h"
#include "../../internal/def.h"
#include "../../config_impl.h"
#include "config_direct.h"

#ifdef ENABLE_JSON
    #include "../json.h"
#endif

namespace cloris {

using namespace std::placeholders;

static bool read_line(const std::string& raw_conf, std::string& buf, size_t& index) {
    if (index >= raw_conf.size()) {
        return false;
    }
    size_t pos = raw_conf.find('\n', index);
    if (pos == std::string::npos) {
        pos = raw_conf.size() - 1;
    }
    buf = raw_conf.substr(index, pos - index + 1);
    index = pos + 1; 
    return true;
}

ConfigKeeperDirect::ConfigKeeperDirect(ConfigImpl* impl) 
    : ConfigKeeper(impl) {
}

bool ConfigKeeperDirect::LoadJson(const std::string& raw_conf, std::string* err_msg) {
#ifdef ENABLE_JSON
    ConfigInserter handler = std::bind(&ConfigImpl::Insert, this->impl(), _1, _2, _3, 0);
    return ParseJsonConfig(raw_conf, false, handler, err_msg);
#else
    if (err_msg) {
        *err_msg = "json format is not support yet, please recompile cloriConf with ENABLE_JSON=ON";
    }
    return false;
#endif
}

bool ConfigKeeperDirect::LoadConfig(const std::string& raw_conf, int format, std::string* err_msg) {
    if (format & FMT_JSON) {
        return this->LoadJson(raw_conf, err_msg);
    } else {
        // use joml as default data format
        return this->LoadJoml(raw_conf, format, err_msg);
    }
}

// cloriConf just support JINI config format only now
bool ConfigKeeperDirect::LoadJoml(const std::string& raw_conf, int format, std::string* err_msg) {
    bool ret(true);
    std::string buf;
    std::vector<TraceNode> vec_trace;

    std::vector<std::string> comments;
    LoadCommentChars(format, comments);

    ConfigInserter handler = std::bind(&ConfigImpl::Insert, this->impl(), _1, _2, _3, 0);
    size_t index = 0;
    while (read_line(raw_conf, buf, index)) {
        if (!PurgeLine(buf, err_msg, comments) || !ParseLine(buf, vec_trace, handler, err_msg)) {
            ret = false;
            break;
        }
    }
    if (!ret) {
        if (err_msg) {
            *err_msg = "[parse_config_failed]" + (*err_msg);
        }
    }
    impl()->FlushWatcher();
    return ret;
}

} // namespace cloris
