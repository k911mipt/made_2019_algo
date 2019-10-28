#ifndef CUSTOM_SORT_TEN_H_
#define CUSTOM_SORT_TEN_H_

#include <cassert>
#include "sort.h"

//typedef uint32_t size_type;
typedef int32_t size_type;

//#define INLINE_SWAP(a, b) (T tmp = (a); //\
//                           (a) = (b);\
//                           (b) = tmp;)
//#define INLINE_SWAP(a, b) T tmp = a; a = b; b = tmp;
#define INLINE_SWAP(a, b) {const T __SWAP_TEMP__ = a; a = b; b = __SWAP_TEMP__;}
//#define INLINE_SWAP(a, b) {const T __SWAP_TEMP__ = (a); (a) = (b); (b) = __SWAP_TEMP__;}

const int k = 2;
int lefts[k];
int rights[k];

namespace ten {

    template <class T>
    void ChoosePivotOfThree(T* first, T* last) {
        assert(last >= first);
        size_type length = last + 1 - first;
        size_type pivot_index = length / 20;
        T* pivot = first + pivot_index * 10;
        if (*pivot > *first)
            std::swap(*pivot, *first);
        if (*last > *first)
            std::swap(*last, *first);
        if (*pivot > *last)
            std::swap(*pivot, *last);
    }

    template <class T>
    T* Partition(T* first, T* last) {
        assert(end > first);
        ChoosePivotOfThree(begin, last);
        T pivot = *last;
        T* i = first;
        T* j = first;
        for (T* j = first; j < last; j += 10) {
            if (!(*j > pivot)) {
                std::swap(*i, *j);
                i += 10;
            }
        }
        std::swap(*i, *last);
        return i;
    }

    template <class T>
    void QuickSortInternal(T* begin, T* end) {
        if (begin < end - 1) {
            T* median = Partition(begin, end - 1);
            QuickSortInternal(begin, median - 10 + 1);
            QuickSortInternal(median + 10, end);
        }
    }

    template <class T>
    void QuickSort(T* begin, T* end) {
        QuickSortInternal(begin, end);
    }
}

#endif // !CUSTOM_SORT_TEN_H_
