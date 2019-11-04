#ifndef K9_SORT_H_
#define K9_SORT_H_

#include "common.h"
#include <cassert>

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

#define LAST_EXP__ (sizeof(int) << 3)

    void sort_byte(Element* arr, Element* &helper, uint8_t &shift, ElementCounter size, uint8_t exp, bucket, pointer, ptr_init, optional_ptr_init) {
        ElementCounter bucket[MAX_DIGIT_COUNT] = { 0 };
        //Digit* digit_ptr = (Digit*)(arr)+(exp >> 3);
        // Filling buckets of highest unsorted digit
        {// Loop unrolling
            Digit *digit_ptr, *digit_end;
            // Iterating without last < 8 elements
            // TODO make more readable condition. (~0 << 3) = 1111...11111000 = UINT32_MAX << 3
            for (digit_ptr = (Digit*)(arr)+(exp >> 3),
                digit_end = (Digit*)(arr + (size & (~0 << 3)));
                digit_ptr < digit_end;) {
                ++bucket[*digit_ptr]; digit_ptr += ELEMENT_SIZE;
                ++bucket[*digit_ptr]; digit_ptr += ELEMENT_SIZE;
                ++bucket[*digit_ptr]; digit_ptr += ELEMENT_SIZE;
                ++bucket[*digit_ptr]; digit_ptr += ELEMENT_SIZE;
                ++bucket[*digit_ptr]; digit_ptr += ELEMENT_SIZE;
                ++bucket[*digit_ptr]; digit_ptr += ELEMENT_SIZE;
                ++bucket[*digit_ptr]; digit_ptr += ELEMENT_SIZE;
                ++bucket[*digit_ptr]; digit_ptr += ELEMENT_SIZE;
            }
            for (digit_ptr = (Digit*)(arr + (size & (~0 << 3))) + (exp >> 3), digit_end = (Digit*)(arr + size); digit_ptr < digit_end;) {
                ++bucket[*digit_ptr];
                digit_ptr += sizeof(int);
            }
        }
        Element* arr_ptr = helper;
        bool next = false;
        if (size > 65535) {
            for (ElementCounter i = 0; i < 0x100 && !next; ++i) {
                if (bucket[i] == size) {
                    optional_ptr_init;
                    next = 1;
                }
            }
        }
        // Sadly we got more than one bucket. So lets sort it
        if (!next) {
            // Get start pointers for each digit
            for (ElementCounter i = 0; i < MAX_DIGIT_COUNT; arr_ptr += bucket[i++]) {
                ptr_init;
            }
            // Copy values from source to buffer in corresponding order
            for (arr_ptr = arr, arr_end = arr + (size & (~0 << 3)); arr_ptr < arr_end;) {
                *point[(*arr_ptr >> exp) & 0xFF]++ = *arr_ptr; ++arr_ptr;
                *point[(*arr_ptr >> exp) & 0xFF]++ = *arr_ptr; ++arr_ptr;
                *point[(*arr_ptr >> exp) & 0xFF]++ = *arr_ptr; ++arr_ptr;
                *point[(*arr_ptr >> exp) & 0xFF]++ = *arr_ptr; ++arr_ptr;
                *point[(*arr_ptr >> exp) & 0xFF]++ = *arr_ptr; ++arr_ptr;
                *point[(*arr_ptr >> exp) & 0xFF]++ = *arr_ptr; ++arr_ptr;
                *point[(*arr_ptr >> exp) & 0xFF]++ = *arr_ptr; ++arr_ptr;
                *point[(*arr_ptr >> exp) & 0xFF]++ = *arr_ptr; ++arr_ptr;
            }
            for (arr_ptr = arr + (size & (~0 << 3)), arr_end = arr + size; arr_ptr < arr_end;) {
                *point[(*arr_ptr >> exp) & 0xFF]++ = *arr_ptr; ++arr_ptr;
            }
            for (s = vector, k = &vector[size & (~0 << 3)]; s < k;) {
                *pointer[(*s shift) & 0xFF]++ = *s;	++s;
                *pointer[(*s shift) & 0xFF]++ = *s;	++s;
                *pointer[(*s shift) & 0xFF]++ = *s;	++s;
                *pointer[(*s shift) & 0xFF]++ = *s;	++s;
                *pointer[(*s shift) & 0xFF]++ = *s;	++s;
                *pointer[(*s shift) & 0xFF]++ = *s;	++s;
                *pointer[(*s shift) & 0xFF]++ = *s;	++s;
                *pointer[(*s shift) & 0xFF]++ = *s;	++s;
            }
            for (s = &vector[size & (~0 << 3)], k = &vector[size]; s < k;) {
                *pointer[(*s shift) & 0xFF]++ = *s; ++s;
            }
            swap = 1 - swap;
        }
        exp += 8;
    }

#define MAX_THREE_DIGIT_NUMBER (((Element)(~0) >> 1) >> 7);
    static Element const max_three_digit = MAX_THREE_DIGIT_NUMBER;
    static const ElementCounter L3_CACHE = 20 * 1024 * 1024; // 20 MB
    void k9_raw_sort(Element* arr, ElementCounter size, Element *buffer) {
        // TODO replace casting with reinterpret_cast everywhere!
        Element max = *arr;
        Element min = max;
        // Preliminary value to retrieve some initial info from the array
        const Element prel = (size > 128) ? 64 : size / 8;
        Element* arr_ptr;
        Element* arr_end;
        ElementCounter i;
        Element exp; // bits sorted;
        Digit* digit_ptr;
        Digit* digit_end;
        //bool next;
        // Get max value(to know how many bytes to sort)
        {
            // check first few values
            for (arr_ptr = arr + 1, arr_end = arr + prel; arr_ptr < arr_end; ++arr_ptr) {
                max = *arr_ptr > max ? *arr_ptr : max;
                min = *arr_ptr < min ? *arr_ptr : min;
                //if (*arr_ptr > max || *arr_ptr < min) {
                //    if (*arr_ptr > max) {
                //        max = *arr_ptr;
                //    }
                //    else {
                //        min = *arr_ptr;
                //    }
                //}
            }
            // check last few values
            for (arr_ptr = arr + size - prel, arr_end = arr + size; arr_ptr < arr_end; ++arr_ptr) {
                max = *arr_ptr > max ? *arr_ptr : max;
                min = *arr_ptr < min ? *arr_ptr : min;
            }
            // if we're not sure yet that we need all 4 digits, let's check the whole array
            if (max <= max_three_digit || max == min) {
                for (arr_ptr = arr + prel, arr_end = arr + size - prel; arr_ptr < arr_end; ++arr_ptr) {
                    max = *arr_ptr > max ? *arr_ptr : max;
                    min = *arr_ptr < min ? *arr_ptr : min;
                }
            }
        }

        uint8_t bytes_to_sort;
        // Count number of bytes to sort
        {
            if (max != min) { // TODO check if least byte doesnt change? or it will be checked later...
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
        }

        // TODO : try to get rid of swap variable
        bool swap = false; // Tells where sorted array is (if true, sorted is in vector)
        // TODO : try to get rid of last_byte_sorted variable
        bool last_byte_sorted = false; // true if we had to sort the last byte
        // Core algorithm template:                                              */
        // 1 - Fill the buckets (using byte iterators)                           */
        // 2 - Check if there is 1 bucket w/ all elements (no need to sort byte) */
        // 3 - Set the pointers to the helper array if setting the byte          */
        // 4 - Iterate the bucket values within the orginal array and chnage the */
        //     corresponding values in the helper                                */
        Element* point[MAX_DIGIT_COUNT] = { 0 }; // Array of pointers to the helper array

        const ElementCounter CACHE_FRIENDLY_PROCESS_SIZE = L3_CACHE / sizeof(Element) / 2; // Divide by 2 since we have a bufer array too
        // Start sorting with MSB (size > 2621440 elements)
        if (bytes_to_sort > 1 && size > CACHE_FRIENDLY_PROCESS_SIZE) {
            exp -= 8;
            // old_point will serve as a copy of the initial values of "point"
            // Beggining of each subarray in 1st subdivision (256 subarrays)
            Element *old_point[MAX_DIGIT_COUNT] = { 0 };

            /* Sort last byte */
            ElementCounter bucket[MAX_DIGIT_COUNT] = { 0 };
            digit_ptr = (Digit*)(arr)+(exp >> 3); // TODO remove from here?
            // Filling buckets of highest unsorted digit
            {// Loop unrolling
                digit_ptr = (Digit*)(arr)+(exp >> 3);
                // Iterating without last < 8 elements
                // TODO make more readable condition. (~0 << 3) = 1111...11111000 = UINT32_MAX << 3
                for (digit_end = (Digit*)(arr + (size & (~0 << 3))); digit_ptr < digit_end;) {
                    ++bucket[*digit_ptr]; digit_ptr += ELEMENT_SIZE;
                    ++bucket[*digit_ptr]; digit_ptr += ELEMENT_SIZE;
                    ++bucket[*digit_ptr]; digit_ptr += ELEMENT_SIZE;
                    ++bucket[*digit_ptr]; digit_ptr += ELEMENT_SIZE;
                    ++bucket[*digit_ptr]; digit_ptr += ELEMENT_SIZE;
                    ++bucket[*digit_ptr]; digit_ptr += ELEMENT_SIZE;
                    ++bucket[*digit_ptr]; digit_ptr += ELEMENT_SIZE;
                    ++bucket[*digit_ptr]; digit_ptr += ELEMENT_SIZE;
                }
                // Iterating last < 8 elements
                for (digit_ptr = (Digit*)(arr + (size & (~0 << 3))) + (exp >> 3), digit_end = (uint8_t*)(arr + size); digit_ptr < digit_end;) {
                    ++bucket[*digit_ptr]; digit_ptr += ELEMENT_SIZE;
                }
            }
            // Check if only one bucket is filled up. Then we don't need to sort it
            arr_ptr = buffer; // TODO remove from here?
            bool next = false;
            if (size > (1 << 16)) {
                for (i = 0; i < MAX_DIGIT_COUNT && !next; ++i)
                {
                    if (bucket[i] == size) // All digits in
                    {
                        old_point[i] = arr_ptr; // TODO replace to old_point[i] = buffer; ?!
                        point[i] = old_point[i] + size;
                        next = true;
                    }
                }
            }
            // Sadly we got more than one bucket. So lets sort it
            if (!next) {
                // Get start pointers for each digit
                {
                    arr_ptr = buffer;
                    if (exp != ((sizeof(int) << 3) - 8)) {// number of bits != 24
                        // Making array of pointers to start of each digit sequences
                        for (i = 0; i < MAX_DIGIT_COUNT; arr_ptr += bucket[i++])
                            old_point[i] = point[i] = arr_ptr;
                    }
                    else {
                        // TODO optimize! now we assume negative numbers too.
                        // we certainly know, that highest byte is no more than 0x3F = 63,
                        // so we only need to iterate 64 times
                        for (i = 128; i < 0x100; arr_ptr += bucket[i++])
                            old_point[i] = point[i] = arr_ptr;
                        for (i = 0; i < 128; arr_ptr += bucket[i++])
                            old_point[i] = point[i] = arr_ptr;
                    }
                }
                // Copy values from source to buffer in corresponding order
                {// Loop unrolling
                    for (arr_ptr = arr, arr_end = arr + (size & (~0 << 3)); arr_ptr < arr_end;) {
                        *point[(*arr_ptr >> exp) & 0xFF]++ = *arr_ptr; ++arr_ptr;
                        *point[(*arr_ptr >> exp) & 0xFF]++ = *arr_ptr; ++arr_ptr;
                        *point[(*arr_ptr >> exp) & 0xFF]++ = *arr_ptr; ++arr_ptr;
                        *point[(*arr_ptr >> exp) & 0xFF]++ = *arr_ptr; ++arr_ptr;
                        *point[(*arr_ptr >> exp) & 0xFF]++ = *arr_ptr; ++arr_ptr;
                        *point[(*arr_ptr >> exp) & 0xFF]++ = *arr_ptr; ++arr_ptr;
                        *point[(*arr_ptr >> exp) & 0xFF]++ = *arr_ptr; ++arr_ptr;
                        *point[(*arr_ptr >> exp) & 0xFF]++ = *arr_ptr; ++arr_ptr;
                    }
                    for (arr_ptr = arr + (size & (~0 << 3)), arr_end = arr + size; arr_ptr < arr_end;) {
                        *point[(*arr_ptr >> exp) & 0xFF]++ = *arr_ptr; ++arr_ptr;
                    }
                    swap = !swap;
                }
            }
            exp += 8;
            --bytes_to_sort;
            if (exp == (sizeof(Element) << 3)) {// 32
                last_byte_sorted = true;
            }

            // 2nd subdivision only for 3 bytes or more (and size > 512M)
            ElementCounter j;
            if (bytes_to_sort > 1 && size > 512000000) {
                assert(false); // TODO implement
            }
            // LSB sort rest bytes
            else {
                max = 1 << ((bytes_to_sort - 1) << 3);
                int swap_copy = swap; // Once more, reset swap in ea subarray
                for (j = 0; j < MAX_DIGIT_COUNT; ++j) {
                    exp = 0;
                    size = point[j] - old_point[j];
                    swap = swap_copy;

                    Element* sub_help;
                    Element* sub_vec; /* Temporary arrays */
                    //if (swap) {
                    //    sub_help = arr + (old_point[j] - buffer);
                    //    sub_vec = old_point[j];
                    //}
                    //else {
                    //    sub_help = old_point[j];
                    //    sub_vec = arr + (old_point[j] - buffer);
                    //}
                    // TODO check asm for what is better
                    Element* old_point_j = old_point[j];
                    const ElementCounter sub_size = old_point_j - buffer;
                    sub_help = swap ? arr + sub_size : old_point_j;
                    sub_vec = swap ? old_point_j : arr + sub_size;

                    Element* temp_point[MAX_DIGIT_COUNT]; // Temp pointers for segment
                    while (exp < (sizeof(int) << 3) && (max >> exp) > 0)
                    { /* While there are remaining bytes */
                        if (exp)
                        {
                            if (swap != swap_copy)
                            {
                                ElementCounter bucket1[MAX_DIGIT_COUNT] = { 0 };
                                digit_ptr = (Digit*)(sub_help)+(exp >> 3);
                                for (digit_end = (Digit*)(sub_help + (size & (~0 << 3))); digit_ptr < digit_end;) {
                                    ++bucket1[*digit_ptr]; digit_ptr += ELEMENT_SIZE;
                                    ++bucket1[*digit_ptr]; digit_ptr += ELEMENT_SIZE;
                                    ++bucket1[*digit_ptr]; digit_ptr += ELEMENT_SIZE;
                                    ++bucket1[*digit_ptr]; digit_ptr += ELEMENT_SIZE;
                                    ++bucket1[*digit_ptr]; digit_ptr += ELEMENT_SIZE;
                                    ++bucket1[*digit_ptr]; digit_ptr += ELEMENT_SIZE;
                                    ++bucket1[*digit_ptr]; digit_ptr += ELEMENT_SIZE;
                                    ++bucket1[*digit_ptr]; digit_ptr += ELEMENT_SIZE;
                                }
                                for (digit_ptr = (Digit*)(sub_help + (size & (~0 << 3))) + (exp >> 3), digit_end = (uint8_t*)(sub_help + size); digit_ptr < digit_end;) {
                                    ++bucket1[*digit_ptr]; digit_ptr += ELEMENT_SIZE;
                                }
                                arr_ptr = sub_vec;
                                next = 0;
                                if (size > 65535)
                                {
                                    for (i = 0; i < 0x100 && !next; ++i)
                                    {
                                        if (bucket1[i] == size)
                                        {
                                            ;
                                            next = 1;
                                        }
                                    }
                                }
                                if (!next)
                                {
                                    if (exp != ((sizeof(int) << 3) - 8))
                                    {
                                        for (i = 0; i < 0x100; s += bucket1[i++])
                                        {
                                            temp_point[i] = s;
                                        }
                                    }
                                    else
                                    {
                                        for (i = 128; i < 0x100; s += bucket1[i++])
                                        {
                                            temp_point[i] = s;
                                        }
                                        for (i = 0; i < 128; s += bucket1[i++])
                                        {
                                            temp_point[i] = s;
                                        }
                                    }
                                    for (s = sub_help, k = &sub_help[size & (~0 << 3)]; s < k;)
                                    {
                                        *temp_point[(*s >> exp) & 0xFF]++ = *s;
                                        ++s;
                                        *temp_point[(*s >> exp) & 0xFF]++ = *s;
                                        ++s;
                                        *temp_point[(*s >> exp) & 0xFF]++ = *s;
                                        ++s;
                                        *temp_point[(*s >> exp) & 0xFF]++ = *s;
                                        ++s;
                                        *temp_point[(*s >> exp) & 0xFF]++ = *s;
                                        ++s;
                                        *temp_point[(*s >> exp) & 0xFF]++ = *s;
                                        ++s;
                                        *temp_point[(*s >> exp) & 0xFF]++ = *s;
                                        ++s;
                                        *temp_point[(*s >> exp) & 0xFF]++ = *s;
                                        ++s;
                                    }
                                    for (s = &sub_help[size & (~0 << 3)], k = &sub_help[size]; s < k;)
                                    {
                                        *temp_point[(*s >> exp) & 0xFF]++ = *s;
                                        ++s;
                                    }
                                    swap = 1 - swap;
                                }
                                exp += 8;
                                ;
                            }
                            else
                            {
                                int bucket1[0x100] = { 0 };
                                n = (unsigned char *)(sub_vec)+(exp >> 3);
                                for (m = (unsigned char *)(&sub_vec[size & (~0 << 3)]); n < m;)
                                {
                                    ++bucket1[*n];
                                    n += sizeof(int);
                                    ++bucket1[*n];
                                    n += sizeof(int);
                                    ++bucket1[*n];
                                    n += sizeof(int);
                                    ++bucket1[*n];
                                    n += sizeof(int);
                                    ++bucket1[*n];
                                    n += sizeof(int);
                                    ++bucket1[*n];
                                    n += sizeof(int);
                                    ++bucket1[*n];
                                    n += sizeof(int);
                                    ++bucket1[*n];
                                    n += sizeof(int);
                                }
                                for (n = (unsigned char *)(&sub_vec[size & (~0 << 3)]) + (exp >> 3), m = (unsigned char *)(&sub_vec[size]); n < m;)
                                {
                                    ++bucket1[*n];
                                    n += sizeof(int);
                                }
                                s = sub_help;
                                next = 0;
                                if (size > 65535)
                                {
                                    for (i = 0; i < 0x100 && !next; ++i)
                                    {
                                        if (bucket1[i] == size)
                                        {
                                            ;
                                            next = 1;
                                        }
                                    }
                                }
                                if (!next)
                                {
                                    if (exp != ((sizeof(int) << 3) - 8))
                                    {
                                        for (i = 0; i < 0x100; s += bucket1[i++])
                                        {
                                            temp_point[i] = s;
                                        }
                                    }
                                    else
                                    {
                                        for (i = 128; i < 0x100; s += bucket1[i++])
                                        {
                                            temp_point[i] = s;
                                        }
                                        for (i = 0; i < 128; s += bucket1[i++])
                                        {
                                            temp_point[i] = s;
                                        }
                                    }
                                    for (s = sub_vec, k = &sub_vec[size & (~0 << 3)]; s < k;)
                                    {
                                        *temp_point[(*s >> exp) & 0xFF]++ = *s;
                                        ++s;
                                        *temp_point[(*s >> exp) & 0xFF]++ = *s;
                                        ++s;
                                        *temp_point[(*s >> exp) & 0xFF]++ = *s;
                                        ++s;
                                        *temp_point[(*s >> exp) & 0xFF]++ = *s;
                                        ++s;
                                        *temp_point[(*s >> exp) & 0xFF]++ = *s;
                                        ++s;
                                        *temp_point[(*s >> exp) & 0xFF]++ = *s;
                                        ++s;
                                        *temp_point[(*s >> exp) & 0xFF]++ = *s;
                                        ++s;
                                        *temp_point[(*s >> exp) & 0xFF]++ = *s;
                                        ++s;
                                    }
                                    for (s = &sub_vec[size & (~0 << 3)], k = &sub_vec[size]; s < k;)
                                    {
                                        *temp_point[(*s >> exp) & 0xFF]++ = *s;
                                        ++s;
                                    }
                                    swap = 1 - swap;
                                }
                                exp += 8;
                                ;
                            }
                        }
                        else
                        {
                            int buckett[0x100] = { 0 };
                            n = (unsigned char *)(sub_vec)+(exp >> 3);
                            for (m = (unsigned char *)(&sub_vec[size & (~0 << 3)]); n < m;)
                            {
                                ++buckett[*n];
                                n += sizeof(int);
                                ++buckett[*n];
                                n += sizeof(int);
                                ++buckett[*n];
                                n += sizeof(int);
                                ++buckett[*n];
                                n += sizeof(int);
                                ++buckett[*n];
                                n += sizeof(int);
                                ++buckett[*n];
                                n += sizeof(int);
                                ++buckett[*n];
                                n += sizeof(int);
                                ++buckett[*n];
                                n += sizeof(int);
                            }
                            for (n = (unsigned char *)(&sub_vec[size & (~0 << 3)]) + (exp >> 3), m = (unsigned char *)(&sub_vec[size]); n < m;)
                            {
                                ++buckett[*n];
                                n += sizeof(int);
                            }
                            s = sub_help;
                            next = 0;
                            if (size > 65535)
                            {
                                for (i = 0; i < 0x100 && !next; ++i)
                                {
                                    if (buckett[i] == size)
                                    {
                                        ;
                                        next = 1;
                                    }
                                }
                            }
                            if (!next)
                            {
                                if (exp != ((sizeof(int) << 3) - 8))
                                {
                                    for (i = 0; i < 0x100; s += buckett[i++])
                                    {
                                        temp_point[i] = s;
                                    }
                                }
                                else
                                {
                                    for (i = 128; i < 0x100; s += buckett[i++])
                                    {
                                        temp_point[i] = s;
                                    }
                                    for (i = 0; i < 128; s += buckett[i++])
                                    {
                                        temp_point[i] = s;
                                    }
                                }
                                for (s = sub_vec, k = &sub_vec[size & (~0 << 3)]; s < k;)
                                {
                                    *temp_point[(*s) & 0xFF]++ = *s;
                                    ++s;
                                    *temp_point[(*s) & 0xFF]++ = *s;
                                    ++s;
                                    *temp_point[(*s) & 0xFF]++ = *s;
                                    ++s;
                                    *temp_point[(*s) & 0xFF]++ = *s;
                                    ++s;
                                    *temp_point[(*s) & 0xFF]++ = *s;
                                    ++s;
                                    *temp_point[(*s) & 0xFF]++ = *s;
                                    ++s;
                                    *temp_point[(*s) & 0xFF]++ = *s;
                                    ++s;
                                    *temp_point[(*s) & 0xFF]++ = *s;
                                    ++s;
                                }
                                for (s = &sub_vec[size & (~0 << 3)], k = &sub_vec[size]; s < k;)
                                {
                                    *temp_point[(*s) & 0xFF]++ = *s;
                                    ++s;
                                }
                                swap = 1 - swap;
                            }
                            exp += 8;
                            ;
                        }
                    }
                }

            }

        }
        // LSB order
        else {
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