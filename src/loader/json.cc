//
// cloriConf json parser implementation 
// Copyright 2018 James Wei (weijianlhp@163.com)
// version: 1.0
//

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include "json.h"

#define EMPTY_VALUE ""

namespace cloris {

using namespace rapidjson;

static bool parsePlainObjectNode(const Value& obj, std::vector<TraceNode>& vec_trace, 
        const ConfigInserter& handler, 
        std::string* err_msg) {
    std::string cpath = vec2str(vec_trace, "/");
    if (obj.IsString()) {
        return handler(cpath, obj.GetString(), err_msg);
    } else if (obj.IsNumber()) {
        if (obj.IsInt64()) {
            return handler(cpath, std::to_string(obj.GetInt64()), err_msg);
        } else { 
            return handler(cpath, std::to_string(obj.GetDouble()), err_msg);
        }
    } else if (obj.IsBool()) {
        return handler(cpath, obj.GetBool() ? "true" : "false", err_msg);
    } else if (obj.IsNull()) {
        return handler(cpath, "", err_msg);
    } else {
        if (err_msg) {
            *err_msg = "json parser internal error";
        }
        return false;
    }
}

static bool parseMemberNode(const std::string& key, const Value& value, std::vector<TraceNode>& vec_trace, 
        const ConfigInserter& handler, std::string* err_msg) {
    bool ok;
    vec_trace.push_back(TraceNode(key, 0));
    if (value.IsObject()) {
        // empty object '{ }'
        if (value.MemberBegin() == value.MemberEnd()) {
            std::string cpath = vec2str(vec_trace, "/");
            ok = handler(cpath, EMPTY_VALUE, err_msg);
        } else {
            for (Value::ConstMemberIterator in_iter = value.MemberBegin(); in_iter != value.MemberEnd(); ++in_iter) {
                ok = parseMemberNode(in_iter->name.GetString(), in_iter->value, vec_trace, handler, err_msg);
                if (!ok) {
                    break;
                }
            }
        }
    } else if (value.IsArray()) {
        // empty array '[ ]'
        if (value.Begin() == value.End()) {
            std::string cpath = vec2str(vec_trace, "/");
            ok = handler(cpath, EMPTY_VALUE, err_msg);
        } else {
            int i = 0;
            for (Value::ConstValueIterator in_iter = value.Begin(); in_iter != value.End(); ++in_iter) {
                // p1/p2/0/...
                // p1/p2/1/...
                std::string mkey = std::to_string(i++);
                ok = parseMemberNode(mkey, *in_iter, vec_trace, handler, err_msg);
                if (!ok) {
                    break;
                }
            }
        }
    } else {
        ok = parsePlainObjectNode(value, vec_trace, handler, err_msg);
    }
    vec_trace.pop_back();
    return ok;
}

bool parseJsonConfig(const std::string& input, const ConfigInserter& handler, std::string* err_msg) {
    Document doc;
    if (doc.Parse<0>(input.c_str()).HasParseError()) {
        if (err_msg) {
            *err_msg = GetParseError_En(doc.GetParseError());
        }
        return false;
    }
    if (!doc.IsObject()) {
        if (err_msg) {
            *err_msg = "whole config is not json object";
        }
        return false;
    }
    std::vector<TraceNode> vec_trace;
    for (Value::ConstMemberIterator iter = doc.MemberBegin(); iter != doc.MemberEnd(); ++iter) {
        if (!parseMemberNode(iter->name.GetString(), iter->value, vec_trace, handler, err_msg)) {
            return false;
        }
    }
    return true;
}

} // namespace cloris
