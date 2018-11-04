// 
// cloriConf API definition 
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#ifndef CLORIS_CONFIG_H_
#define CLORIS_CONFIG_H_

#include <stdlib.h>
#include <pthread.h>
#include <map>
#include <functional>
#include <boost/noncopyable.hpp>
#include "internal/double_buffer.h"
#include "internal/macro.h"

namespace cloris {

class ConfNode;
class ConfigImpl;
struct HashNode;

typedef std::map<std::string, ConfNode*> ChildrenNodes;
typedef const ConfNode* ConfNodePtr;
typedef std::function<void(ConfNode*, const std::string&, uint32_t)> EventHandler;

template <bool Const> struct SelectIfImpl { template <typename T1, typename T2> struct Apply { typedef T1 Type; }; };
template <> struct SelectIfImpl<false> { template <typename T1, typename T2> struct Apply { typedef T2 Type; }; };
template <bool Const, typename T1, typename T2> struct SelectIfCond : SelectIfImpl<Const>::template Apply<T1, T2> {};
template <bool Const, typename T> struct MaybeAddConst : SelectIfCond<Const, const T, T> {};

template <bool Const, typename TypeName>
class GenericIterator {
    typedef typename MaybeAddConst<Const, TypeName>::Type ValueType;
    typedef typename SelectIfCond<Const, ChildrenNodes::const_iterator, ChildrenNodes::iterator>::Type IteratorType;
public:
    typedef GenericIterator Iterator;
    typedef GenericIterator<true, TypeName> ConstIterator;
    GenericIterator() : current_() {}
    GenericIterator(IteratorType iter) : current_(iter) {}

    Iterator& operator++() { ++current_; return *this; }
    Iterator& operator--() { --current_; return *this; }
    ValueType& operator*() const { return current_->second; }
    ValueType* operator->() const { return current_->second; }
    bool   operator!=(GenericIterator that) const { return current_ != that.current_; }
private:
    IteratorType current_;
};

class ConfNode {
    friend class ConfigImpl;
    friend struct HashNode;
    ConfNode() = delete;
public:
    ConfNode(ConfigImpl* impl, const std::string& name, const std::string& hash_key, const std::string& value, bool is_leaf); 
    ~ConfNode();

    typedef typename GenericIterator<false, ConfNode>::Iterator ChildrenIterator;
    typedef typename GenericIterator<true, ConfNode>::Iterator  ConstChildrenIterator;

    ConstChildrenIterator begin() const { return ConstChildrenIterator(children_.begin()); } 
    ConstChildrenIterator end() const { return ConstChildrenIterator(children_.end()); }
    ChildrenIterator begin() { return ChildrenIterator(children_.begin()); }
    ChildrenIterator end() { return ChildrenIterator(children_.end()); }

    const std::string& AsString() const;
    int32_t AsInt32() const;
    int64_t AsInt64() const;
    double  AsDouble() const;
    bool    AsBool() const;

    const std::string GetString(const std::string& name, const std::string& default_value = "") const;
    int32_t GetInt32(const std::string& name, int32_t default_value = 0) const;
    int64_t GetInt64(const std::string& name, int64_t default_value = 0L) const;
    double  GetDouble(const std::string& name, double default_value = 0.0) const;
    bool    GetBool(const std::string& name, bool default_value = false) const;
    ConfNode* GetConfNode(const std::string& name = "") const; 
    bool Exists(const std::string& name) const;
    const std::string& name() const { return name_; }

private:
    const DoubleBuffer<std::string>& value() const { return value_; } 
    DoubleBuffer<std::string>& mutable_value() { return value_; } 
    bool enabled() const  { return enabled_; }
    void Disable() { enabled_ = false; }
    void Refresh();
    void Flush();
    std::map<std::string, ConfNode*>& children() { return children_; }
    bool is_leaf() const { return is_leaf_; }
    void set_is_leaf(bool is_leaf) { is_leaf_ = is_leaf; }

    ConfigImpl *impl_;
    std::map<std::string, ConfNode*> children_;
    std::string name_;
    DoubleBuffer<std::string> value_;
    std::string hash_key_;
    bool enabled_;
    bool is_leaf_;
};

class Config : boost::noncopyable {
public:
    static Config* instance() noexcept;
    Config(const std::string& input, uint32_t mode);
    Config();
    ~Config();

    Config* Load(const std::string& input, uint32_t mode, std::string* err_msg = NULL) noexcept;
    bool Watch(const std::string& path, uint32_t event, EventHandler& handler);

    ConfNode* GetConfNode(const std::string& name = "") const;
    std::string GetString(const std::string& name, const std::string& default_value = "") const;
    int32_t GetInt32(const std::string& name, int32_t default_value = 0) const;
    int64_t GetInt64(const std::string& name, int64_t default_value = 0L) const;
    double  GetDouble(const std::string& name, double default_value = 0.0) const;
    bool    GetBool(const std::string& name, bool default_value = false) const;
    bool    Exists(const std::string& name) const ;

    bool Ok() const { return (status_ == 0); }
    const std::string& ErrorText() const { return last_error_; }
private:
    ConfigImpl* impl_;
    int status_;
    std::string last_error_;
};

} // namespace cloris

#endif // CLORIS_CONFIG_H_
