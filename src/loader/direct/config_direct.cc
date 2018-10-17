// cloriConf cpp header
// Copyright 2018 James Wei (weijianlhp@163.com)
// version: 1.0
// description: direct config implementation, use JOML format

#include <vector>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include "../joml.h"
#include "../../config_impl.h"
#include "config_direct.h"

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

// cloriConf just support JINI config format only now
bool ConfigKeeperDirect::LoadConfig(const std::string& raw_conf, int format, std::string* err_msg) {
    bool ret(true);
    std::vector<std::string> comments;
    int comment_flag = format & CMT_MASK;
    if (comment_flag & CMT_SHARP) {
        comments.push_back("#");
    }
    if (comment_flag & CMT_SLASH) {
        comments.push_back("//");
    }
    if (comment_flag & CMT_SEMICOLON) {
        comments.push_back(";");
    }
    if (comment_flag & CMT_PERCENT) {
        comments.push_back("%");
    }

    std::string buf;
    std::vector<TraceNode> vec_trace;

    ConfigInserter handler = std::bind(&ConfigImpl::Insert, this->impl(), _1, _2, _3, 0);
    size_t index = 0;
    while (read_line(raw_conf, buf, index)) {
        if (!purgeLine(buf, err_msg, comments) || !parseLine(buf, vec_trace, handler, err_msg)) {
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
