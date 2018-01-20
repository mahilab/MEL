#pragma once

#include <memory>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

template <typename T>
class RingBuffer {

public:

    /// Default constructor
    RingBuffer(std::size_t capacity) :
        size_(capacity + 1),
        head_(0),
        tail_(0),
        buffer_(std::unique_ptr<T[]>(new T[size_]))
    { }

    /// Read access
    const T& operator[](std::size_t index) const {
        return buffer_[tail_ + index];
    }

    /// Write access
    T& operator[](std::size_t index) {
        return buffer_[tail_ + index];
    }

    /// Puts the value at the head of the RingBuffer
    void push_back(T value) {
        buffer_[head_] = value;
        head_ = (head_ + 1) % size_;
        if (head_ == tail_)
            tail_ = (tail_ + 1) % size_;
    }

    /// Gets the value at the tail of the RingBuffer
    T pop_front() {
        if (is_empty())
            return T();
        T value = buffer_[tail_];
        tail_ = (tail_ + 1) % size_;
        return value;
    }

    /// Returns true if the RingBuffer is empty
    bool is_empty() const {
        return head_ == tail_;
    }

    /// Returns true if the RingBuffer is full
    bool is_full() const  {
        return ((head_ + 1) % size_) == tail_;
    }

    /// Returns the capacity of the RingBuffer
    size_t capacity() const {
        return size_ - 1;
    }

    /// Returns occupied size of RingBuffer
    size_t size() const {
        return head_ - tail_;
    }

private:

    size_t size_;  ///< the internal size of the buffer (requested size + 1)
    size_t head_;  ///< head of the RingBuffer
    size_t tail_;  ///< tail of the RingBuffer
    std::unique_ptr<T[]> buffer_; ///< underlying buffer array

};

} // mel
