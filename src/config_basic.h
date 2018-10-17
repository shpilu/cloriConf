// 
// cloriConf basic header
// version: 1.0 
// Copyright 2018 James Wei (weijianlhp@163.com)
//

#ifndef CLORIS_CONFIG_BASIC_H_
#define CLORIS_CONFIG_BASIC_H_

#define EVENT_INIT          0x00000001
#define EVENT_ADD           0x00000002
#define EVENT_DELETE        0x00000004
#define EVENT_UPDATE        0x00000008
#define EVENT_CHILDREN      0x00000010
#define EVENT_SELF_CHANGED  (EVENT_ADD | EVENT_DELETE | EVENT_UPDATE)
#define EVENT_CHANGED       (EVENT_ADD | EVENT_DELETE | EVENT_UPDATE | EVENT_CHILDREN)
#define EVENT_ALL           (EVENT_INIT | EVENT_ADD | EVENT_DELETE | EVENT_UPDATE | EVENT_CHILDREN)

// |...| FMT(4bit) | COMMENT(4bit) | SRC(4bit) |
#define SRC_LOCAL       0x00000001
#define SRC_DIRECT      0x00000002
#define SRC_ZK          0x00000004

#define FMT_JINI        0x00000010
#define FMT_JSON        0x00000020

#define CMT_SHARP       0x00000100  // #
#define CMT_SLASH       0x00000200  // //
#define CMT_SEMICOLON   0x00000400  // ;
#define CMT_PERCENT     0x00000800  // %

#define SRC_MASK        0x0000000f
#define FMT_MASK        0x000000f0
#define CMT_MASK        0x00000f00

namespace cloris {

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

template <typename T>
class Singleton : boost::noncopyable {
public:
    static T* instance() {
        pthread_once(&ponce_, &Singleton::Init);
        return value_;
    }   
private:
    static void Init() {
        value_ = new T();
        ::atexit(Destroy);
    }   
    static void Destroy() {
        delete value_;
    }   
private:
    static pthread_once_t ponce_;
    static T* value_;
};

template<typename T>
pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

template<typename T>
T* Singleton<T>::value_ = NULL;

} // namespace cloris

#endif // CLORIS_CONFIG_BASIC_H_
