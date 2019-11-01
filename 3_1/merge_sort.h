#ifndef MERGE_SORT_H_
#define MERGE_SORT_H_

#include <algorithm>

namespace made {

    namespace stl {

        typedef size_t size_type;

        template <class T>
        void Merge(T* left, T* mid, T* right, T* buffer) {
            T* left_cur = left;
            T* mid_cur = mid;
            T* buffer_cur = buffer;
            while ((left_cur < mid) && (mid_cur < right)) {
                if (*left_cur < *mid_cur) {
                    *buffer_cur = *left_cur;
                    ++buffer_cur;
                    ++left_cur;
                }
                else {
                    *buffer_cur = *mid_cur;
                    ++buffer_cur;
                    ++mid_cur;
                }
            }
            while (left_cur < mid) {
                *buffer_cur = *left_cur;
                ++buffer_cur;
                ++left_cur;
            }
            while (mid_cur < right) {
                *buffer_cur = *mid_cur;
                ++buffer_cur;
                ++mid_cur;
            }
            std::copy(buffer, buffer_cur, left);
        }

        template <class T>
        void MergeSortRecursive(T* begin, T* end, T* buffer) {
            if (begin + 1 < end) {
                T* mid = begin + (end - begin) / 2;
                MergeSortRecursive(begin, mid, buffer);
                MergeSortRecursive(mid, end, buffer);
                Merge(begin, mid, end, buffer);
            }
        }

        template <class T>
        void MergeSortIterative(T* left, T* right, T* buffer) {
            size_type length = right - left;
            for (size_type i = 1; i < length; i *= 2) {
                for (size_type j = 0; j < length - i; j += 2 * i) {
                    Merge(left + j, left + j + i, left + std::min(j + 2 * i, length), buffer);
                }
            }
        }

        template <class T>
        void MergeSort(T* begin, T* end, T* buffer = nullptr) {
            if (!buffer) {
                buffer = new T[end - begin];
                MergeSortIterative(begin, end, buffer);
                delete[] buffer;
            }
            else {
                MergeSortIterative(begin, end, buffer);
            }
        }
    }
}

#endif // MERGE_SORT_H_