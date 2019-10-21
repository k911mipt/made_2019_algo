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
            if (left >= heap_size)
                return;
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
            int j = (i - 1) / 2;
            while ((i > 0) && (comparator(begin[j], begin[i]))) {
                std::swap(begin[i], begin[j]);
                i = j;
                j = (i - 1) / 2;
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
        using std::greater;
        typedef stl::size_type size_type;

        int Sum(PriorityQueue<int, vector<int>, greater<int>> &pq) {
            int a = pq.Top();
            pq.Pop();
            a = a + pq.Top();
            pq.Pop();
            pq.Push(a);
            return a;
        }

        int Solve(int *arr, const size_type arr_length) {
            PriorityQueue<int, vector<int>, greater<int>> pq(arr, arr + arr_length);
            int operation_cost = 0;
            while (pq.Size() > 1) {
                operation_cost += Sum(pq);
            }
            return operation_cost;
        }
    }

}

int main() {
    std::ifstream input_stream("input.txt");
    std::cin.rdbuf(input_stream.rdbuf());
    int n;
    std::cin >> n;
    int *arr = new int[n];
    try {
        for (int i = 0; i < n; i++) {
            std::cin >> arr[i];
        }
        int answer = made::solution::Solve(arr, n);
        std::cout << answer << std::endl;
    } 
    catch (std::exception &e) {
        delete arr;
        std::cerr << e.what() << std::endl;
        throw;
    }
    catch (...){
        delete arr;
        throw;
    }
    delete arr;
    return EXIT_SUCCESS;
}