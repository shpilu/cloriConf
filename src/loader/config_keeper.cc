//
// config keeper
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//
#include <string>
#include <vector>
#include <functional>
#include <boost/noncopyable.hpp>
#include "config_keeper.h"

namespace cloris {

std::string vec2str(const std::vector<TraceNode>& vec, const std::string& separator) {
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

} // namespace cloris 

