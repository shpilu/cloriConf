// 
// cloriConf header
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

enum IterType {
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
    ~CNode();
    CNode(ConfigImpl* impl, const std::string& path, const std::string& hash_key, const std::string& value, bool is_leaf); 

    CNodeIterator begin();
    CNodeIterator end();
    CNodeIterator leaf_begin();
    CNodeIterator leaf_end();
    CNodeIterator non_leaf_begin();
    CNodeIterator non_leaf_end();
public:
    const std::string& AsString() const;
    int32_t     AsInt32() const;
    int64_t     AsInt64() const;
    double      AsDouble() const;
    bool        AsBool() const;

    const std::string GetString(const std::string& key, const std::string& def_val = "") const;
    int32_t     GetInt32(const std::string& key, int32_t def_val = 0) const;
    int64_t     GetInt64(const std::string& key, int64_t def_val = 0L) const;
    double      GetDouble(const std::string& key, double def_val = 0.0) const;
    bool        GetBool(const std::string& key, bool def_val = false) const;
    const CNode* GetCNode(const std::string& key = "") const; 

    std::map<std::string, CNode*>& children() { return children_; }
    const std::string& key() const { return key_; }
    void Disable() { enabled_ = false; }
    void Refresh();
    bool enabled() const  { return enabled_; }
    void set_is_leaf(bool is_leaf) { is_leaf_ = is_leaf; }
    const DoubleBuffer<std::string>& value() const { return value_; } 
    DoubleBuffer<std::string>& mutable_value() { return value_; } 

private:
    bool is_leaf() const { return is_leaf_; }

    std::map<std::string, CNode*> children_;
    DoubleBuffer<std::string> value_;
    ConfigImpl *impl_;
    std::string key_;
    std::string hkey_;
    bool enabled_;
    bool is_leaf_;
};

class Config : boost::noncopyable {
public:
    static Config* instance();
    Config(const std::string& input, int mode);
    Config();
    ~Config();

    Config* Load(const std::string& input, int mode, std::string* err_msg = NULL);
    bool Watch(const std::string& path, uint32_t event, EventHandler& handler);

    const CNode*      GetCNode(const std::string& key = "") const;
    std::string GetString(const std::string& key, const std::string& def_val = "") const;
    int32_t     GetInt32(const std::string& key, int32_t def_val = 0) const;
    int64_t     GetInt64(const std::string& key, int64_t def_val = 0L) const;
    double      GetDouble(const std::string& key, double def_val = 0.0) const;
    bool        GetBool(const std::string& key, bool def_val = false) const;

    bool Ok() { return (status_ == 0); }
    const std::string& ErrorText() const { return last_error_; }
private:
    int status_;
    std::string last_error_;
    ConfigImpl* impl_;
};


} // namespace cloris

#endif // CLORIS_CONFIG_H_
