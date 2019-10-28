#ifndef CUSTOM_SORT_H_
#define CUSTOM_SORT_H_

#include "sort.h"

typedef uint32_t size_type;

inline void Sort(size_type* arr, size_type size) {
    for (size_type i = 0; i < size; i++)
        for (size_type j = i + 1; j < size; j++) {
            if (arr[i] < arr[j]) continue;
            int tmp = arr[j];
            arr[j] = arr[i];
            arr[i] = tmp;
        }
}

#endif // !CUSTOM_SORT_H_
