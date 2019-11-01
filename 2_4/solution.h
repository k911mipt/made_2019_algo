#ifndef SOLUTION_H_
#define SOLUTION_H_

#include "priority_queue.h"

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