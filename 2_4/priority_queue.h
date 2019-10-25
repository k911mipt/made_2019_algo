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