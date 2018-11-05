//
// cloriConf json parser implementation 
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//
#include <fstream>
#include <stdio.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include "json.h"

#define EMPTY_VALUE ""

namespace cloris {

using namespace rapidjson;

static bool ParsePlainObjectNode(const Value& obj, std::vector<TraceNode>& vec_trace, 
        const ConfigInserter& handler, 
        std::string* err_msg) {
    std::string full_path = vec2str(vec_trace, "/");
    if (obj.IsString()) {
        return handler(full_path, obj.GetString(), err_msg);
    } else if (obj.IsNumber()) {
        if (obj.IsInt64()) {
            return handler(full_path, std::to_string(obj.GetInt64()), err_msg);
        } else { 
            return handler(full_path, std::to_string(obj.GetDouble()), err_msg);
        }
    } else if (obj.IsBool()) {
        return handler(full_path, obj.GetBool() ? "true" : "false", err_msg);
    } else if (obj.IsNull()) {
        return handler(full_path, "", err_msg);
    } else {
        if (err_msg) {
            *err_msg = "json parser internal error";
        }
        return false;
    }
}

static bool ParseMemberNode(const std::string& key, const Value& value, std::vector<TraceNode>& vec_trace, 
        const ConfigInserter& handler, std::string* err_msg) {
    bool ok;
    vec_trace.push_back(TraceNode(key, 0));
    if (value.IsObject()) {
        // empty object '{ }'
        if (value.MemberBegin() == value.MemberEnd()) {
            std::string full_path = vec2str(vec_trace, "/");
            ok = handler(full_path, EMPTY_VALUE, err_msg);
        } else {
            for (Value::ConstMemberIterator in_iter = value.MemberBegin(); in_iter != value.MemberEnd(); ++in_iter) {
                ok = ParseMemberNode(in_iter->name.GetString(), in_iter->value, vec_trace, handler, err_msg);
                if (!ok) {
                    break;
                }
            }
        }
    } else if (value.IsArray()) {
        // empty array '[ ]'
        if (value.Begin() == value.End()) {
            std::string full_path = vec2str(vec_trace, "/");
            ok = handler(full_path, EMPTY_VALUE, err_msg);
        } else {
            int i = 0;
            for (Value::ConstValueIterator in_iter = value.Begin(); in_iter != value.End(); ++in_iter) {
                // p1/p2/0/...
                // p1/p2/1/...
                std::string mkey = std::to_string(i++);
                ok = ParseMemberNode(mkey, *in_iter, vec_trace, handler, err_msg);
                if (!ok) {
                    break;
                }
            }
        }
    } else {
        ok = ParsePlainObjectNode(value, vec_trace, handler, err_msg);
    }
    vec_trace.pop_back();
    return ok;
}

bool ParseJsonConfig(const std::string& input, bool is_file, const ConfigInserter& handler, std::string* err_msg) {
    Document doc;
    if (is_file) {
        std::ifstream file(input.c_str());
        FILE* fp = fopen(input.c_str(), "r");
        char buffer[65536];
        FileReadStream stream(fp, buffer, sizeof(buffer));
        doc.ParseStream(stream);
    } else {
        doc.Parse<0>(input.c_str());
    }
    if (doc.HasParseError()) {
        if (err_msg) {
            *err_msg = GetParseError_En(doc.GetParseError());
        }
        return false;
    }

    std::vector<TraceNode> vec_trace;
    if (doc.IsObject()) {
        for (Value::ConstMemberIterator iter = doc.MemberBegin(); iter != doc.MemberEnd(); ++iter) {
            if (!ParseMemberNode(iter->name.GetString(), iter->value, vec_trace, handler, err_msg)) {
                return false;
            }
        }
        return true;
    } else if (doc.IsArray()) {
        int i = 0;
        for (Value::ConstValueIterator in_iter = doc.Begin(); in_iter != doc.End(); ++in_iter) {
            // p1/p2/0/...
            // p1/p2/1/...
            std::string mkey = std::to_string(i++);
            if (!ParseMemberNode(mkey, *in_iter, vec_trace, handler, err_msg)) {
                return false;
            }
        }
        return true;
    } else {
        if (err_msg) {
            *err_msg = "config is not object or array";
        }
        return false;
    }
}

} // namespace cloris
