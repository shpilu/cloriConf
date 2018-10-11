// cloriConf cpp header
// Copyright 2018 James Wei (weijianlhp@163.com)
// version: 1.0
// description: direct config implementation, use JOML format

#include <vector>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include "../../config_impl.h"
#include "config_direct.h"

namespace cloris {

using namespace std::placeholders;

static bool isLegal(const std::string& str, std::string& err_msg) { 
    return true; 
}

bool read_line(const std::string& raw_conf, std::string& buf, size_t& index) {
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

static std::string vec2str(const std::vector<TraceNode>& vec, const std::string& separator) {
    if (vec.size() == 0) {
        return ""; 
    } else if (vec.size() == 1) {
        return vec[0].id;
    } else {
        std::string res(vec[0].id);
        for (size_t i = 1; i < vec.size(); ++i) {
            res += separator + vec[i].id;
        }
        return res;
    }
}

static bool purgeLine(std::string& line, std::string& err_msg, std::vector<std::string>& comments) {
    for (auto &p : comments) {
        line = line.substr(0, line.find(p));
        boost::trim(line);
    }
    // TODO check unsupported chars 
    if (!isLegal(line, err_msg)) {
        return false;
    }
    return true;
}

static bool insertConfNode(const std::string& key, 
        const std::string& value, 
        std::vector<TraceNode>& vec_trace, 
        const ConfigInserter& handler,
        int current_weight,
        std::string& err_msg) {
    size_t separator_pos = key.find(".");
    if ((separator_pos == 0) || (separator_pos == key.size() - 1)) {
        // item like ".xxx" "xxx." is not supported
        err_msg = "bad key, '.'format error";
        return false;
    }
    std::string first_key = key.substr(0, separator_pos);
    bool is_last_key((separator_pos == std::string::npos) ? true : false);
    bool is_leaf(is_last_key && (current_weight == 0));
    std::string node_value(is_leaf ? value : "");

    // special case like: key1. key2 . key3 = v1, treated as key1.key2.key3=v1
    boost::trim(first_key);
    if (first_key.empty()) {
        err_msg = "empty key";
        return false;
    }

    vec_trace.push_back(TraceNode(first_key, current_weight));
    std::string cpath = vec2str(vec_trace, "/");
    if (!handler(cpath, node_value, err_msg)) {
        return false;
    }

    if (is_last_key) {
        return true;
    } else {
        std::string pending_key = key.substr(separator_pos + 1);
        return insertConfNode(pending_key, value, vec_trace, handler, current_weight, err_msg);
    }
}

static void traceBack(std::vector<TraceNode>& vec_trace, int current_weight) {
    while (!vec_trace.empty() && (vec_trace.back().weight <= current_weight)) {
        vec_trace.pop_back();
    }
}

static bool parseKeyLine(const std::string& buf, std::vector<TraceNode>& vec_trace, const ConfigInserter& handler, 
        int current_weight, std::string& err_msg) {
    traceBack(vec_trace, current_weight); 
    size_t eq_pos = buf.find('=');
    std::string key(buf.substr(0, eq_pos));        
    std::string value((eq_pos == std::string::npos) ? "" : buf.substr(eq_pos + 1));

    boost::trim(value);

    return insertConfNode(key, value, vec_trace, handler, current_weight, err_msg);
}

static bool parseSectionLine(const std::string& buf, std::vector<TraceNode>& vec_trace, 
        const ConfigInserter& handler, std::string& err_msg) {
    int i(0);
    int j(buf.size() - 1); 

    while (i < j) {
        if ((buf[i] == '[') && (buf[j] == ']')) {
            ++i;
            --j;
        } else if ((buf[i] != '[') && (buf[j] != ']')) {
            break;
        } else {
            err_msg = "bad section format";
            return false;
        }
    }
    
    std::string key = buf.substr(i, j - i + 1);
    boost::trim(key);

    // support '[[ ]]' --- separate sections
    if (key.empty()) {
        traceBack(vec_trace, i);
        return true;
    } else {
        return parseKeyLine(key, vec_trace, handler, i, err_msg);
    }
}

static bool parseLine(const std::string& buf, std::vector<TraceNode>& vec_trace, const ConfigInserter& handler, std::string& err_msg) {
    if (buf.empty()) {
        return true;
    } else if (buf[0] == '[') {
        return parseSectionLine(buf, vec_trace, handler, err_msg);
    } else {
        return parseKeyLine(buf, vec_trace, handler, 0, err_msg);
    }
}

ConfigKeeperDirect::ConfigKeeperDirect(ConfigImpl* impl) 
    : ConfigKeeper(impl) {
}

// Config::instance()->Load("xadsad", "#|;|//");
bool ConfigKeeperDirect::LoadConfig(const std::string& raw_conf, std::string& err_msg) {
    bool ret(true);
    std::vector<std::string> comments = {"#"};
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
        err_msg = "[parse_config_failed]" + err_msg;
    }
    impl()->FlushWatcher();
    return ret;
}

} // namespace cloris
