/*
# ������� 2_4. ���������� ��������

| ����������� ������� | 2 �������                        |
|---------------------|----------------------------------|
| ����������� ������  | 5Mb                              |
| ����                | ����������� ���� ��� input.txt   |
| �����               | ����������� ����� ��� output.txt |

��� ������ ����� ����� `A[0..n)`, `n` �� ����������� `100 000`.
����� ����� ������ ���������� ���� (��������������� ������������� ��������� �������) � ���� ������� `k`, `k <= n`.
��������� ��� ������� ��������� ���� (�� `0` � �� `n-k`) ������� �������� ��������� � ����. 
�������� ������ `O(n log n)`, ������ `O(n)`.

## ������ �����

������� �������� `n` - ���������� ��������� �������. ����� �������� `n` ����� �� ��������� ������� ��������. ����� �������� `k` - ������ ����.

## ������ ������

����������� �������� �������� ���������� ��� ������� ��������� ����.
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

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

    namespace solution {

        using stl::PriorityQueue;
        using std::vector;
        typedef stl::size_type size_type;

        struct QueueElement {
            size_type index;
            int value;
        };

        class compareQueueElements
        {
        public:
            bool operator() (const QueueElement& lhs, QueueElement& rhs) { return lhs.value < rhs.value; }
        };

        void SolveWithHeap(int *arr, const size_type n, const size_type window_size, int* answers) {
            PriorityQueue<QueueElement, vector<QueueElement>, compareQueueElements> pq;
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
    }

}

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