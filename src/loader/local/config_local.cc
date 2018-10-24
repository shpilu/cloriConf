// cloriConf cpp header
// Copyright 2018 James Wei (weijianlhp@163.com)
// version: 1.0
// description: local file implementation

#include <vector>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include "../joml.h"
#include "../json.h"
#include "../../config_impl.h"
#include "config_local.h"

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
    ConfigInserter handler = std::bind(&ConfigImpl::Insert, this->impl(), _1, _2, _3, 0);
    return ParseJsonConfig(filename, true, handler, err_msg);
}

bool ConfigKeeperLocal::LoadJoml(const std::string& filename, int format, std::string* err_msg) {
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

    std::ifstream fl(filename);
    std::string buf;
    std::vector<TraceNode> vec_trace;
    
    if (!fl.is_open()) {
        *err_msg = "failed to open file " + filename;
        return false;
    }

    ConfigInserter handler = std::bind(&ConfigImpl::Insert, this->impl(), _1, _2, _3, 0);
    while (getline(fl, buf)) {
        if (!purgeLine(buf, err_msg, comments) || !parseLine(buf, vec_trace, handler, err_msg)) {
            ret = false;
            break;
        }
    }
    fl.close();

    if (!ret) {
        *err_msg = "[parse_config_failed]" + (*err_msg);
    }
    impl()->FlushWatcher();
    return ret;
}

} // namespace cloris
