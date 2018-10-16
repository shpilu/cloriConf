// 
// cloriConf -- a flexisible config service and service found 
// version: 1.0 
// Copyright 2018 James Wei (weijianlhp@163.com)
//

#ifndef CLORIS_CONFIG_H_
#define CLORIS_CONFIG_H_

#include <stdlib.h>
#include <pthread.h>
#include <map>
#include <functional>
#include <boost/noncopyable.hpp>
#include "config_basic.h"

namespace cloris {

class CNode;
class ConfigImpl;

typedef std::function<void(CNode*, const std::string&, uint32_t)> EventHandler;

enum ITYPE {
    ITYPE_ALL = 0,
    ITYPE_LEAF  = 1,
    ITYPE_NONLEAF = 2,
};

class CNodeIterator {
    friend class CNode;
public:
    CNodeIterator& operator++();
    CNode& operator*();
    CNode* operator->();
    bool   operator!=(const CNodeIterator& iter);
    const std::map<std::string, CNode*>::iterator& current() const { return current_; }
private:
    explicit CNodeIterator(const std::map<std::string, CNode*>::iterator& begin, 
                const std::map<std::string, CNode*>::iterator& end, uint8_t mode);
    std::map<std::string, CNode*>::iterator current_;
    std::map<std::string, CNode*>::iterator end_;
    uint8_t mode_;
};

// config node 
class CNode {
    friend class CNodeIterator;
public:
    CNode() = delete;
    CNode(ConfigImpl* impl, const std::string& key, const std::string& hkey, const std::string& value, bool is_leaf); 

    CNodeIterator begin();
    CNodeIterator end();
    CNodeIterator leaf_begin();
    CNodeIterator leaf_end();
    CNodeIterator nonleaf_begin();
    CNodeIterator nonleaf_end();
public:
    inline void refresh();
    const std::string& asString();
    int32_t     asInt32();
    int64_t     asInt64();
    double      asDouble();
    bool        asBool();

    std::string getString(const std::string& key, const std::string& def_val = "");
    int32_t     getInt32(const std::string& key, int32_t def_val = 0);
    int64_t     getInt64(const std::string& key, int64_t def_val = 0L);
    double      getDouble(const std::string& key, double def_val = 0.0);
    bool        getBool(const std::string& key, bool def_val = false);

    std::map<std::string, CNode*>& children() { 
        return children_; 
    }
    DoubleBuffer<std::string>& value() { 
        return value_; 
    } 

    CNode* getCNode(const std::string& key = ""); 
    void disable() { enabled_ = false; }
    void set_is_leaf(bool is_leaf) { is_leaf_ = is_leaf; }

    bool is_leaf() const { return is_leaf_; }
    const std::string& key() const { return key_; }
    bool enabled() const  { return enabled_; }
    
private:
    ConfigImpl *impl_;
    std::map<std::string, CNode*> children_;
    std::string key_;
    std::string hkey_;
    DoubleBuffer<std::string> value_;
    bool enabled_;
    bool is_leaf_;
};

class Config : boost::noncopyable {
public:
    static Config* instance();
    Config(const std::string& src, int mode);
    Config();
    ~Config();

    Config* Load(const std::string& src, int mode, std::string* err_msg = NULL);
    bool Watch(const std::string& path, uint32_t event, EventHandler& handler);

    CNode*      getCNode(const std::string& key = "");
    std::string getString(const std::string& key, const std::string& def_val = "");
    int32_t     getInt32(const std::string& key, int32_t def_val = 0);
    int64_t     getInt64(const std::string& key, int64_t def_val = 0L);
    double      getDouble(const std::string& key, double def_val = 0.0);
    bool        getBool(const std::string& key, bool def_val = false);

    bool ok() { return (status_ == 0); }
    const std::string& error() const { return last_error_; }
private:
    int status_;
    std::string last_error_;

    ConfigImpl* impl_;
};

inline void CNode::refresh() { 
    enabled_ = true; 
    value_.Set("");
}

} // namespace cloris

#endif // CLORIS_CONFIG_H_
