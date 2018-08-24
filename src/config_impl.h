// cloriConf -- a flexisible config service and service found 
// version: 1.0 
// Copyright 2018 James Wei (weijianlhp@163.com)

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

struct HNode {
    HNode() : value_(NULL), version_(0), enabled_(true) { }
    HNode(void* val) : value_(val), version_(0), enabled_(true) { }
    HNode(void* val, int64_t version) : value_(val), version_(version), enabled_(true) { }

    void *value_;
    int64_t version_;
    bool enabled_;
    void disable();
};

struct WNode {
    WNode(const std::string& cpath, EventHandler& handler) : cpath_(cpath), handler_(handler) { }
    std::string cpath_;
    EventHandler handler_;
};

class ConfigImpl {
public:
    ConfigImpl();
    ~ConfigImpl();
    void DisableDeletedNode(std::set<std::string>& nset);
    bool RegisterWatcher(const std::string& cpath, uint32_t event, EventHandler& handler); 
    
    bool CheckIfNotExistOrExpired(const std::string& cpath, int64_t version = 0, bool* noexist = NULL);
    bool Load(const std::string& src, SP_MODE mode, std::string& err_msg);
    bool Insert(const std::string& cpath, const std::string& value, std::string& err_msg, int64_t version = 0);

    void NodifyWatcher(const std::string& cpath); 
    void FlushWatcher();
    CNode* getCNode(const std::string& key);
    CNode* getCNode(const std::string& key_prefix, const std::string& key);
    size_t count() { return htable_.size(); }
private:
    std::unordered_map<std::string, HNode*> htable_;
    std::unordered_map<std::string, WNode*> wtable_;
    std::set<std::string> trigger_;
    std::unique_ptr<ConfigKeeper> ck_;
    CNode root_;
};

} // namespace cloris

#endif // CLORIS_CONFIG_IMPL_H_
