#ifndef MADE_SORT_H_
#define MADE_SORT_H_

#include "sort.h"
#include <cstdint>
#include <cassert>
#include <iostream>
#include <cstring>

#ifndef COMMON_SORT_H_
namespace made {
    typedef uint8_t Digit;
    //typedef uint16_t Digit;
    typedef uint32_t Element; // 0..10^9 value range
    typedef uint32_t ElementCounter; // 0..10^7 array size
    const uint8_t ELEMENT_SIZE = sizeof(Element);
    const uint8_t DIGIT_SIZE = sizeof(Digit);
    const uint32_t MAX_DIGIT_COUNT = 1 << (DIGIT_SIZE * 8);
    // TODO which type to use for mask? 32 or 8 bit? Check how different mask types applies
    const Element LOWER_DIGIT_MASK = MAX_DIGIT_COUNT - 1; // digit = 1 byte: 0xFF, digit = 2 byte: 0xFFFF, etc.;
}
#endif
// digit_i^r(x) = [x/r^i] mod R   i-я цифра числа, состоящая из r бит
//#define DIGIT(x, i, R) ((x >> i * R) & (R - 1))

namespace made {

    void LSDSort(Element* arr, const ElementCounter size)
    {
        //for (ElementCounter i = size / 10; i < size; i += size / 10) {
        //made::stl::FindKOrderStatistic(arr, arr + size, size / 2);
        //}
        Element *buffer = new Element[size];
        const uint8_t step = sizeof(Element) / DIGIT_SIZE;
        const uint8_t num_digits = step;
        ElementCounter digit_counters[num_digits][MAX_DIGIT_COUNT] = { 0 };
        ElementCounter(&digit_start_indexes)[num_digits][MAX_DIGIT_COUNT] = digit_counters; // Reference for further readability

        Element constant_bits = 0;
        Element first = *arr;
        for (Element* arr_iterator = arr + 1; arr_iterator < arr + size; ++arr_iterator) {
            constant_bits |= (*arr^*arr_iterator);
        }
        /* Number is stored in reverse order
         * example: uint32_t number 439041101 located at 0x00000000
         * 439041101 = 0x1A2B3C4D
         * uint32_t 0x00000000 : 0x1A2B3C4D
         * uint8_t  0x00000000 : 0x4D
         * uint8_t  0x00000001 : 0x3C
         * uint8_t  0x00000002 : 0x2B
         * uint8_t  0x00000003 : 0x1A
         */
        const Digit *digit_ptrs[num_digits];
        for (int j = 0; j < num_digits; ++j) {
            digit_ptrs[j] = reinterpret_cast<const Digit*>(arr) + j;
        }
        // loop over all of the bytes in the sorce, incrementing the index
        const Digit* digit_ptr = reinterpret_cast<const Digit*>(arr);
        for (ElementCounter i_element = 0; i_element < size; ++i_element) {
            //for (int j = 0; j < num_digits; ++j) {
            //    ++digit_counters[j][*digit_ptrs[j]];
            //    digit_ptrs[j] += step;
            //}
            ++digit_counters[0][*digit_ptr++];
            ++digit_counters[1][*digit_ptr++];
            ++digit_counters[2][*digit_ptr++];
            ++digit_counters[3][((*digit_ptr++) & 0x3F)];
        }

        // Count start indexes for each digit. Store in digit_start_indexes
        unsigned start_indexes[num_digits] = { 0 };
        ElementCounter temp_counts[num_digits];
        //ElementCounter temp;
        for (ElementCounter i_e = 0; i_e < MAX_DIGIT_COUNT; ++i_e) { // i_element
            for (int j_d = 0; j_d < num_digits; ++j_d) { // j_digit
                temp_counts[j_d] = digit_counters[j_d][i_e];
                digit_start_indexes[j_d][i_e] = start_indexes[j_d];
                start_indexes[j_d] += temp_counts[j_d];
            }
            //temp_counts[0] = digit_counters[0][i_e];
            //digit_start_indexes[0][i_e] = start_indexes[0];
            //start_indexes[0] += temp_counts[0];

            //temp_counts[1] = digit_counters[1][i_e];
            //digit_start_indexes[1][i_e] = start_indexes[1];
            //start_indexes[1] += temp_counts[1];

            //temp_counts[2] = digit_counters[2][i_e];
            //digit_start_indexes[2][i_e] = start_indexes[2];
            //start_indexes[2] += temp_counts[2];

            //temp_counts[3] = digit_counters[3][i_e];
            //digit_start_indexes[3][i_e] = start_indexes[3];
            //start_indexes[3] += temp_counts[3];
        }

        // Sorting and placing results into buffer
        Element* arr_iterator = arr;
        Digit* digit_iterator;
        for (uint8_t i_digit = 0; i_digit < num_digits; ++i_digit) {
            digit_iterator = reinterpret_cast<Digit*>(arr_iterator) + i_digit;
            for (ElementCounter i = 0; i < size; ++i, ++arr_iterator, digit_iterator += step)
            {
                uint32_t *value_ptr = digit_start_indexes[i_digit] + *digit_iterator;
                // p & 0x3FFFFFFF - зануление старших двух битов
                // p | 0xc0000000 - присваивание единицы в старшие двух битов
                *arr_iterator |= 0xc0000000;
                *arr_iterator &= 0x3FFFFFFF;
                buffer[(*value_ptr)++] = *arr_iterator;
                //(*value_ptr)++;
            }
            arr_iterator = buffer; buffer = arr; arr = arr_iterator; // swapping pointers for next sorting iteration
            //arr_iterator = arr;
        }
        delete[] buffer;
    }

    void LSDThreeSort(Element* arr, const ElementCounter size)
    {
        Element *buffer = new Element[size];
        //const uint8_t step = sizeof(Element) / DIGIT_SIZE;
        const uint8_t num_digits = 3;
        const uint32_t max_digit_count = 1 << 10;
        ElementCounter digit_counters[num_digits][max_digit_count] = { 0 };// std::memset(digit_counters, 0, sizeof(digit_counters));
        ElementCounter(&digit_start_indexes)[num_digits][max_digit_count] = digit_counters; // Reference for further readability

        Element* arr_iterator = arr;
        for (unsigned i_element = 0; i_element < size; ++i_element, ++arr_iterator) {
            Element value = *arr_iterator;
            assert(value < 1000000001);
            ++digit_counters[0][(value & 0x3FF)];
            ++digit_counters[1][(value >> 10 & 0x3FF)];
            ++digit_counters[2][(value >> 20)];
        }

        unsigned start_indexes[num_digits] = { 0 };
        ElementCounter temp_counts[num_digits];
        for (unsigned i_e = 0; i_e < max_digit_count; ++i_e) { // i_element
            //for (int j_d = 0; j_d < num_digits; ++j_d) { // j_digit
            //    temp_counts[j_d] = digit_counters[j_d][i_e];
            //    digit_start_indexes[j_d][i_e] = start_indexes[j_d];
            //    start_indexes[j_d] += temp_counts[j_d];
            //}
            temp_counts[0] = digit_counters[0][i_e];
            digit_start_indexes[0][i_e] = start_indexes[0];
            start_indexes[0] += temp_counts[0];

            temp_counts[1] = digit_counters[1][i_e];
            digit_start_indexes[1][i_e] = start_indexes[1];
            start_indexes[1] += temp_counts[1];

            temp_counts[2] = digit_counters[2][i_e];
            digit_start_indexes[2][i_e] = start_indexes[2];
            start_indexes[2] += temp_counts[2];

            //temp_counts[3] = digit_counters[3][i_e];
            //digit_start_indexes[3][i_e] = start_indexes[3];
            //start_indexes[3] += temp_counts[3];
        }

        arr_iterator = arr;
        for (ElementCounter i = 0; i < size; ++i, ++arr_iterator) {
            Element value = *arr_iterator;
            assert(value < 1000000001);
            uint32_t *value_ptr = digit_start_indexes[0] + (value & 0x3FF);
            buffer[(*value_ptr)++] = *arr_iterator;
            //(*value_ptr)++;
        }
        arr_iterator = buffer; buffer = arr; arr = arr_iterator; // swapping pointers for next sorting iteration
        for (ElementCounter i = 0; i < size; ++i, ++arr_iterator) {
            Element value = *arr_iterator;
            assert(value < 1000000001);
            uint32_t *value_ptr = digit_start_indexes[1] + ((value >> 10) & 0x3FF);
            buffer[(*value_ptr)++] = *arr_iterator;
            //(*value_ptr)++;
        }
        arr_iterator = buffer; buffer = arr; arr = arr_iterator; // swapping pointers for next sorting iteration
        for (ElementCounter i = 0; i < size; ++i, ++arr_iterator) {
            Element value = *arr_iterator;
            assert(value < 1000000001);
            uint32_t *value_ptr = digit_start_indexes[2] + (value >> 20);
            buffer[(*value_ptr)++] = *arr_iterator;
            //(*value_ptr)++;
        }
        memcpy(arr, buffer, size * 4);
        delete[] buffer;
    }

    void LSDPointerSort(Element* arr, const ElementCounter size)
    {
        Element *buffer = new Element[size];
        const uint8_t step = sizeof(Element) / DIGIT_SIZE;
        const uint8_t num_digits = step;
        ElementCounter digit_counters[num_digits][MAX_DIGIT_COUNT] = { 0 };
        ElementCounter(&digit_start_indexes)[num_digits][MAX_DIGIT_COUNT] = digit_counters; // Reference for further readability
        const Element *arr_end = arr + size;
        const Element *buffer_end = buffer + size;
        /* Number is stored in reverse order
         * example: uint32_t number 439041101 located at 0x00000000
         * 439041101 = 0x1A2B3C4D
         * uint32_t 0x00000000 : 0x1A2B3C4D
         * uint8_t  0x00000000 : 0x4D
         * uint8_t  0x00000001 : 0x3C
         * uint8_t  0x00000002 : 0x2B
         * uint8_t  0x00000003 : 0x1A
         */
        const Digit *digit_ptrs[num_digits];
        for (int j = 0; j < num_digits; ++j) {
            digit_ptrs[j] = reinterpret_cast<const Digit*>(arr) + j;
        }
        // loop over all of the bytes in the sorce, incrementing the index
        const Digit* digit_ptr = reinterpret_cast<const Digit*>(arr);
        const Digit* const digit_end = reinterpret_cast<const Digit*>(arr_end);
        for (; digit_ptr < digit_end;) {
            //for (int j = 0; j < num_digits; ++j) {
            //    ++digit_counters[j][*digit_ptrs[j]];
            //    digit_ptrs[j] += step;
            //}
            ++digit_counters[0][*digit_ptr++];
            ++digit_counters[1][*digit_ptr++];
            ++digit_counters[2][*digit_ptr++];
            ++digit_counters[3][((*digit_ptr++) & 0x3F)];
        }

        // Count start indexes for each digit. Store in digit_start_indexes
        unsigned start_indexes[num_digits] = { 0 };
        ElementCounter temp_counts[num_digits];
        //ElementCounter temp;
        for (ElementCounter i_e = 0; i_e < MAX_DIGIT_COUNT; ++i_e) { // i_element
            for (int j_d = 0; j_d < num_digits; ++j_d) { // j_digit
                temp_counts[j_d] = digit_counters[j_d][i_e];
                digit_start_indexes[j_d][i_e] = start_indexes[j_d];
                start_indexes[j_d] += temp_counts[j_d];
            }
            //temp_counts[0] = digit_counters[0][i_e];
            //digit_start_indexes[0][i_e] = start_indexes[0];
            //start_indexes[0] += temp_counts[0];

            //temp_counts[1] = digit_counters[1][i_e];
            //digit_start_indexes[1][i_e] = start_indexes[1];
            //start_indexes[1] += temp_counts[1];

            //temp_counts[2] = digit_counters[2][i_e];
            //digit_start_indexes[2][i_e] = start_indexes[2];
            //start_indexes[2] += temp_counts[2];

            //temp_counts[3] = digit_counters[3][i_e];
            //digit_start_indexes[3][i_e] = start_indexes[3];
            //start_indexes[3] += temp_counts[3];
        }

        // Sorting and placing results into buffer
        Element* arr_iterator = arr;
        Digit* digit_iterator;
        for (uint8_t i_digit = 0; i_digit < num_digits; ++i_digit) {
            digit_iterator = reinterpret_cast<Digit*>(arr_iterator) + i_digit;
            for (ElementCounter i = 0; i < size; ++i, ++arr_iterator, digit_iterator += step) {
                uint32_t *value_ptr = digit_start_indexes[i_digit] + *digit_iterator;
                buffer[(*value_ptr)++] = *arr_iterator;
            }
            arr_iterator = buffer; buffer = arr; arr = arr_iterator; // swapping pointers for next sorting iteration
        }
        delete[] buffer;
    }

    void LSDStep(Element* arr, const ElementCounter size, const uint8_t num_digits, Element *buffer)
    {
        const uint8_t step = sizeof(Element) / DIGIT_SIZE;
        const uint8_t MAX_NUM_DIGITS = step;
        ElementCounter digit_counters[MAX_NUM_DIGITS][MAX_DIGIT_COUNT] = { 0 };
        ElementCounter(&digit_start_indexes)[MAX_NUM_DIGITS][MAX_DIGIT_COUNT] = digit_counters; // Reference for further readability

         // Filling the buckets
        const Digit* digit_ptrs[MAX_NUM_DIGITS];
        for (int j = 0; j < num_digits; ++j) {
            digit_ptrs[j] = reinterpret_cast<const Digit*>(arr) + j;
        }
        //const Digit* digit_ptr = reinterpret_cast<const Digit*>(arr);
        for (ElementCounter i_element = 0; i_element < size; ++i_element) {
            for (int j = 0; j < num_digits; ++j) {
                ++digit_counters[j][*digit_ptrs[j]];
                digit_ptrs[j] += step;
            }
        }

        // Count start indexes for each digit. Store in digit_start_indexes
        unsigned start_indexes[MAX_NUM_DIGITS] = { 0 };
        ElementCounter temp_counts[MAX_NUM_DIGITS];

        for (int j_d = 0; j_d < num_digits; ++j_d) { // j_digit
            for (ElementCounter i_e = 0; i_e < MAX_DIGIT_COUNT; ++i_e) { // i_element
                // TODO размотать цикл?
                temp_counts[j_d] = digit_counters[j_d][i_e];
                digit_start_indexes[j_d][i_e] = start_indexes[j_d];
                start_indexes[j_d] += temp_counts[j_d];
            }
        }

        // Sorting and placing results into buffer
        Element* arr_iterator = arr;
        Digit* digit_iterator;
        for (uint8_t i_digit = 0; i_digit < num_digits; ++i_digit) {
            digit_iterator = reinterpret_cast<Digit*>(arr_iterator) + i_digit;
            for (ElementCounter i = 0; i < size; ++i, ++arr_iterator, digit_iterator += step) {
                uint32_t *value_ptr = digit_start_indexes[i_digit] + *digit_iterator;
                assert(*value_ptr < size);
                buffer[(*value_ptr)++] = *arr_iterator;
            }
            arr_iterator = buffer; buffer = arr; arr = arr_iterator; // swapping pointers for next sorting iteration
        }
        if (num_digits % 2) {
            arr_iterator = buffer; buffer = arr; arr = arr_iterator;
            memcpy(arr, buffer, size * 4);
        }

    }

    void insertion_sort(Element* array, ElementCounter offset, ElementCounter end) {
        ElementCounter x, y;
        Element temp;
        for (x = offset; x < end; ++x) {
            for (y = x; y > offset && array[y - 1] > array[y]; y--) {
                temp = array[y];
                array[y] = array[y - 1];
                array[y - 1] = temp;
            }
        }
    }



    void _radix_sort_lsb(Element *begin, Element *end, Element *begin1, Element maxshift)
    {
        Element *end1 = begin1 + (end - begin);

        //for (Element shift = 0; shift <= maxshift; shift += 8)
        //{
        //    size_t count[0x100] = {};
        //    for (Element *p = begin; p != end; p++)
        //        count[(*p >> shift) & 0xFF]++;
        //    unsigned *bucket[0x100], *q = begin1;
        //    for (int i = 0; i < 0x100; q += count[i++])
        //        bucket[i] = q;
        //    for (Element *p = begin; p != end; p++)
        //        *bucket[(*p >> shift) & 0xFF]++ = *p;
        //    std::swap(begin, begin1);
        //    std::swap(end, end1);
        //}
        size_t count[0x100];
        Element *bucket[0x100];
        for (Element shift = 0; shift <= maxshift; shift += 8)
        {
            std::memset(count, 0, sizeof(count));
            for (Element *p = begin; p != end; p++)
                count[(*p >> shift) & 0xFF]++;
            Element *q = begin1;
            for (int i = 0; i < 0x100; q += count[i++])
                bucket[i] = q;
            for (Element *p = begin; p != end; p++)
                *bucket[(*p >> shift) & 0xFF]++ = *p;
            std::swap(begin, begin1);
            std::swap(end, end1);
        }
    }

    void LSB(Element *begin, ElementCounter size, Element *buffer, Element maxshift);
    void MSDStep(Element* arr, ElementCounter offset, ElementCounter end, uint8_t shift, Element* buffer) {
        assert(offset >= 0);
        ElementCounter x;
        ElementCounter last[MAX_DIGIT_COUNT] = { 0 };
        ElementCounter pointer[MAX_DIGIT_COUNT];
        // count digits
        for (x = offset; x < end; ++x) {
            ++last[(arr[x] >> shift) & LOWER_DIGIT_MASK];
        }
        // count digit positions
        last[0] += offset;
        pointer[0] = offset;
        for (x = 1; x < MAX_DIGIT_COUNT; ++x) {
            pointer[x] = last[x - 1];
            last[x] += last[x - 1];
        }
        // swapping elements
        Element value;
        ElementCounter y;
        Element temp;
        for (x = 0; x < MAX_DIGIT_COUNT; ++x) {
            while (pointer[x] != last[x]) {
                value = arr[pointer[x]];
                y = (value >> shift) & LOWER_DIGIT_MASK;
                while (x != y) {
                    temp = arr[pointer[y]];
                    arr[pointer[y]++] = value;
                    value = temp;
                    y = (value >> shift) & LOWER_DIGIT_MASK;
                }
                arr[pointer[x]++] = value;
            }
        }

        if (shift > 0) {
            shift -= 8;
            for (x = 0; x < MAX_DIGIT_COUNT; ++x) {
                ElementCounter pX = pointer[x];
                temp = x > 0 ? pX - pointer[x - 1] : pX - offset;
                if (temp > 1) {
                    //insertion_sort(arr, pointer[x] - temp, pointer[x]);
                    // LSDStep(arr + pX - temp, temp, shift / 8 + 1, buffer);
                    // _radix_sort_lsb(arr + (pX - temp), arr + temp, buffer + (pX - temp), shift);
                    LSB(arr + pX - temp, temp, buffer + (pX - temp), shift);
                    // _radix_sort_lsb(obucket[i], bucket[i], begin + (obucket[i] - begin1), shift - 8);
                    // LSDStep(obucket[i], bucket[i] - obucket[i], shift / 8 + 1, begin + (obucket[i] - begin1));
                }
            }
            memcpy(arr, buffer, (end - offset) * 4);
        }

    }

    void MSDSort(made::Element *arr, made::ElementCounter size) {
        Element *buffer = new Element[size];
        made::MSDStep(arr, 0, size, 24, buffer);
        delete buffer;
    }

    void _radix_sort_msb(Element *begin, Element *end, Element *begin1, Element shift)
    {
        //unsigned *end1 = begin1 + (end - begin);

        size_t count[0x100] = {};
        for (Element *p = begin; p != end; p++)
            count[(*p >> shift) & 0xFF]++;
        Element *bucket[0x100], *obucket[0x100], *q = begin1;
        for (int i = 0; i < 0x100; q += count[i++])
            obucket[i] = bucket[i] = q;
        for (Element *p = begin; p != end; p++)
            *bucket[(*p >> shift) & 0xFF]++ = *p;
        for (int i = 0; i < 0x100; ++i)
            _radix_sort_lsb(obucket[i], bucket[i], begin + (obucket[i] - begin1), shift - 8);
        // for (int i = 0; i < 0x100; ++i)
        //    LSDStep(obucket[i], bucket[i] - obucket[i], shift / 8 + 1, begin + (obucket[i] - begin1));

    }

    void radix_sort(Element *begin, ElementCounter size)
    {
        Element *end = begin + size;
        unsigned *buffer = new Element[end - begin];
        _radix_sort_msb(begin, end, buffer, 24);
        //_radix_sort_lsb(begin, end, begin1, 32);
        delete[] buffer;
    }
}

#ifndef MADE_CUSTOM_SORT_H_
#define MADE_CUSTOM_SORT_H_
void Sort(made::Element *arr, made::ElementCounter size) {
    made::MSDSort(arr, size);
    //made::LSDSort(arr, size);
    //made::LSDSort((made::Element*)arr, (made::ElementCounter)size);
    //made::LSDThreeSort((made::Element*)arr, (made::ElementCounter)size);
}
void Sort(int *arr, int size) {
    Sort((made::Element*)arr, (made::ElementCounter)size);
}
#endif // !MADE_CUSTOM_SORT_H_

#endif // !MADE_SORT_H_
