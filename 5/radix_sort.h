#ifndef RADIX_H_
#define RADIX_H_
#include <iostream>
#include <cassert>
#include "common.h"

namespace radix_sort {
#ifdef INLINE_SWAP
#undef INLINE_SWAP
#endif
#define INLINE_SWAP(a, b) {const T __SWAP_TEMP__ = a; a = b; b = __SWAP_TEMP__;}

    void insertion_sort(size_type *array, int offset, int end) {
        int x, y, temp;
        for (x = offset; x < end; ++x) {
            for (y = x; y > offset && array[y - 1] > array[y]; y--) {
                temp = array[y];
                array[y] = array[y - 1];
                array[y - 1] = temp;
            }
        }
    }

    template <class T>
    //void InsertionSort(T* begin, T* end, const TLess& isLess) {
    void InsertionSort(T* begin, T* end) {
        T* j;
        for (T* i = begin + 1; i < end; ++i) {
            j = i - 1;
            while ((j >= begin) && (*j > *(j + 1))) {
            //while ((j >= begin) && isLess(*(j + 1), *j)) {
                INLINE_SWAP(*j, *(j + 1));
                --j;
            }
        }
    }

    //const uint16_t size = 256; // 2^8
    ////const uint16_t size_literal = 0xFF;
    //const uint16_t size_literal = 255; // 2 ^ 8 - 1;
    //const uint16_t shift_change = 8;
    //const uint16_t shift_start = 32;
    const uint8_t shift_step = 8;
    const uint32_t size = 1 << shift_step; // 2^shift_step
    const uint32_t LOWER_BYTE = size - 1; // 2^shift_step - 1;
    const uint8_t shift_start = 32 / shift_step * shift_step;

    template <class T>
    void radix_sort(T *arr, T offset, T end, uint8_t shift) {
        assert(offset >= 0);
        T x;
        T last[size] = { 0 };
        T pointer[size];

        for (x = offset; x < end; ++x) {
            ++last[(arr[x] >> shift) & LOWER_BYTE];
        }

        last[0] += offset;
        pointer[0] = offset;
        for (x = 1; x < size; ++x) {
            pointer[x] = last[x - 1];
            last[x] += last[x - 1];
        }

        T value;
        T y;
        T temp;
        for (x = 0; x < size; ++x) {
            while (pointer[x] != last[x]) {
                value = arr[pointer[x]];
                y = (value >> shift) & LOWER_BYTE;
                while (x != y) {
                    temp = arr[pointer[y]];
                    arr[pointer[y]++] = value;
                    value = temp;
                    y = (value >> shift) & LOWER_BYTE;
                }
                arr[pointer[x]++] = value;
            }
        }

        if (shift > 0) {
            shift -= shift_step;
            for (x = 0; x < size; ++x) {
                T pX = pointer[x];
                temp = x > 0 ? pX - pointer[x - 1] : pointer[0] - offset;
                int temp_left = pX - temp;
                int temp_right = pX;
                if (temp > 64) {
                    radix_sort(arr, pX - temp, pX, shift);
                }
                else if (temp > 1) {
                    //insertion_sort(array, pointer[x] - temp, pointer[x]);
                    InsertionSort(arr + pX - temp, arr + pX);
                }
            }
        }
    }
#undef INLINE_SWAP
}

#endif // !RADIX_H_
