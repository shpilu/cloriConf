//
// base class of configuration keeper  
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//
#ifndef CLORIS_CONFIG_KEEPER_H_
#define CLORIS_CONFIG_KEEPER_H_

#include <string>
#include <vector>
#include <functional>
#include <boost/noncopyable.hpp>

namespace cloris {

typedef std::function<bool(const std::string&, const std::string&, std::string*)> ConfigInserter;
class ConfigImpl;

class ConfigKeeper : boost::noncopyable {
public:
    ConfigKeeper(ConfigImpl* impl) : impl_(impl) { }
    virtual ~ConfigKeeper() { }
    virtual bool LoadConfig(const std::string& src, int format, std::string* err_msg = NULL) = 0; 

    ConfigImpl* impl() { return impl_; }
private:
    ConfigImpl* impl_;
};

struct TraceNode {
    TraceNode(const std::string& s_id, int i_weight) 
        : id(s_id),
           weight(i_weight) { }
    std::string id;
    int weight;
};

std::string vec2str(const std::vector<TraceNode>& vec, const std::string& separator); 

} // namespace cloris 

#endif //  CLORIS_CONFIG_KEEPER_H_
