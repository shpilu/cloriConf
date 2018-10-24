//
// cloriConf joml parser implementation 
// Copyright 2018 James Wei (weijianlhp@163.com)
// version: 1.0
//

#include <boost/algorithm/string.hpp>
#include "../config_impl.h"
#include "joml.h"

namespace cloris {

using namespace std::placeholders;

static bool IsLegal(const std::string& str, std::string* err_msg) { 
    return true; 
}

void LoadCommentChars(int format, std::vector<std::string>& comments) {
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
}

bool PurgeLine(std::string& line, std::string* err_msg, std::vector<std::string>& comments) {
    for (auto &p : comments) {
        line = line.substr(0, line.find(p));
        boost::trim(line);
    }
    // TODO check unsupported chars 
    if (!IsLegal(line, err_msg)) {
        return false;
    }
    if (comments.size() == 0) {
        boost::trim(line);
    }
    return true;
}

static bool InsertConfNode(const std::string& key, 
        const std::string& value, 
        std::vector<TraceNode>& vec_trace, 
        const ConfigInserter& handler,
        int current_weight,
        std::string* err_msg) {
    size_t separator_pos = key.find(".");
    if ((separator_pos == 0) || (separator_pos == key.size() - 1)) {
        // item like ".xxx" "xxx." is not supported
        if (err_msg) {
            *err_msg = "bad key, '.'format error";
        }
        return false;
    }
    std::string first_key = key.substr(0, separator_pos);
    bool is_last_key((separator_pos == std::string::npos) ? true : false);
    bool is_leaf(is_last_key && (current_weight == 0));
    std::string node_value(is_leaf ? value : "");

    // special case like: key1. key2 . key3 = v1, treated as key1.key2.key3=v1
    boost::trim(first_key);
    if (first_key.empty()) {
        if (err_msg) {
            *err_msg = "empty key";
        }
        return false;
    }

    vec_trace.push_back(TraceNode(first_key, current_weight));
    std::string full_path = vec2str(vec_trace, "/");
    if (!handler(full_path, node_value, err_msg)) {
        return false;
    }

    if (is_last_key) {
        return true;
    } else {
        std::string pending_key = key.substr(separator_pos + 1);
        return InsertConfNode(pending_key, value, vec_trace, handler, current_weight, err_msg);
    }
}

static void TraceBack(std::vector<TraceNode>& vec_trace, int current_weight) {
    while (!vec_trace.empty() && (vec_trace.back().weight <= current_weight)) {
        vec_trace.pop_back();
    }
}

static bool ParseKeyLine(const std::string& buf, std::vector<TraceNode>& vec_trace, const ConfigInserter& handler, 
        int current_weight, std::string* err_msg) {
    TraceBack(vec_trace, current_weight); 
    size_t eq_pos = buf.find('=');
    std::string key(buf.substr(0, eq_pos));        
    std::string value((eq_pos == std::string::npos) ? "" : buf.substr(eq_pos + 1));

    boost::trim(value);

    return InsertConfNode(key, value, vec_trace, handler, current_weight, err_msg);
}

static bool ParseSectionLine(const std::string& buf, std::vector<TraceNode>& vec_trace, 
        const ConfigInserter& handler, std::string* err_msg) {
    int i(0);
    int j(buf.size() - 1); 

    while (i < j) {
        if ((buf[i] == '[') && (buf[j] == ']')) {
            ++i;
            --j;
        } else if ((buf[i] != '[') && (buf[j] != ']')) {
            break;
        } else {
            if (err_msg) {
                *err_msg = "bad section format:" + buf;
            }
            return false;
        }
    }
    
    std::string key = buf.substr(i, j - i + 1);
    boost::trim(key);

    // support '[[ ]]' --- separate sections
    if (key.empty()) {
        TraceBack(vec_trace, i);
        return true;
    } else {
        return ParseKeyLine(key, vec_trace, handler, i, err_msg);
    }
}

bool ParseLine(const std::string& buf, std::vector<TraceNode>& vec_trace, const ConfigInserter& handler, std::string* err_msg) {
    if (buf.empty()) {
        return true;
    } else if (buf[0] == '[') {
        return ParseSectionLine(buf, vec_trace, handler, err_msg);
    } else {
        return ParseKeyLine(buf, vec_trace, handler, 0, err_msg);
    }
}

} // namespace cloris
