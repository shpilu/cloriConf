// cloriConf -- a flexisible config service and service found 
// version: 1.0 
// Copyright 2018 James Wei (weijianlhp@163.com)
//
// usage:
// conf = Config::instance().Load(path, SP_LOCAL); // Load local config file 
// conf = Config::instance().Load(path, SP_DIRECT); // 直接将入参作为参数解析
// conf = Config::instance().Load(path, SP_ZK); // 加载zookeeper
// conf = Config::instance().Load(path, SP_HTTP); // 加载HTTP
// conf = Config::instance().Load(path, SP_ZK, MD_LOOP); // 加载并监听, 如果有更新则更新conf本地
// conf = Config::instance().LoadConfig(configure); // 以配置文件的形式加载
// spect = std::bind(xxxx);
// conf.Watch(path, spect); // 消息订阅, 
// std::string name  = conf->getNode().asString();
// int32_t year = conf->getInt32("xxx/year");
// for (auto &p : dada) {
//      LOG(p.asString());
// }
//
// 支持的功能:
// (1) 从各个源(本地文件、直接配置、zookeeper、http接口)加载配置
// (2) 快速的配置读取
// (3) 配置热更新
// (4) 消息订阅
// (5) 配置转换
//
//

#ifndef CLORIS_CONFIG_H_
#define CLORIS_CONFIG_H_

#include <stdlib.h>
#include <pthread.h>
#include <map>
#include <functional>
#include <boost/noncopyable.hpp>

#define EVENT_INIT          0x00000001
#define EVENT_ADD           0x00000002
#define EVENT_DELETE        0x00000004
#define EVENT_UPDATE        0x00000008
#define EVENT_CHILDREN      0x00000010
#define EVENT_SELF_CHANGED  (EVENT_ADD | EVENT_DELETE | EVENT_UPDATE)
#define EVENT_CHANGED       (EVENT_ADD | EVENT_DELETE | EVENT_UPDATE | EVENT_CHILDREN)
#define EVENT_ALL           (EVENT_INIT | EVENT_ADD | EVENT_DELETE | EVENT_UPDATE | EVENT_CHILDREN)

namespace cloris {

class ConfigImpl;

template <typename T>
class DoubleBuffer : boost::noncopyable {
public:
    DoubleBuffer();
    DoubleBuffer(const T& value);
    ~DoubleBuffer();

    void Set(const T& value);
    void SetDirect(T value);
    T& get();
private:
    T value_[2];
    int32_t current_;
};

template<typename T>
DoubleBuffer<T>::DoubleBuffer() 
    : current_(0) {
}

template<typename T>
DoubleBuffer<T>::DoubleBuffer(const T& value) 
    : current_(0) {
    value_[0] = value;
}

template<typename T>
DoubleBuffer<T>::~DoubleBuffer() {
}

template<typename T>
void DoubleBuffer<T>::Set(const T& value) {
    value_[!current_] = value;
    current_ = !current_;
}

template<typename T>
void DoubleBuffer<T>::SetDirect(T value) {
    value_[!current_] = value;
    current_ = !current_;
}

template<typename T> 
T& DoubleBuffer<T>::get() {
    return value_[current_];
}

class CNode;

typedef std::function<void(CNode*, const std::string&, uint32_t)> EventHandler;

enum SP_MODE {
    SP_LOCAL = 0,
    SP_DIRECT = 1,
    SP_ZK  = 2,
};

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

class CNode {
public:
    friend class CNodeIterator;

    CNodeIterator begin();
    CNodeIterator end();
    CNodeIterator leaf_begin();
    CNodeIterator leaf_end();
    CNodeIterator nonleaf_begin();
    CNodeIterator nonleaf_end();
public:
    CNode() = delete;
    CNode(ConfigImpl* impl, const std::string& key, const std::string& hkey, const std::string& value, bool is_leaf) 
        : impl_(impl), 
          key_(key), 
          hkey_(hkey),
          enabled_(true), 
          is_leaf_(is_leaf) { 
        value_.Set(value);
    }

    void refresh() { 
        enabled_ = true; 
        value_.Set("");
    }
    std::string& asString();
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

    bool enabled() { return enabled_; }
    void disable() { enabled_ = false; }
    void set_is_leaf(bool is_leaf) { is_leaf_ = is_leaf; }
    bool is_leaf() { return is_leaf_; }
    const std::string& key() const { return key_; }

    CNode* getCNode(const std::string& key = ""); 
    
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

    Config(const std::string& src, SP_MODE mode);
    Config();
    ~Config();

    Config* Load(const std::string& src, SP_MODE mode, std::string* err_msg = NULL);
    bool Watch(const std::string& path, uint32_t event, EventHandler& handler);

    CNode*      getCNode(const std::string& key = "");
    std::string getString(const std::string& key, const std::string& def_val = "");
    int32_t     getInt32(const std::string& key, int32_t def_val = 0);
    int64_t     getInt64(const std::string& key, int64_t def_val = 0L);
    double      getDouble(const std::string& key, double def_val = 0.0);
    bool        getBool(const std::string& key, bool def_val = false);

    bool ok() { return (status_ == 0); }
    const std::string& error() { return last_error_; }
private:
    int status_;
    std::string last_error_;

    static void Init();
    static void Destroy();

    ConfigImpl* impl_;
    static pthread_once_t ponce_;
    static Config* value_;
};

} // namespace cloris

#endif // CLORIS_CONFIG_H_
