//
// cloriConf implementation header 
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//
#ifndef CLORIS_CONFIG_IMPL_H_
#define CLORIS_CONFIG_IMPL_H_

#include <map>
#include <set>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "config.h"

namespace cloris {

class Config;
class ConfigKeeper;

// use hash table to accelerate cnode search 
struct HashNode {
    HashNode() : value_(NULL), version_(0), enabled_(true) { }
    HashNode(void* val) : value_(val), version_(0), enabled_(true) { }
    HashNode(void* val, int64_t version) : value_(val), version_(version), enabled_(true) { }

    void *value_;
    int64_t version_;
    bool enabled_;
    void Disable();
};

// watch node
struct WatchNode {
    WatchNode(const std::string& node_path, EventHandler& handler) : node_path_(node_path), handler_(handler) { }
    std::string node_path_;
    EventHandler handler_;
};

class ConfigImpl {
public:
    ConfigImpl();
    ~ConfigImpl();
    void DisableDeletedNode(const std::set<std::string>& node_set);
    bool RegisterWatcher(const std::string& node_path, uint32_t event, EventHandler& handler); 
    
    bool CheckIfNotExistOrExpired(const std::string& node_path, int64_t version = 0, bool* no_exist = NULL);
    bool Load(const std::string& src, int mode, std::string* err_msg);
    bool Insert(const std::string& node_path, const std::string& value, std::string* err_msg, int64_t version = 0);

    void NodifyWatcher(const std::string& node_path); 
    void FlushWatcher();
    ConfNode* GetConfNode(const std::string& key);
    ConfNode* GetConfNode(const std::string& key_prefix, const std::string& key);
    size_t count() const { return hash_table_.size(); }
private:
    void Flush();
    std::unordered_map<std::string, HashNode*> hash_table_;
    std::unordered_map<std::string, WatchNode*> watch_table_;
    std::set<std::string> trigger_;
    std::unique_ptr<ConfigKeeper> config_keeper_;
    ConfNode* root_;
};

} // namespace cloris

#endif // CLORIS_CONFIG_IMPL_H_
