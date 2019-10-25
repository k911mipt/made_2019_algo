#ifndef SOLUTION_H_
#define SOLUTION_H_

#include <algorithm>
#include "priority_queue.h"
#include "window_queue.h"
#include "common.h"

namespace made {

    namespace solution {

        using stl::PriorityQueue;
        using std::vector;
        typedef stl::size_type size_type;
        using task::WindowElement;
        using stl::WindowStdQueue;

        void SolveWithHeap(int *arr, const size_type n, const size_type window_size, int* answers) {
            //PriorityQueue<WindowElement, vector<WindowElement>, task::lessWindowElement> pq;
            PriorityQueue<WindowElement, vector<WindowElement>, std::less<>> pq;
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

        void RMQSolution(int* arr, const size_type n, const size_type window_size, int* answers) {
            WindowStdQueue rmQueue(arr, window_size);
            int* end = arr + n;
            arr += window_size;
            for (; arr < end; ++arr, ++answers) {
                *answers = rmQueue.getMax();
                rmQueue.push(*arr);
            }
            *answers = rmQueue.getMax();
        }
    }

}

#endif // SOLUTION_H_