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

#pragma region "solution.h"
#ifndef SOLUTION_H_
#define SOLUTION_H_


#pragma region "priority_queue.h"
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
                int left = i * 2 + 1;
                int right = left + 1;
                int heap_size = end - begin;
                int best_index = i;
                if ((left < heap_size) && comparator(begin[best_index], begin[left])) {
                    best_index = left;
                }
                if ((right < heap_size) && comparator(begin[best_index], begin[right])) {
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
                for (size_type i = heap_size / 2; i > 0; --i) {
                    SiftDown(begin, end, i - 1, comparator);
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
#pragma endregion "priority_queue.h"


namespace made {

    namespace solution {

        typedef stl::size_type size_type;

        struct WindowElement {
            size_type index;
            int value;
        };

        class lessWindowElement
        {
        public:
            bool operator() (const WindowElement& lhs, const WindowElement& rhs) { return lhs.value < rhs.value; }
        };

        void SolveWithHeap(WindowElement *arr, const size_type n, const size_type window_size, int* answers) {
            stl::PriorityQueue<WindowElement, std::vector<WindowElement>, lessWindowElement> pq(arr, arr + window_size);
            *answers = pq.Top().value;
            answers++;
            for (size_type i = window_size; i < n; i++) {
                size_type left_window_border = i - window_size + 1;
                // if index of maximum falls out left window border, 
                // we drop top element until maximum returns into window borders
                while (pq.Size() && (left_window_border > pq.Top().index)) {
                    pq.Pop();
                }
                pq.Push(arr[i]);
                *answers = pq.Top().value;
                answers++;
            }
        }
    }
}

#endif // SOLUTION_H_
#pragma endregion "solution.h"


using namespace made::solution;

int main() {
    std::ifstream input_stream("input.txt");
    std::cin.rdbuf(input_stream.rdbuf());

    size_type n;
    std::cin >> n;
    WindowElement* arr = new WindowElement[n];
    try {
        for (size_type i = 0; i < n; i++) {
            arr[i].index = i;
            std::cin >> arr[i].value;
        }
        size_type window_size;
        std::cin >> window_size;
        size_type answers_size = n - window_size + 1;
        int* answers = new int[answers_size];
        try {
            SolveWithHeap(arr, n, window_size, answers);
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
