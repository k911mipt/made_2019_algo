#ifndef DEQUEUE_H_
#define DEQUEUE_H_

namespace made {

    namespace stl {

        typedef size_t size_type;

        template <typename T>
        class Dequeue
        {
        public:
            void PushFront(T value);
            T PopFront();
            void PushBack(T value);
            T PopBack();
            Dequeue() = default;
            ~Dequeue() {
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

            void ReallocBuffer(size_type newCapacity);
            void IncSize();
            void DecSize();
            void IncModCapacity(T* &cur);
            void DecModCapacity(T* &cur);
        };

        template <typename T>
        void Dequeue<T>::PushFront(T value) {
            IncSize();
            *head_ = value;
            DecModCapacity(head_);
        }

        template <typename T>
        T Dequeue<T>::PopFront() {
            if (size_ == 0) return -1;
            DecSize();
            IncModCapacity(head_);
            return *head_;
        }

        template <typename T>
        void Dequeue<T>::PushBack(T value) {
            IncSize();
            *tail_ = value;
            IncModCapacity(tail_);
        }

        template <typename T>
        T Dequeue<T>::PopBack() {
            if (size_ == 0) return -1;
            DecSize();
            DecModCapacity(tail_);
            return *tail_;
        }

        template<typename T>
        void Dequeue<T>::ReallocBuffer(size_type newCapacity)
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
        }

        template<typename T>
        inline void Dequeue<T>::IncSize() {
            ++size_;
            if (size_ > capacity_) {
                ReallocBuffer(capacity_ * 2);
            }
        }

        template<typename T>
        inline void Dequeue<T>::DecSize() {
            --size_;
            size_type lowThresholdCapacity = capacity_ / 4;
            if (size_ < lowThresholdCapacity) {
                ReallocBuffer(lowThresholdCapacity);
            }
        }

        template<typename T>
        inline void Dequeue<T>::IncModCapacity(T* &cur) {
            if (++cur == right_)
                cur = buffer_;
        }

        template<typename T>
        inline void Dequeue<T>::DecModCapacity(T* &cur) {
            if (--cur < buffer_)
                cur = right_ - 1;
        }

    }

}

#endif // DEQUEUE_H_

