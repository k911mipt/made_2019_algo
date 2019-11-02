#ifndef K9_SORT_H_
#define K9_SORT_H_

#include "common.h"

namespace made {
    void k9_lsd(Element* arr, const ElementCounter size, const uint8_t num_digits, Element *buffer)
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

    void k9_msb(Element* arr, ElementCounter size, Element *buffer)
    {
        Element* end = arr + size;
        const uint16_t MAX_BUCKETS = 1 << 8; // 256;
        const uint8_t shift = 24;
        size_t count[MAX_BUCKETS] = {};
        for (Element* p = arr; p != end; p++) {
            //count[(*p >> shift) & 0xFF]++;
            uint8_t index = (*p >> shift) & 0xFF;
            count[index]++;
        }
        Element b_begin[MAX_BUCKETS];
        Element b_end[MAX_BUCKETS];
        Element q = 0; // счётчик буффера
        for (int i = 0; i < MAX_BUCKETS; q += count[i++]) {
            b_begin[i] = b_end[i] = q;
        }
        for (Element* p = arr; p != end; p++) {
            //*bucket[(*p >> shift) & 0xFF]++ = *p;
            buffer[(b_end[(*p >> shift) & 0xFF])++] = *p;
            //uint8_t index = (*p >> shift) & 0xFF; // можно использовать указатель на uint8_t чтобы не делать shift
            //buffer[b_end[index]] = *p;
            //b_end[index]++;
        }

        //for (int i = 0; i < MAX_BUCKETS; ++i)
        //    _radix_sort_lsb(buffer + b_begin[i], buffer + b_end[i], arr + b_begin[i], shift - 8);
        for (int i = 0; i < 0x100; ++i) {
            k9_lsd(buffer + b_begin[i], b_end[i] - b_begin[i], shift / 8 + 1, arr + b_begin[i]);
        }
        //memcpy(arr, buffer, (arr_end - arr) * 4);
    }

#define MAX_THREE_DIGIT_NUMBER (((Element)(~0) >> 1) >> 7);
    static Element const max_three_digit = MAX_THREE_DIGIT_NUMBER;
    static const ElementCounter L3_CACHE = 20 * 1024 * 1024; // 20 MB
    void k9_raw_sort(Element* arr, ElementCounter size, Element *buffer) {
        Element max = *arr;
        Element min = max;
        // Preliminary value to retrieve some initial info from the array
        const Element prel = (size > 128) ? 64 : size / 8;
        Element* i_arr;
        Element* n_arr;
        // Get max value(to know how many bytes to sort)
        {
            // check first few values
            for (i_arr = arr + 1, n_arr = arr + prel; i_arr < n_arr; ++i_arr) {
                max = *i_arr > max ? *i_arr : max;
                min = *i_arr < min ? *i_arr : min;
                //if (*i_arr > max || *i_arr < min) {
                //    if (*i_arr > max) {
                //        max = *i_arr;
                //    }
                //    else {
                //        min = *i_arr;
                //    }
                //}
            }
            // check last few values
            for (i_arr = arr + size - prel, n_arr = arr + size; i_arr < n_arr; ++i_arr) {
                max = *i_arr > max ? *i_arr : max;
                min = *i_arr < min ? *i_arr : min;
            }
            // if we're not sure yet that we need all 4 digits, let's check the whole array
            if (max <= max_three_digit || max == min) {
                for (i_arr = arr + prel, n_arr = arr + size - prel; i_arr < n_arr; ++i_arr) {
                    max = *i_arr > max ? *i_arr : max;
                    min = *i_arr < min ? *i_arr : min;
                }
            }
        }
        
        Element diff = max - min;
        Element exp;
        uint8_t bytes_to_sort;
        if (diff != 0) { // TODO check if least byte doesnt change? or it will be checked later...
            bytes_to_sort = 1;
            exp = 8;
            while (exp < (sizeof(Element) << 3) && (max >> (exp - 1)) > 0)
            {
                bytes_to_sort++;
                exp += 8;
            }
        }
        else {
            // Nothing to sort :-)
            return;
        }
        // Core algorithm template:                                              */
        // 1 - Fill the buckets (using byte iterators)                           */
        // 2 - Check if there is 1 bucket w/ all elements (no need to sort byte) */
        // 3 - Set the pointers to the helper array if setting the byte          */
        // 4 - Iterate the bucket values within the orginal array and chnage the */
        //     corresponding values in the helper                                */
        int* point[0x100] = { 0 }; // Array of pointers to the helper array
        
        const ElementCounter CACHE_FRIENDLY_PROCESS_SIZE = L3_CACHE / sizeof(Element) / 2; // Divide by 2 since we have a bufer array too
        if (bytes_to_sort > 1 && size > 1600000)
        { /* MSB order (size > 1.6M) */

        }
    }

    void k9_sort(Element *begin, ElementCounter size)
    {
        //Element *end = begin + size;
        //Element *buffer = new Element[end - begin];
        Element *buffer = (Element *)malloc(sizeof(Element) * size);
        //k9_msb(begin, size, buffer);
        k9_raw_sort(begin, size, buffer);
        free(buffer);
        //delete[] buffer;
    }
}
#endif // !K9_SORT_H_