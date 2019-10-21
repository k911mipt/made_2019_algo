#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

namespace made {

    namespace stl {

        using std::vector;
        using std::less;
        typedef size_t size_type;

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
        void Heapify(PtrLike begin, PtrLike end, Comparator comparator = Comparator()) {
            int length = end - begin;
            for (int i = (length - 1) / 2; i >= 0; i--) {
                SiftDown(begin, end, i, comparator);
            }
        }

        template <class PtrLike, class Comparator = less<>>
        void PopHeap(PtrLike begin, PtrLike end, Comparator comparator = Comparator()) {
            end--;
            std::swap(*begin, *end);
            SiftDown(begin, end, 0, comparator);
        }

        template <class PtrLike, class Comparator = less<>>
        void PushHeap(PtrLike begin, PtrLike end, Comparator comparator = Comparator()) {
            int i = end - begin - 1;
            while (comparator(begin[(i - 1) / 2], begin[i])) {
                std::swap(begin[i], begin[(i - 1) / 2]);
                i = (i - 1) / 2;
            }
        }

        template<class T,
            class Container = vector<T>,
            class Comparator = std::less<typename Container::value_type>>
            class PriorityQueue
        {
        private:
            Container buffer_;
            Comparator comparator_;
        public:
            PriorityQueue() = default;

            template <class PtrLike>
            PriorityQueue(PtrLike begin, PtrLike end) : buffer_(begin, end), comparator_() {
                Heapify(buffer_.begin(), buffer_.end(), comparator_);
            }

            ~PriorityQueue() = default;

            inline void Push(T value) {
                buffer_.push_back(value);
                PushHeap(buffer_.begin(), buffer_.end(), comparator_);
            }
            inline void Pop() {
                PopHeap(buffer_.begin(), buffer_.end(), comparator_);
                buffer_.pop_back();
            }
            inline T Top() {
                return buffer_.front();
            }
            inline size_type Size() {
                return buffer_.size();
            }
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
            int maximum = *(left);
            for (; left < right; left++) {
                maximum = std::max(maximum, *left);
            }
            return maximum;
        }

        void SolveWithQueue(int* arr, const size_type n, const size_type window_size, int* answers) {
            int* left = arr;
            int* right = arr + window_size;
            int* last = arr + n;
            int maximum = FindMaximum(left, right);
            *answers = maximum;
            while (right < last) {
                if (*right >= maximum) {
                    maximum = *right;
                    left++;
                    right++;
                }
                else if (maximum == *left) {
                    left++;
                    right++;
                    maximum = FindMaximum(left, right);
                }
                else if (maximum > *left) {
                    left++;
                    right++;
                }
                answers++;
                *answers = maximum;
            }
        }

        void BetterBenchmark(int* arr, const size_type n, const size_type window_size, int* answers) {
            if (window_size > 1) {
                SolveWithQueue(arr, n, window_size, answers);
            }
            else {
                // Direct solution
                for (size_type i = 0; i <= n - window_size; i++, arr++, answers++) {
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
            //made::solution::BetterBenchmark(arr, n, windowSize, answers);
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