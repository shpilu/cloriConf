// config keeper: base class of config keeper  
// Copyright 2018 James Wei (weijianlhp@163.com)
// version:1.0

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

} // namespace cloris 

#endif //  CLORIS_CONFIG_KEEPER_H_
