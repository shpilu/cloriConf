//
// cloriConf wrapper 
// Copyright 2018 James Wei (weijianlhp@163.com)
//

#include "config_impl.h"
#include "config.h"

namespace cloris {

const std::string& CNode::asString() {
    return value_.get();
}

int32_t CNode::asInt32() {
    return atoi(value_.get().c_str());
}

int64_t CNode::asInt64() {
    return atol(value_.get().c_str());
}

double CNode::asDouble() {
    return atof(value_.get().c_str());
}

bool CNode::asBool() {
    return (value_.get() == "true");
}

CNode* CNode::getCNode(const std::string& key) {
    return impl_->getCNode(this->hkey_, key);
}

std::string CNode::getString(const std::string& key, const std::string& def_val) {
    CNode* node = this->getCNode(key);
    if (node) {
        return node->asString();
    } else {
        return def_val;
    }
}

int32_t CNode::getInt32(const std::string& key, int32_t def_val) {
    CNode* node = this->getCNode(key);
    if (node) {
        return node->asInt32();
    } else {
        return def_val;
    }
}

int64_t CNode::getInt64(const std::string& key, int64_t def_val) {
    CNode* node = this->getCNode(key);
    if (node) {
        return node->asInt64();
    } else {
        return def_val;
    }
}

double CNode::getDouble(const std::string& key, double def_val) {
    CNode* node = this->getCNode(key);
    if (node) {
        return node->asDouble();
    } else {
        return def_val;
    }
}

bool CNode::getBool(const std::string& key, bool def_val) {
    CNode* node = this->getCNode(key);
    if (node) {
        return node->asBool();
    } else {
        return def_val;
    }
}

CNodeIterator::CNodeIterator(const std::map<std::string, CNode*>::iterator& begin,
    const std::map<std::string, CNode*>::iterator& end, uint8_t mode) 
        : current_(begin), 
          end_(end),
          mode_(mode) {
}

CNodeIterator& CNodeIterator::operator++() {
    ++current_;
    if (mode_ == ITYPE_ALL) {
        ;
    } else if (mode_ == ITYPE_LEAF) {
        while ((current_ != end_) && (!current_->second->is_leaf())) {
            ++current_;
        }
    } else {
        while ((current_ != end_) && (current_->second->is_leaf())) {
            ++current_;
        }
    }
    return *this;
}

CNode & CNodeIterator::operator*() {
    return *(current_->second);
}

CNode* CNodeIterator::operator->() {
    return current_->second;
}

bool CNodeIterator::operator!=(const CNodeIterator& iter) {
    return current_ != iter.current();
}

CNodeIterator CNode::begin() {
    return CNodeIterator(children_.begin(), children_.end(), ITYPE_ALL);
}

CNodeIterator CNode::end() {
    return CNodeIterator(children_.end(), children_.end(), ITYPE_ALL);
}

CNodeIterator CNode::leaf_begin() {
    std::map<std::string, CNode*>::iterator iter = children_.begin();
    while ((iter != children_.end()) && (!iter->second->is_leaf())) {
        ++iter;
    }
    return CNodeIterator(iter, children_.end(), ITYPE_LEAF);
}

CNodeIterator CNode::leaf_end() {
    return CNodeIterator(children_.end(), children_.end(), ITYPE_LEAF);
}

CNodeIterator CNode::nonleaf_begin() {
    std::map<std::string, CNode*>::iterator iter = children_.begin();
    while ((iter != children_.end()) && (iter->second->is_leaf())) {
        ++iter;
    }
    return CNodeIterator(iter, children_.end(), ITYPE_NONLEAF);
}

CNodeIterator CNode::nonleaf_end() {
    return CNodeIterator(children_.end(), children_.end(), ITYPE_NONLEAF);
}

Config* Config::instance() {
    return Singleton<Config>::instance();
}

Config::Config()
    : status_(0),
      last_error_("") {
    impl_ = new ConfigImpl();
}

Config::Config(const std::string& src, int mode) : Config() {
    if (!this->Load(src, mode, &last_error_)) {
        status_ = -1;
    }
}

Config::~Config() {
    delete impl_;
}

bool Config::Watch(const std::string& cpath, uint32_t event, EventHandler& handler) {
    // TODO
    return impl_->RegisterWatcher(cpath, event, handler);
}

Config* Config::Load(const std::string& src, int mode, std::string* err_msg) {
    bool ret = impl_->Load(src, mode, err_msg);
    return ret ? this : NULL; 
}

CNode::CNode(ConfigImpl* impl, const std::string& key, const std::string& hkey, const std::string& value, bool is_leaf) 
    : impl_(impl), 
      key_(key), 
      hkey_(hkey),
      enabled_(true), 
      is_leaf_(is_leaf) { 
    value_.Set(value);
}

CNode::~CNode() {
    for (auto &p : children_) {
        delete p.second;
    }
}

CNode* Config::getCNode(const std::string& key) {
    return impl_->getCNode(key);
}

std::string Config::getString(const std::string& key, const std::string& def_val) {
    CNode* node = this->getCNode(key);
    if (node) {
        return node->asString();
    } else {
        return def_val;
    }
} 

int32_t Config::getInt32(const std::string& key, int32_t def_val) {
    CNode* node = this->getCNode(key);
    if (node) {
        return node->asInt32();
    } else {
        return def_val;
    }
} 

int64_t Config::getInt64(const std::string& key, int64_t def_val) {
    CNode* node = this->getCNode(key);
    if (node) {
        return node->asInt64();
    } else {
        return def_val;
    }
} 

double Config::getDouble(const std::string& key, double def_val) {
    CNode* node = this->getCNode(key);
    if (node) {
        return node->asDouble();
    } else {
        return def_val;
    }
} 

bool Config::getBool(const std::string& key, bool def_val) {
    CNode* node = this->getCNode(key);
    if (node) {
        return node->asBool();
    } else {
        return def_val;
    }
} 

} // namespace cloris
