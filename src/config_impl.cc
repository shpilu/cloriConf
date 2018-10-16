#include <memory>
#include <boost/algorithm/string.hpp>
#include "loader/zookeeper/config_zk.h"
#include "loader/local/config_local.h"
#include "loader/direct/config_direct.h"
#include "config_impl.h"


// 所有路径格式都以"/xx1/xx2/xx3"的格式进行标准化

namespace cloris {

using namespace std::placeholders;

static std::string vec2hkey(const std::vector<std::string>& vec) {
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
static std::string convert2stdkey(const std::string& key) {
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
    return vec2hkey(vec_real);
}

void HNode::disable() {
    this->enabled_ = false;
    CNode* cn = static_cast<CNode*>(this->value_);
    if (cn) {
        cn->disable();
    } else {
        // BUG
    }
}

ConfigImpl::ConfigImpl() : root_(this, "ROOT", "", "", false) {
    ck_.reset();
}

ConfigImpl::~ConfigImpl() {
}

void ConfigImpl::NodifyWatcher(const std::string& cpath) {
    std::vector<std::string> vec_path;
    std::vector<std::string> real_path;
    boost::split(vec_path, cpath, boost::is_any_of("/"));
    for (auto &p : vec_path) {
        if (p.size() > 0) {
            real_path.push_back(p);
            std::string hkey = vec2hkey(real_path);
            if (wtable_.find(hkey) != wtable_.end()) {
                trigger_.insert(std::move(hkey));
            }
        }
    }
}

void ConfigImpl::FlushWatcher() {
    for (auto &p : trigger_) {
        if (wtable_.find(p) != wtable_.end()) {
            CNode* node = NULL;
            if ((htable_.find(p) != htable_.end()) && (htable_[p]->enabled_)) {
                node = reinterpret_cast<CNode*>(htable_[p]->value_);
            }
            wtable_[p]->handler_(node, p, EVENT_UPDATE);
        }
    }
    trigger_.clear();
}

void ConfigImpl::DisableDeletedNode(std::set<std::string>& nset) {
    for (std::unordered_map<std::string, HNode*>::iterator p = htable_.begin(); p != htable_.end(); ++p) {
        if (!p->second->enabled_) {
            continue;
        }
        if (nset.find(p->first) == nset.end()) {
            p->second->disable();
            this->NodifyWatcher(p->first);
        }
    }
}

bool ConfigImpl::CheckIfNotExistOrExpired(const std::string& stdkey, int64_t version, bool* noexist) {
    if (noexist) {
        *noexist = ((htable_.find(stdkey) == htable_.end()) || (!htable_[stdkey]->enabled_));
    }
    if (htable_.find(stdkey) == htable_.end()) {
        return true;
    }
    if (!htable_[stdkey]->enabled_) {
        return true;
    }
    if (htable_[stdkey]->version_ < version) {
        return true;
    }
    return false;
}

bool ConfigImpl::Load(const std::string& src, int mode, std::string& err_msg) {
    switch (mode) {
        case SRC_LOCAL:
            ck_.reset(new ConfigKeeperLocal(this));
            break;
        case SRC_ZK:
            ck_.reset(new ConfigZookeeper(this));
            break;
        case SRC_DIRECT:
            ck_.reset(new ConfigKeeperDirect(this));
            break;
        default:
            err_msg = "unsupported loader";
    }
    if (ck_) {
        return ck_->LoadConfig(src, err_msg); 
    } else {
        return false;
    }
}

bool ConfigImpl::RegisterWatcher(const std::string& cpath, uint32_t event, EventHandler& handler) {
    std::string std_path = convert2stdkey(cpath);
    WNode *node = new WNode(std_path, handler);
    // wtable_.insert(std::pair<std::string, WNode*>(std_path, node));
    wtable_.emplace(std_path, node);

    if (event & EVENT_INIT) {
        CNode *node = this->getCNode(std_path);
        if (node) {
            handler(node, std_path, EVENT_INIT);
        }
    }

    return true;
}

// cpath : format like 'xx1/xx2/xx3' '/xx1/xx2/xx3' or '/xx1/xx2/xx3/' is all OK, then change to standard format '/x1/x2/x3'
bool ConfigImpl::Insert(const std::string& cpath, const std::string& value, std::string& err_msg, int64_t version) {
    std::string cleaned_path = convert2stdkey(cpath);
    std::vector<std::string> vec_path;
    std::vector<std::string> real_path;
    boost::split(vec_path, cleaned_path, boost::is_any_of("/"));
    CNode* current = &root_; 
    int index(0);
    for (auto &p : vec_path) {
        if (p.size() > 0) {
            ++index;
            real_path.push_back(p);
            std::string hkey = vec2hkey(real_path);

            if (current->children().find(p) != current->children().end()) {
                current = current->children()[p];
                if (!current->enabled()) {
                    current->refresh();
                }
                // set all enabled in the path
                if (htable_.find(hkey) != htable_.end()) {
                    htable_[hkey]->enabled_ = true; 
                }
                // 
            } else {
                CNode* node = new CNode(this, p, hkey, "", true);
                // current->children().insert(std::pair<std::string, CNode*>(p, node));
                current->children().emplace(p, node);
                current->set_is_leaf(false);
                current = node;
                HNode *hn = new HNode(current, version);
                // htable_.insert(std::pair<std::string, HNode*>(hkey, hn));
                htable_.emplace(hkey, hn);
            }
        } else {
            // TODO bad case
        }
    }
    if (index > 0) {
        // update version
        current->value().Set(value);
        std::string hkey = vec2hkey(real_path);
        if (htable_.find(hkey) != htable_.end()) {
            htable_[hkey]->version_ = version; 
        }
        this->NodifyWatcher(cleaned_path);
    } else {
    }
    
    return true;
}

CNode* ConfigImpl::getCNode(const std::string& key) {
    std::string real_key = convert2stdkey(key);
    if (real_key == "/") {
        return &root_;
    }
    if ((htable_.find(real_key) != htable_.end()) && (htable_[real_key]->enabled_)) {
        return reinterpret_cast<CNode*>(htable_[real_key]->value_);
    } else {
        return NULL;
    }
}

CNode* ConfigImpl::getCNode(const std::string& key_prefix, const std::string& key) {
    std::string real_key = key_prefix + convert2stdkey(key);
    if (real_key == "/") {
        return &root_;
    }
    if ((htable_.find(real_key) != htable_.end()) && (htable_[real_key]->enabled_)) {
        return reinterpret_cast<CNode*>(htable_[real_key]->value_);
    } else {
        return NULL;
    }
}

} // namespace cloris
