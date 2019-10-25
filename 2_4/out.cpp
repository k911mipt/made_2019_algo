/*
# Задание 2_4. Скользящий максимум

| Ограничение времени | 2 секунды                        |
|---------------------|----------------------------------|
| Ограничение памяти  | 5Mb                              |
| Ввод                | стандартный ввод или input.txt   |
| Вывод               | стандартный вывод или output.txt |

Дан массив целых чисел `A[0..n)`, `n` не превосходит `100 000`.
Также задан размер некоторого окна (последовательно расположенных элементов массива) в этом массиве `k`, `k <= n`.
Требуется для каждого положения окна (от `0` и до `n-k`) вывести значение максимума в окне.
Скорость работы `O(n log n)`, память `O(n)`.

## Формат ввода

Вначале вводится `n` - количество элементов массива. Затем вводится `n` строк со значением каждого элемента. Затем вводится `k` - размер окна.

## Формат вывода

Разделенные пробелом значения максимумов для каждого положения окна.
*/

#include <iostream>
#include <fstream>

// BEGIN INCLUDING solution.h
#ifndef SOLUTION_H_
#define SOLUTION_H_

#include <algorithm>

// BEGIN INCLUDING priority_queue.h
#ifndef PRIORITY_QUEUE_H_
#define PRIORITY_QUEUE_H_

#include <vector>

namespace made {

    namespace stl {
        using std::vector;
        using std::less;
        typedef size_t size_type;

        namespace heap {

            inline size_type Root(size_type index) {
                return (index - 1) / 2;
            }

            inline size_type Left(size_type index) {
                return (index * 2) + 1;
            }

            inline size_type Right(size_type index) {
                return (index * 2) + 2;
            }

            template <class PtrLike, class Comparator = less<>>
            void SiftDown(PtrLike begin, PtrLike end, size_type i, Comparator comparator = Comparator()) {
                size_type heap_size = end - begin;
                if (!heap_size || (Root(heap_size) <= i)) {
                    return;
                }
                size_type left = Left(i);
                size_type right = Right(i);
                size_type best_index = i;
                if (comparator(begin[best_index], begin[left])) {
                    best_index = left;
                }
                if (comparator(begin[best_index], begin[right])) {
                    best_index = right;
                }
                if (best_index != i) {
                    std::swap(begin[i], begin[best_index]);
                    SiftDown(begin, end, best_index, comparator);
                }
            }

            template <class PtrLike, class Comparator = less<>>
            void MakeHeap(PtrLike begin, PtrLike end, Comparator comparator = Comparator()) {
                size_type heap_size = end - begin;
                for (size_type i = Root(heap_size); i >= 0; --i) {
                    SiftDown(begin, end, i, comparator);
                }
            }

            template <class PtrLike, class Comparator = less<>>
            void PopHeap(PtrLike begin, PtrLike end, Comparator comparator = Comparator()) {
                --end;
                std::swap(*begin, *end);
                heap::SiftDown(begin, end, 0, comparator);
            }

            template <class PtrLike, class Comparator = less<>>
            void PushHeap(PtrLike begin, PtrLike end, Comparator comparator = Comparator()) {
                size_type i = end - begin - 1;
                if (i > 0) {
                    size_type root = heap::Root(i);
                    while (i && comparator(begin[root], begin[i])) {
                        std::swap(begin[i], begin[root]);
                        i = root;
                        root = heap::Root(i);
                    }
                }
            }
        }

        template<class T,
            class Container = vector<T>,
            class Comparator = std::less<typename Container::value_type>>
            class PriorityQueue
        {
        public:
            PriorityQueue() = default;

            template <class PtrLike>
            PriorityQueue(PtrLike begin, PtrLike end) : buffer_(begin, end), comparator_() {
                heap::MakeHeap(buffer_.begin(), buffer_.end(), comparator_);
            }

            ~PriorityQueue() = default;

            inline void Push(T value) {
                buffer_.push_back(value);
                heap::PushHeap(buffer_.begin(), buffer_.end(), comparator_);
            }
            inline void Pop() {
                heap::PopHeap(buffer_.begin(), buffer_.end(), comparator_);
                buffer_.pop_back();
            }
            inline T Top() {
                return buffer_.front();
            }
            inline size_type Size() {
                return buffer_.size();
            }
        private:
            Container buffer_;
            Comparator comparator_;
        };
    }
}

#endif // PRIORITY_QUEUE_H_
// END INCLUDING priority_queue.h


// BEGIN INCLUDING window_queue.h
#ifndef RMQUEUE_H_
#define RMQUEUE_H_

#include <deque>
#include <cassert>

// BEGIN INCLUDING common.h
#ifndef COMMON_H_
#define COMMON_H_

namespace made {

    namespace task {

        typedef size_t size_type;

        struct WindowElement {
            size_type index;
            int value;
            friend bool operator == (const WindowElement& lhs, const WindowElement& rhs) { return lhs.value == rhs.value; }
            friend bool operator < (const WindowElement& lhs, const WindowElement& rhs) { return lhs.value < rhs.value; }
            friend bool operator <= (const WindowElement& lhs, const WindowElement& rhs) { return lhs.value <= rhs.value; }
            friend bool operator > (const WindowElement& lhs, const WindowElement& rhs) { return lhs.value > rhs.value; }
            friend bool operator >= (const WindowElement& lhs, const WindowElement& rhs) { return lhs.value >= rhs.value; }
        };

        class lessWindowElement
        {
        public:
            bool operator() (const WindowElement& lhs, const WindowElement& rhs) { return lhs.value < rhs.value; }
        };
    }
}

#endif // COMMON_H_
// END INCLUDING common.h


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

// END INCLUDING window_queue.h


// BEGIN INCLUDING common.h
#ifndef COMMON_H_
#define COMMON_H_

namespace made {

    namespace task {

        typedef size_t size_type;

        struct WindowElement {
            size_type index;
            int value;
            friend bool operator == (const WindowElement& lhs, const WindowElement& rhs) { return lhs.value == rhs.value; }
            friend bool operator < (const WindowElement& lhs, const WindowElement& rhs) { return lhs.value < rhs.value; }
            friend bool operator <= (const WindowElement& lhs, const WindowElement& rhs) { return lhs.value <= rhs.value; }
            friend bool operator > (const WindowElement& lhs, const WindowElement& rhs) { return lhs.value > rhs.value; }
            friend bool operator >= (const WindowElement& lhs, const WindowElement& rhs) { return lhs.value >= rhs.value; }
        };

        class lessWindowElement
        {
        public:
            bool operator() (const WindowElement& lhs, const WindowElement& rhs) { return lhs.value < rhs.value; }
        };
    }
}

#endif // COMMON_H_
// END INCLUDING common.h


namespace made {

    namespace solution {

        using stl::PriorityQueue;
        using std::vector;
        typedef stl::size_type size_type;
        using task::WindowElement;
        using stl::WindowStdQueue;

        void SolveWithHeap(int *arr, const size_type n, const size_type window_size, int* answers) {
            //PriorityQueue<WindowElement, vector<WindowElement>, task::lessWindowElement> pq;
            PriorityQueue<WindowElement, vector<WindowElement>, std::less<>> pq;
            for (size_type i = 0; i < window_size; i++) {
                pq.Push({ i, arr[i] });
            }
            *answers = pq.Top().value;
            answers++;
            for (size_type i = window_size; i < n; i++) {
                size_type left_window_border = i - window_size + 1;
                // if index of maximum falls out left window border, 
                // we drop top element until maximum returns into window borders
                while (pq.Size() && (left_window_border > pq.Top().index)) {
                    pq.Pop();
                }
                pq.Push({ i, arr[i] });
                *answers = pq.Top().value;
                answers++;
            }
        }

        inline int FindMaximum(int* left, int* right) {
            int maximum = *left;
            for (; left < right; ++left) {
                maximum = std::max(maximum, *left);
            }
            return maximum;
        }

        inline int FindMaximum(int* left, int* right, int old_maximum) {
            int maximum = *left;
            for (; (left < right) && (maximum < old_maximum); ++left) {
                maximum = std::max(maximum, *left);
            }
            return maximum;
        }

        void SolveWithQueue(int* arr, const size_type n, const size_type window_size, int* answers) {
            // 1. Find maximum in first window 
            // 2. for each window moving we "push" a new number and pop and old one
            // if we popped number equal to current maximum, repeat maximum search
            int* left = arr;
            int* right = arr + window_size;
            int* last = arr + n;
            int maximum = FindMaximum(left, right);
            *answers = maximum;
            while (right < last) {
                if (*right >= maximum) {
                    maximum = *right;
                    ++left;
                    ++right;
                }
                else if (maximum == *left) {
                    ++left;
                    ++right;
                    maximum = FindMaximum(left, right, maximum);
                }
                else if (maximum > *left) {
                    ++left;
                    ++right;
                }
                ++answers;
                *answers = maximum;
            }
        }

        void BetterBenchmark(int* arr, const size_type n, const size_type window_size, int* answers) {
            if (window_size > 1) {
                SolveWithQueue(arr, n, window_size, answers);
            }
            else {
                // Direct solution
                for (size_type i = 0; i < n; i++, arr++, answers++) {
                    *answers = *arr;
                }
            }
        }

        void RMQSolution(int* arr, const size_type n, const size_type window_size, int* answers) {
            WindowStdQueue rmQueue(arr, window_size);
            int* end = arr + n;
            arr += window_size;
            for (; arr < end; ++arr, ++answers) {
                *answers = rmQueue.getMax();
                rmQueue.push(*arr);
            }
            *answers = rmQueue.getMax();
        }
    }

}

#endif // SOLUTION_H_
// END INCLUDING solution.h


using made::stl::size_type;

int main() {
    std::ifstream input_stream("input.txt");
    std::cin.rdbuf(input_stream.rdbuf());

    size_type n;
    std::cin >> n;
    int* arr = new int[n];
    try {
        for (size_type i = 0; i < n; i++) {
            std::cin >> arr[i];
        }
        size_type window_size;
        std::cin >> window_size;

        size_type answers_size = n - window_size + 1;
        int* answers = new int[answers_size];
        try {
            // https://contest.yandex.ru/contest/14656/run-report/23142719/
            // This solution takes 18ms and 536Kb memory on 11 test, which is better than heap solution (22ms and 904Kb)
            // Though it has O(n^2) complexity and should not pass time limits if given array looks like `a[i] = n - i`
            //made::solution::BetterBenchmark(arr, n, window_size, answers);

            made::solution::SolveWithHeap(arr, n, window_size, answers);
            //made::solution::RMQSolution(arr, n, window_size, answers);
            
            for (size_type i = 0; i < answers_size; i++) {
                std::cout << answers[i] << " ";
            }
        }
        catch (...) {
            delete answers;
            throw;
        }
        delete answers;
    }
    catch (...) {
        delete arr;
        throw;
    }
    delete arr;
    return EXIT_SUCCESS;
}
