#ifdef WINDOWS
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <iostream>
#include <fstream>
#include <algorithm>
#include <cassert>
#include <vector>
#include <queue>
#include <valarray>

namespace made {

    namespace stl {
        using std::vector;
        using std::less;
        ///*typedef size_t size_type;*/
        typedef int size_type;

        template <class PtrLike, class Comparator = less<>>
        void siftDown(PtrLike begin, PtrLike end, size_type i, Comparator comparator = Comparator()) {
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
                siftDown(begin, end, best_index);
            }
        }

        template <class PtrLike, class Comparator = less<>>
        void heapify(PtrLike begin, PtrLike end, Comparator comparator = Comparator()) {
            int length = end - begin;
            for (int i = (length - 1) / 2; i >= 0; i--) {
                siftDown(begin, end, i, comparator);
            }
        }

        template <class PtrLike, class Comparator = less<>>
        void popHeap(PtrLike begin, PtrLike end, Comparator comparator = Comparator()) {
            end--;
            std::swap(*begin, *end);
            siftDown(begin, end, 0, comparator);
        }

        template <class PtrLike, class Comparator = less<>>
        void pushHeap(PtrLike begin, PtrLike end, Comparator comparator = Comparator()) {
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
            Container _buffer;
            Comparator _comparator;
        public:
            PriorityQueue() = default;
            template <class PtrLike>
            PriorityQueue(PtrLike begin, PtrLike end) : _buffer(begin, end), _comparator() {
                heapify(_buffer.begin(), _buffer.end(), _comparator);
            }
            ~PriorityQueue() = default;

            void push(T value) {
                _buffer.push_back(value);
                pushHeap(_buffer.begin(), _buffer.end(), _comparator);
            }
            void pop() {
                popHeap(_buffer.begin(), _buffer.end(), _comparator);
                _buffer.pop_back();
            }
            T top() {
                return _buffer.front();
            }
            int size() {
                return _buffer.size();
            }
        };
    }

    namespace queueSolution {
        inline int findMaximum(int* left, int* right) {
            int maximum = *(left);
            for (; left < right; left++) {
                maximum = std::max(maximum, *left);
            }
            return maximum;
        }

        void solveWithQueue(const int n, int* arr, const int windowSize, int* answers) {
            int* left = arr;
            int* right = arr + windowSize;
            int* last = arr + n;
            int maximum = findMaximum(left, right);
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
                    maximum = findMaximum(left, right);
                }
                else if (maximum > *left) {
                    left++;
                    right++;
                }
                answers++;
                *answers = maximum;
            }
        }

        void solve(const int n, int* arr, const int windowSize, int* answers) {
            if (windowSize > 1) {
                solveWithQueue(n, arr, windowSize, answers);
            }
            else {
                // Direct solution
                for (int i = 0; i <= n - windowSize; i++, arr++, answers++) {
                    *answers = *arr;
                }
            }
        }
    }

    namespace sortSolution {
        void eat(int &n, int* &arr, const int weightCapacity) {
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

        int solve(int n, int* arr, const int weightCapacity) {
            //std::sort(arr, arr + n, greater<int>());
            int numOperations = 0;
            while (n > 0) {
                std::sort(arr, arr + n, std::greater<int>());
                eat(n, arr, weightCapacity);
                numOperations++;
            }


            return numOperations;
        }
    }

    namespace heapVectorSolution {
        void eat(std::vector<int> &vec, const int weightCapacity) {
            int numFruits = 0;
            int gatheredWeight = 0;
            std::vector<int> takenFruits;
            while ((gatheredWeight <= weightCapacity) && (numFruits < vec.size())) {
                int fruit = vec.front();
                gatheredWeight += fruit;
                if (gatheredWeight <= weightCapacity) {
                    std::pop_heap(vec.begin(), vec.end());
                    vec.pop_back();
                    int eaten = fruit / 2;
                    if (eaten) {
                        takenFruits.push_back(eaten);
                    }
                }
            };

            for (auto fruit : takenFruits) {
                vec.push_back(fruit);
                std::push_heap(vec.begin(), vec.end());
            }
        }
        int solve(int n, int* arr, const int weightCapacity) {
            std::vector<int> vec(arr, arr + n);
            std::make_heap(arr, arr + n);
            std::make_heap(vec.begin(), vec.end());

            int *copy = new int[n];
            std::copy(arr, arr + n, copy);
            //stl::heapify(copy, copy + n);
            std::vector<int> vec2(copy, copy + n);

            int *copy2 = new int[n];
            std::copy(copy, copy + n, copy2);
            //{ //std::pop_heap
            //int temp = *copy2;
            //*copy2 = *(copy2 + n - 1);
            //*(copy2 + n - 1) = temp;
            //stl::heapify(copy2, copy2 + n - 1, 0);
            //stl::popHeap(copy2, copy2 + n);
            //stl::heapify(copy2, copy2 + n - 1,)
            //}
            std::vector<int> vec3(copy2, copy2 + n);

            //stl::make_heap(copy + 1, copy + n);
            //std::vector<int> vec3(copy + 1, copy + n);
            std::pop_heap(vec.begin(), vec.end());
            //stl::make_heap(vec.begin(), vec.end());

            //stl::pushHeap(copy2, copy2 + n);
            //std::push_heap(copy2, copy2 + n);
            //stl::heapify(copy2, copy2 + n, 0);
            std::vector<int> vec4(copy2, copy2 + n);


            int numOperations = 0;
            while (vec.size() > 0) {
                eat(vec, weightCapacity);
                numOperations++;
            }
            //std::make_heap(arr, arr + n);
            return numOperations;
        }
    }

    namespace heapSolution {
        using std::priority_queue;
        using std::vector;
        typedef stl::size_type size_type;
        using std::greater;
        using std::less;

        void eat(priority_queue<int, vector<int>, less<int>>  &pq, const int weightCapacity) {
            int numFruits = 0;
            int gatheredWeight = 0;
            //pq.top()

            std::vector<int> takenFruits;
            while ((gatheredWeight <= weightCapacity) && (numFruits < pq.size())) {
                int fruit = pq.top();
                gatheredWeight += fruit;
                if (gatheredWeight <= weightCapacity) {
                    pq.pop();
                    int eaten = fruit / 2;
                    if (eaten) {
                        takenFruits.push_back(eaten);
                    }
                }
            };
            for (auto fruit : takenFruits) {
                pq.push(fruit);
            }
            //pq.insert(pq.end(), takenFruits.begin(), takenFruits.end());
            //std::push_heap(pq.begin(), pq.end());
        }

        int solve(int n, int* arr, const int weightCapacity) {
            priority_queue<int, vector<int>, less<int>> pq(arr, arr + n);
            int numOperations = 0;
            while (pq.size() > 0) {
                eat(pq, weightCapacity);
                numOperations++;
            }
            return numOperations;
        }
    }

    namespace heapStlSolution {
        using stl::PriorityQueue;
        using std::vector;
        using std::priority_queue;
        typedef stl::size_type size_type;

        void eat(PriorityQueue<int> &pq, const int weightCapacity) {
            int numFruits = 0;
            int gatheredWeight = 0;
            vector<size_type> takenFruits;
            while (pq.size() > 0) {
                int fruit = pq.top();
                gatheredWeight += fruit;
                if (gatheredWeight > weightCapacity) {
                    break;
                }
                pq.pop();
                int eatenFruit = fruit / 2;
                if (0 != eatenFruit) {
                    takenFruits.push_back(eatenFruit);
                }
            }

            for (auto fruit : takenFruits) {
                pq.push(fruit);
            }
        }

        int solve(size_type n, int* arr, const int weightCapacity) {
            PriorityQueue<int> pq(arr, arr + n);
            int numOperations = 0;
            while (pq.size() > 0) {
                eat(pq, weightCapacity);
                numOperations++;
            }
            return numOperations;
        }
    }
}

using namespace made;

#ifdef TEST
void runTests(const stl::size_type n, int* arr, const int weightCapacity, int answer) {
    int *copy = new int[n];
    std::copy(arr, arr + n, copy);
    int naiveAnswer = sortSolution::solve(n, copy, weightCapacity);

    naiveAnswer = heapSolution::solve(n, arr, weightCapacity);
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
    int* arr = new int[n];
    for (int i = 0; i < n; i++) {
        std::cin >> arr[i];
    }
    int weightCapacity;
    std::cin >> weightCapacity;
    // solve
    int answer = heapStlSolution::solve(n, arr, weightCapacity);
#ifdef TEST
    runTests(n, arr, weightCapacity, answer);
#endif
    // output
    std::cout << answer << std::endl;
    // clean and exit;
    delete arr;
    return EXIT_SUCCESS;
}