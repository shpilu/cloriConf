//
// cloriConf wrapper 
// Copyright 2018 James Wei (weijianlhp@163.com)
//

#include "config_impl.h"
#include "config.h"

namespace cloris {

void CNode::Refresh() { 
    enabled_ = true; 
    value_.Set("");
}

const std::string& CNode::AsString() const {
    return value_.Get();
}

int32_t CNode::AsInt32() const {
    return atoi(value_.Get().c_str());
}

int64_t CNode::AsInt64() const {
    return atol(value_.Get().c_str());
}

double CNode::AsDouble() const {
    return atof(value_.Get().c_str());
}

bool CNode::AsBool() const {
    return (value_.Get() == "true");
}

const CNode* CNode::GetCNode(const std::string& key) const {
    return impl_->GetCNode(this->hkey_, key);
}

const std::string CNode::GetString(const std::string& key, const std::string& def_val) const {
    const CNode* node = this->GetCNode(key);
    if (node) {
        return node->AsString();
    } else {
        return def_val;
    }
}

int32_t CNode::GetInt32(const std::string& key, int32_t def_val) const {
    const CNode* node = this->GetCNode(key);
    if (node) {
        return node->AsInt32();
    } else {
        return def_val;
    }
}

int64_t CNode::GetInt64(const std::string& key, int64_t def_val) const {
    const CNode* node = this->GetCNode(key);
    if (node) {
        return node->AsInt64();
    } else {
        return def_val;
    }
}

double CNode::GetDouble(const std::string& key, double def_val) const {
    const CNode* node = this->GetCNode(key);
    if (node) {
        return node->AsDouble();
    } else {
        return def_val;
    }
}

bool CNode::GetBool(const std::string& key, bool def_val) const {
    const CNode* node = this->GetCNode(key);
    if (node) {
        return node->AsBool();
    } else {
        return def_val;
    }
}

CNodeIterator::CNodeIterator(const std::map<std::string, CNode*>::iterator& begin,
    const std::map<std::string, CNode*>::iterator& end, 
    uint8_t mode) 
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

CNode& CNodeIterator::operator*() {
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

CNodeIterator CNode::non_leaf_begin() {
    std::map<std::string, CNode*>::iterator iter = children_.begin();
    while ((iter != children_.end()) && (iter->second->is_leaf())) {
        ++iter;
    }
    return CNodeIterator(iter, children_.end(), ITYPE_NONLEAF);
}

CNodeIterator CNode::non_leaf_end() {
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

Config::Config(const std::string& input, int mode) : Config() {
    if (!this->Load(input, mode, &last_error_)) {
        status_ = -1;
    }
}

Config::~Config() {
    delete impl_;
}

bool Config::Watch(const std::string& node_path, uint32_t event, EventHandler& handler) {
    // TODO
    return impl_->RegisterWatcher(node_path, event, handler);
}

Config* Config::Load(const std::string& input, int mode, std::string* err_msg) {
    bool ret = impl_->Load(input, mode, err_msg);
    return ret ? this : NULL; 
}

CNode::CNode(ConfigImpl* impl, const std::string& path, const std::string& hash_key, const std::string& value, bool is_leaf) 
    : impl_(impl), 
      key_(path), 
      hkey_(hash_key),
      enabled_(true), 
      is_leaf_(is_leaf) { 
    value_.Set(value);
}

CNode::~CNode() {
    for (auto &p : children_) {
        delete p.second;
    }
}

const CNode* Config::GetCNode(const std::string& key) const {
    return impl_->GetCNode(key);
}

std::string Config::GetString(const std::string& key, const std::string& def_val) const {
    const CNode* node = this->GetCNode(key);
    if (node) {
        return node->AsString();
    } else {
        return def_val;
    }
} 

int32_t Config::GetInt32(const std::string& key, int32_t def_val) const {
    const CNode* node = this->GetCNode(key);
    if (node) {
        return node->AsInt32();
    } else {
        return def_val;
    }
} 

int64_t Config::GetInt64(const std::string& key, int64_t def_val) const {
    const CNode* node = this->GetCNode(key);
    if (node) {
        return node->AsInt64();
    } else {
        return def_val;
    }
} 

double Config::GetDouble(const std::string& key, double def_val) const {
    const CNode* node = this->GetCNode(key);
    if (node) {
        return node->AsDouble();
    } else {
        return def_val;
    }
} 

bool Config::GetBool(const std::string& key, bool def_val) const {
    const CNode* node = this->GetCNode(key);
    if (node) {
        return node->AsBool();
    } else {
        return def_val;
    }
} 

} // namespace cloris
