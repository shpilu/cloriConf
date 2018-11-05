//
// cloriConf implementation
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//
#include <memory>
#include <boost/algorithm/string.hpp>
#include "loader/zookeeper/config_zk.h"
#include "loader/local/config_local.h"
#include "loader/direct/config_direct.h"
#include "config_impl.h"

namespace cloris {

using namespace std::placeholders;

// hash key standard format "/a1/a2/a3"
static std::string vector_to_hash_key(const std::vector<std::string>& vec) {
    if (vec.size() == 0) {
        return "";
    } else {
        std::string res("");
        for (size_t i = 0; i < vec.size(); ++i) {
            res += "/" + vec[i];
        }
        return res;
    }
}

// try '.' first , then '/'
static std::string convert_to_standard_path(const std::string& key) {
    std::vector<std::string> vec_raw;
    std::vector<std::string> vec_real;
    size_t dot_pos = key.find('.');
    if (dot_pos == std::string::npos) {
        size_t dlim_pos = key.find('/');
        if (dlim_pos == std::string::npos) {
            vec_real.push_back(key);
        } else {
            boost::split(vec_raw, key, boost::is_any_of("/"));
        }
    } else {
        boost::split(vec_raw, key, boost::is_any_of("."));
    }
    for (auto &p : vec_raw) {
        if (p.size() > 0) {
            vec_real.push_back(p);
        }
    }
    return vector_to_hash_key(vec_real);
}

void HashNode::Disable() {
    this->enabled_ = false;
    ConfNode* node = static_cast<ConfNode*>(this->value_);
    if (node) {
        node->Disable();
    } else {
        // BUG
    }
}

ConfigImpl::ConfigImpl() {
    config_keeper_.reset();
    root_ = new ConfNode(this, "ROOT", "", "", false); 
}


void ConfigImpl::Flush() {
    for (auto &p : hash_table_) {
        delete p.second;
    }
    for (auto &p : watch_table_) {
        delete p.second;
    }
    hash_table_.clear();
    watch_table_.clear();
    delete root_;
    root_ = NULL;
}

ConfigImpl::~ConfigImpl() {
    Flush();
}

void ConfigImpl::NodifyWatcher(const std::string& node_path) {
    std::vector<std::string> vec_path;
    std::vector<std::string> real_path;
    boost::split(vec_path, node_path, boost::is_any_of("/"));
    for (auto &p : vec_path) {
        if (p.size() > 0) {
            real_path.push_back(p);
            std::string hash_key = vector_to_hash_key(real_path);
            if (watch_table_.find(hash_key) != watch_table_.end()) {
                trigger_.insert(std::move(hash_key));
            }
        }
    }
}

void ConfigImpl::FlushWatcher() {
    for (auto &p : trigger_) {
        if (watch_table_.find(p) != watch_table_.end()) {
            ConfNode* node = NULL;
            if ((hash_table_.find(p) != hash_table_.end()) && (hash_table_[p]->enabled_)) {
                node = reinterpret_cast<ConfNode*>(hash_table_[p]->value_);
            }
            watch_table_[p]->handler_(node, p, EVENT_UPDATE);
        }
    }
    trigger_.clear();
}

void ConfigImpl::DisableDeletedNode(const std::set<std::string>& node_set) {
    for (std::unordered_map<std::string, HashNode*>::iterator p = hash_table_.begin(); p != hash_table_.end(); ++p) {
        if (!p->second->enabled_) {
            continue;
        }
        if (node_set.find(p->first) == node_set.end()) {
            p->second->Disable();
            this->NodifyWatcher(p->first);
        }
    }
}

bool ConfigImpl::CheckIfNotExistOrExpired(const std::string& standard_path, int64_t version, bool* no_exist) {
    if (no_exist) {
        *no_exist = ((hash_table_.find(standard_path) == hash_table_.end()) || (!hash_table_[standard_path]->enabled_));
    }
    if (hash_table_.find(standard_path) == hash_table_.end()) {
        return true;
    }
    if (!hash_table_[standard_path]->enabled_) {
        return true;
    }
    if (hash_table_[standard_path]->version_ < version) {
        return true;
    }
    return false;
}

bool ConfigImpl::Load(const std::string& src, int mode, std::string* err_msg) {
    int src_type = SRC_MASK & mode;
    switch (src_type) {
        case SRC_LOCAL:
            config_keeper_.reset(new ConfigKeeperLocal(this));
            break;
        case SRC_ZK:
            config_keeper_.reset(new ConfigZookeeper(this));
            break;
        case SRC_DIRECT:
            config_keeper_.reset(new ConfigKeeperDirect(this));
            break;
        default:
            if (err_msg) {
                *err_msg = "unsupported loader";
            }
    }
    if (config_keeper_) {
        return config_keeper_->LoadConfig(src, mode, err_msg); 
    } else {
        return false;
    }
}

bool ConfigImpl::RegisterWatcher(const std::string& node_path, uint32_t event, EventHandler& handler) {
    std::string standard_path = convert_to_standard_path(node_path);
    WatchNode *node = new WatchNode(standard_path, handler);
    watch_table_.emplace(standard_path, node);
    if (event & EVENT_INIT) {
        ConfNode *node = this->GetConfNode(standard_path);
        if (node) {
            handler(node, standard_path, EVENT_INIT);
        }
    }
    return true;
}

// path : format like 'xx1/xx2/xx3' '/xx1/xx2/xx3' or '/xx1/xx2/xx3/' is all OK, then change to standard format '/x1/x2/x3'
bool ConfigImpl::Insert(const std::string& path, const std::string& value, std::string* err_msg, int64_t version) {
    std::string standard_path = convert_to_standard_path(path);
    std::vector<std::string> vec_path;
    std::vector<std::string> real_path;
    boost::split(vec_path, standard_path, boost::is_any_of("/"));
    ConfNode* current = root_; 
    int index(0);
    for (auto &p : vec_path) {
        if (p.size() > 0) {
            ++index;
            real_path.push_back(p);
            std::string hash_key = vector_to_hash_key(real_path);

            if (current->children().find(p) != current->children().end()) {
                // light up all disabled node in the path
                current = current->children()[p];
                if (!current->enabled()) {
                    current->Refresh();
                }
                if (hash_table_.find(hash_key) != hash_table_.end()) {
                    hash_table_[hash_key]->enabled_ = true; 
                }
            } else {
                ConfNode* node = new ConfNode(this, p, hash_key, "", true);
                current->children().emplace(p, node);
                current->set_is_leaf(false);
                current = node;
                HashNode *hn = new HashNode(current, version);
                hash_table_.emplace(hash_key, hn);
            }
        }
    }
    if (index > 0) {
        current->mutable_value().Set(value);
        std::string hash_key = vector_to_hash_key(real_path);
        if (hash_table_.find(hash_key) != hash_table_.end()) {
            hash_table_[hash_key]->version_ = version; 
        }
        this->NodifyWatcher(standard_path);
    } else {
    }
    
    return true;
}

ConfNode* ConfigImpl::GetConfNode(const std::string& key) {
    std::string real_key = convert_to_standard_path(key);
    if (real_key == "/") {
        return root_;
    }
    if ((hash_table_.find(real_key) != hash_table_.end()) && (hash_table_[real_key]->enabled_)) {
        return reinterpret_cast<ConfNode*>(hash_table_[real_key]->value_);
    } else {
        return NULL;
    }
}

ConfNode* ConfigImpl::GetConfNode(const std::string& key_prefix, const std::string& key) {
    std::string real_key = key_prefix + convert_to_standard_path(key);
    if (real_key == "/") {
        return root_;
    }
    if ((hash_table_.find(real_key) != hash_table_.end()) && (hash_table_[real_key]->enabled_)) {
        return reinterpret_cast<ConfNode*>(hash_table_[real_key]->value_);
    } else {
        return NULL;
    }
}

} // namespace cloris
