// 
// cloriConf API implementation 
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//
#include "internal/singleton.h"
#include "config_impl.h"
#include "config.h"

namespace cloris {

void ConfNode::Refresh() { 
    enabled_ = true; 
    value_.Set("");
}

const std::string& ConfNode::AsString() const {
    return value_.Get();
}

int32_t ConfNode::AsInt32() const {
    return atoi(value_.Get().c_str());
}

int64_t ConfNode::AsInt64() const {
    return atol(value_.Get().c_str());
}

double ConfNode::AsDouble() const {
    return atof(value_.Get().c_str());
}

bool ConfNode::AsBool() const {
    return (value_.Get() == "true");
}

ConfNode* ConfNode::GetConfNode(const std::string& name) const {
    return impl_->GetConfNode(this->hash_key_, name);
}

const std::string ConfNode::GetString(const std::string& name, const std::string& def_val) const {
    const ConfNode* node = this->GetConfNode(name);
    if (node) {
        return node->AsString();
    } else {
        return def_val;
    }
}

int32_t ConfNode::GetInt32(const std::string& name, int32_t def_val) const {
    const ConfNode* node = this->GetConfNode(name);
    if (node) {
        return node->AsInt32();
    } else {
        return def_val;
    }
}

int64_t ConfNode::GetInt64(const std::string& name, int64_t def_val) const {
    const ConfNode* node = this->GetConfNode(name);
    if (node) {
        return node->AsInt64();
    } else {
        return def_val;
    }
}

double ConfNode::GetDouble(const std::string& name, double def_val) const {
    const ConfNode* node = this->GetConfNode(name);
    if (node) {
        return node->AsDouble();
    } else {
        return def_val;
    }
}

bool ConfNode::GetBool(const std::string& name, bool def_val) const {
    const ConfNode* node = this->GetConfNode(name);
    if (node) {
        return node->AsBool();
    } else {
        return def_val;
    }
}

Config* Config::instance() noexcept {
    return Singleton<Config>::instance();
}

Config::Config()
    : status_(0),
      last_error_("") {
    impl_ = new ConfigImpl();
}

Config::Config(const std::string& input, uint32_t mode) : Config() {
    if (!this->Load(input, mode, &last_error_)) {
        status_ = -1;
    }
}

Config::~Config() {
    delete impl_;
}

bool Config::Watch(const std::string& node_path, uint32_t event, EventHandler& handler) {
    return impl_->RegisterWatcher(node_path, event, handler);
}

Config* Config::Load(const std::string& input, uint32_t mode, std::string* err_msg) noexcept {
    bool ret = impl_->Load(input, mode, err_msg);
    return ret ? this : NULL; 
}

bool Config::LoadEx(const std::string& input, uint32_t mode, std::string* err_msg) noexcept {
    bool ret = impl_->Load(input, mode, err_msg);
    return ret;
}

ConfNode::ConfNode(ConfigImpl* impl, const std::string& name, const std::string& hash_key, const std::string& value, bool is_leaf) 
    : impl_(impl), 
      name_(name),
      hash_key_(hash_key),
      enabled_(true), 
      is_leaf_(is_leaf) { 
    value_.Set(value);
}

void ConfNode::Flush() {
    for (auto &p : children_) {
        delete p.second;
    }
    children_.clear();
}

ConfNode::~ConfNode() {
    Flush();
}

ConfNode* Config::GetConfNode(const std::string& name) const {
    return impl_->GetConfNode(name);
}

std::string Config::GetString(const std::string& name, const std::string& def_val) const {
    const ConfNode* node = this->GetConfNode(name);
    if (node) {
        return node->AsString();
    } else {
        return def_val;
    }
} 

int32_t Config::GetInt32(const std::string& name, int32_t def_val) const {
    const ConfNode* node = this->GetConfNode(name);
    if (node) {
        return node->AsInt32();
    } else {
        return def_val;
    }
} 

int64_t Config::GetInt64(const std::string& name, int64_t def_val) const {
    const ConfNode* node = this->GetConfNode(name);
    if (node) {
        return node->AsInt64();
    } else {
        return def_val;
    }
} 

double Config::GetDouble(const std::string& name, double def_val) const {
    const ConfNode* node = this->GetConfNode(name);
    if (node) {
        return node->AsDouble();
    } else {
        return def_val;
    }
} 

bool Config::GetBool(const std::string& name, bool def_val) const {
    const ConfNode* node = this->GetConfNode(name);
    if (node) {
        return node->AsBool();
    } else {
        return def_val;
    }
} 

bool Config::Exists(const std::string& name) const {
    const ConfNode* node = this->GetConfNode(name);
    return node ? true : false;
}

} // namespace cloris
