#include <iostream>
#include <fstream>
#include <vector>

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

        int Solve(int *arr, const size_type arr_length, const int weight_limit) {
            PriorityQueue<int> pq(arr, arr + arr_length);
            int numOperations = 0;
            while (pq.Size() > 0) {
                Eat(pq, weight_limit);
                numOperations++;
            }
            return numOperations;
        }
    }

}

int main() {
    std::ifstream input_stream("input.txt");
    std::cin.rdbuf(input_stream.rdbuf());
    int basket_size;
    std::cin >> basket_size;
    int *arr = new int[basket_size];
    for (int i = 0; i < basket_size; i++) {
        std::cin >> arr[i];
    }
    int weight_limit;
    std::cin >> weight_limit;
    int answer = made::solution::Solve(arr, basket_size, weight_limit);
    std::cout << answer << std::endl;
    delete arr;
    return EXIT_SUCCESS;
}