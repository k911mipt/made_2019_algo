#ifdef WINDOWS
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <iostream>
#include <fstream>
#include <vector>

#ifdef TEST
#include <algorithm>
#include <cassert>
#endif



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
                SiftDown(begin, end, best_index);
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
            inline int Size() {
                return buffer_.size();
            }
        };
    }

#ifdef TEST
    namespace sortSolution {

        void Eat(int &n, int *&arr, const int weightCapacity) {
            int numFruits = 0;
            int gatheredWeight = 0;
            while ((gatheredWeight <= weightCapacity) && (numFruits <= n)) {
                gatheredWeight += arr[numFruits];
                numFruits++;
            }
            numFruits--;
            std::sort(arr, arr + numFruits);
            int* cur = arr;
            for (int i = 0; i < numFruits; i++) {
                if (arr[i] == 1) {
                    cur++;
                }
                else {
                    arr[i] /= 2;
                }
            }
            n -= cur - arr;
            arr = cur;
        }

        int Solve(int n, int *arr, const int weightCapacity) {
            int numOperations = 0;
            while (n > 0) {
                std::sort(arr, arr + n, std::greater<int>());
                Eat(n, arr, weightCapacity);
                numOperations++;
            }

            return numOperations;
        }

    }
#endif

    namespace solution {

        using stl::PriorityQueue;
        using std::vector;
        typedef stl::size_type size_type;

        void Eat(PriorityQueue<int> &pq, const int weight_limit) {
            int numFruits = 0;
            int gatheredWeight = 0;
            vector<size_type> takenFruits;
            while (pq.Size() > 0) {
                int fruit = pq.Top();
                gatheredWeight += fruit;
                if (gatheredWeight > weight_limit) {
                    break;
                }
                pq.Pop();
                int eatenFruit = fruit / 2;
                if (0 != eatenFruit) {
                    takenFruits.push_back(eatenFruit);
                }
            }

            for (auto fruit : takenFruits) {
                pq.Push(fruit);
            }
        }

        int Solve(size_type n, int *arr, const int weight_limit) {
            PriorityQueue<int> pq(arr, arr + n);
            int numOperations = 0;
            while (pq.Size() > 0) {
                Eat(pq, weight_limit);
                numOperations++;
            }
            return numOperations;
        }
    }

}

using namespace made;

#ifdef TEST
void runTests(const stl::size_type n, int *arr, const int weightCapacity, int answer) {
    int *copy = new int[n];
    std::copy(arr, arr + n, copy);
    int naiveAnswer = sortSolution::Solve(n, copy, weightCapacity);
    assert((naiveAnswer == answer));
    delete[] copy;
}
#endif

int main() {
#ifdef WINDOWS
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
#endif // WINDOWS
    // load data
    std::ifstream in("input.txt");
    std::cin.rdbuf(in.rdbuf());
    int n;
    std::cin >> n;
    int *arr = new int[n];
    for (int i = 0; i < n; i++) {
        std::cin >> arr[i];
    }
    int weight_limit;
    std::cin >> weight_limit;
    // solve
    int answer = solution::Solve(n, arr, weight_limit);
#ifdef TEST
    runTests(n, arr, weight_limit, answer);
#endif
    // output
    std::cout << answer << std::endl;
    // clean and exit;
    delete arr;
    return EXIT_SUCCESS;
}