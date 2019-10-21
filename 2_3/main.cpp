#include <iostream>
#include <fstream>
#include <vector>
#include <queue>

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

        struct trainSchedule {
            int arrive_time;
            int departure_time;
        };

        int Solve(trainSchedule *arr, const size_type n) {
            PriorityQueue<int, vector<int>, std::greater<int>> pq;
            size_t maxDocks = 1;
            pq.Push(arr[0].departure_time);
            for (size_type i = 1; i < n; i++) {
                trainSchedule schedule = arr[i];
                while ((pq.Size() > 0) && (pq.Top() < schedule.arrive_time)) {
                    pq.Pop();
                }
                pq.Push(schedule.departure_time);
                maxDocks = std::max(maxDocks, pq.Size());
            }
            return maxDocks;
        }
    }

}

using namespace made::solution;

int main() {
    std::ifstream input_stream("input.txt");
    std::cin.rdbuf(input_stream.rdbuf());
    int n;
    std::cin >> n;
    trainSchedule *arr = new trainSchedule[n];
    try {
        for (int i = 0; i < n; i++) {
            std::cin >> arr[i].arrive_time >> arr[i].departure_time;
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