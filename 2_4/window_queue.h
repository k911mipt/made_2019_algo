#ifndef RMQUEUE_H_
#define RMQUEUE_H_

#include <deque>
#include <cassert>
#include "common.h"

namespace made {

    namespace stl {

        typedef size_t size_type;

        class WindowStdQueue {
        public:
            WindowStdQueue(int* arr, size_type window_size) : window_size_(window_size) {
                maximum_ = *arr;
                buffer_.push_back({ index_, *arr });
                index_++;
                arr++;
                front_element_ = &buffer_.front();
                back_element_ = &buffer_.back();
                for (; index_ < window_size_; arr++) {
                    push(*arr);
                }

            };
            void push(int item) {
                if (window_size_ < 2) {
                    front_element_->value = item;
                    ++index_;
                    return;
                }
                while ((buffer_.size() > 1) && (front_element_->index + window_size_ <= index_)) {
                    buffer_.pop_front();
                    front_element_ = &buffer_.front();
                }
                assert(buffer_.size() <= window_size_);
                int current_value = item;
                back_element_ = &buffer_.back();
                if (front_element_->value <= current_value) {
                    buffer_.clear();
                    buffer_.push_back({ index_, current_value });
                    front_element_ = &buffer_.front();
                    back_element_ = &buffer_.back();
                }
                else if (back_element_->value == current_value) {
                    back_element_->value = current_value;
                    back_element_->index = index_;
                }
                else if (back_element_->value > current_value) {
                    buffer_.push_back({ index_, current_value });
                }
                assert(buffer_.size() <= window_size_);
                ++index_;
            }
            int getMax() {
                return front_element_->value;
                return 0;
            }
        private:
            std::deque<task::WindowElement> buffer_;
            size_type window_size_;
            size_type left_ = 0;
            size_type right_ = 0;
            size_type index_ = 0;
            int maximum_;
            task::WindowElement *front_element_;
            task::WindowElement *back_element_;
        };

        template <typename T>
        class WindowQueue
        {
        public:
            void PushFront(T value) {
                IncSize();
                *head_ = value;
                DecModCapacity(head_);
            };
            T PopFront() {
                if (size_ == 0) return -1;
                DecSize();
                IncModCapacity(head_);
                return *head_;
            };
            void PushBack(T value) {
                IncSize();
                *tail_ = value;
                IncModCapacity(tail_);
            };
            T PopBack() {
                if (size_ == 0) return -1;
                DecSize();
                DecModCapacity(tail_);
                return *tail_;
            };
            WindowQueue() = default;
            ~WindowQueue() {
                if (buffer_) {
                    delete[] buffer_;
                }
            }
        private:
            const size_type DEFAULT_CAPACITY_ = 1;
            size_type size_ = 0;
            size_type capacity_ = DEFAULT_CAPACITY_;
            T* buffer_ = new T[DEFAULT_CAPACITY_];
            T* right_ = buffer_ + capacity_;
            T* head_ = buffer_;
            T* tail_ = buffer_;
        private:
            void ReallocBuffer(size_type newCapacity)
            {
                T* newBuffer = new T[newCapacity];
                IncModCapacity(head_);
                DecModCapacity(tail_);
                ++tail_;
                T* newTail;
                if (head_ < tail_) {
                    newTail = std::copy(head_, tail_, newBuffer);
                }
                else {
                    newTail = std::copy(head_, right_, newBuffer);
                    newTail = std::copy(buffer_, tail_, newTail);
                }
                delete[] buffer_;
                buffer_ = newBuffer;
                capacity_ = newCapacity;
                right_ = buffer_ + capacity_;
                head_ = right_ - 1;
                tail_ = (newTail == right_) ? buffer_ : newTail;
            };
            void IncSize() {
                ++size_;
                if (size_ > capacity_) {
                    ReallocBuffer(capacity_ * 2);
                }
            };
            void DecSize() {
                --size_;
                size_type lowThresholdCapacity = capacity_ / 4;
                if (size_ < lowThresholdCapacity) {
                    ReallocBuffer(lowThresholdCapacity);
                }
            };
            void IncModCapacity(T* &cur) {
                if (++cur == right_)
                    cur = buffer_;
            };
            void DecModCapacity(T* &cur) {
                if (--cur < buffer_)
                    cur = right_ - 1;
            };
        };
    }

}

#endif // RMQUEUE_H_

