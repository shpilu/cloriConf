// 
// implementaton of double buffer 
// Copyright 2017 James Wei (weijianlhp@163.com)
//

#ifndef CLORIS_DOUBLE_BUFFER_H_
#define CLORIS_DOUBLE_BUFFER_H_

namespace cloris {

template <typename T>
class DoubleBuffer : boost::noncopyable {
public:
    DoubleBuffer();
    DoubleBuffer(const T& value);
    ~DoubleBuffer();

    void Set(const T& value);
    void SetDirect(T value);
    const T& Get() const;
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
const T& DoubleBuffer<T>::Get() const  {
    return value_[current_];
}

} // namespace cloris

#endif // CLORIS_DOUBLE_BUFFER_H_
