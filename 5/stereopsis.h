#ifndef STEREOPSIS_H_
#define STEREOPSIS_H_

#include <cstdint>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef long int32;
typedef unsigned long uint32;
typedef float real32;
typedef double real64;
typedef unsigned char uint8;
typedef const char *cpointer;
#define CORRECTNESS	1
#define PREFETCH 1
#if PREFETCH
#include <xmmintrin.h>	// for prefetch
#define pfval	64
#define pfval2	128
#define pf(x)	_mm_prefetch(cpointer(x + i + pfval), 0)
#define pf2(x)	_mm_prefetch(cpointer(x + i + pfval2), 0)
#else
#define pf(x)
#define pf2(x)
#endif

namespace stereopsis {
    typedef uint8_t Digit;
    //typedef uint16_t Digit;
    typedef uint32_t Element; // 0..10^9 value range
    typedef uint32_t ElementCounter; // 0..10^7 array size
    const uint8_t DIGIT_SIZE = sizeof(Digit);
    const uint32_t MAX_DIGIT_COUNT = 1 << (DIGIT_SIZE * 8);

    void StereopsisSort(Element* arr, const uint32_t size)
    {
        Element *buffer = new Element[size];
        const uint8_t step = sizeof(Element) / DIGIT_SIZE;
        const uint8_t num_digits = step;
        ElementCounter digit_counters[num_digits][MAX_DIGIT_COUNT] = { 0 };// std::memset(digit_counters, 0, sizeof(digit_counters));
        ElementCounter(&digit_start_indexes)[num_digits][MAX_DIGIT_COUNT] = digit_counters; // Reference for further readability

        /* Number is stored in reverse order
         * example: uint32_t number 439041101 located at 0x00000000
         * 439041101 = 0x1A2B3C4D
         * uint32_t 0x00000000 : 0x1A2B3C4D
         * uint8_t  0x00000000 : 0x4D
         * uint8_t  0x00000001 : 0x3C
         * uint8_t  0x00000002 : 0x2B
         * uint8_t  0x00000003 : 0x1A
         */
         //const Digit *curBytes[num_digits];
         //for (int j = 0; j < num_digits; ++j) {
         //    curBytes[j] = reinterpret_cast<const Digit*>(arr) + j;
         //}
         // loop over all of the bytes in the sorce, incrementing the index
        const Digit* digit_ptr = reinterpret_cast<const Digit*>(arr);
        for (unsigned i_element = 0; i_element < size; ++i_element) {
            //for (int j = 0; j < num_digits; ++j) {
            //    ++DigitCounters[j][*curBytes[j]];
            //    curBytes[j] += step;
            //}
            ++digit_counters[0][*digit_ptr++];
            ++digit_counters[1][*digit_ptr++];
            ++digit_counters[2][*digit_ptr++];
            ++digit_counters[3][*digit_ptr++];
        }

        // Count start indexes for each digit. Store in digit_start_indexes
        unsigned start_indexes[num_digits] = { 0 };
        //std::memset(start_indexes, 0, sizeof(start_indexes));
        ElementCounter temp_counts[num_digits];
        //ElementCounter temp;
        for (unsigned i_e = 0; i_e < MAX_DIGIT_COUNT; ++i_e) { // i_element
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

            temp_counts[3] = digit_counters[3][i_e];
            digit_start_indexes[3][i_e] = start_indexes[3];
            start_indexes[3] += temp_counts[3];
        }

        // now that you have indexes, copy the values over
        // Sorting and placing results into buffer
        Element* arr_iterator = arr;
        Digit* digit_iterator;
        for (uint8_t i_digit = 0; i_digit < num_digits; ++i_digit) {
            digit_iterator = reinterpret_cast<Digit*>(arr_iterator) + i_digit;
            for (unsigned i = 0; i < size; ++i, ++arr_iterator, digit_iterator += step)
            {
                uint32_t *countPtr = digit_start_indexes[i_digit] + *digit_iterator;
                buffer[(*countPtr)++] = *arr_iterator;
            }
            arr_iterator = buffer; buffer = arr; arr = arr_iterator; // swapping pointers for next sorting iteration
        }
        delete[] buffer;
    }

#define _0(x)	(x & 0x7FF)
#define _1(x)	(x >> 11 & 0x7FF)
#define _2(x)	(x >> 22 )

    static void RadixSort11(Element *farray, const uint32_t elements)
    {
        Element *sorted = new Element[elements];
        uint32_t i;
        uint32_t *sort = (uint32_t*)sorted;
        uint32_t *array = (uint32_t*)farray;

        // 3 histograms on the stack:
        const uint32_t kHist = 2048;
        uint32_t b0[kHist * 3];

        uint32_t *b1 = b0 + kHist;
        uint32_t *b2 = b1 + kHist;

        for (i = 0; i < kHist * 3; i++) {
            b0[i] = 0;
        }
        //memset(b0, 0, kHist * 12);

        // 1.  parallel histogramming pass
        //
        for (i = 0; i < elements; i++) {

            pf(array);

            //uint32 fi = FloatFlip((uint32&)array[i]);
            uint32 fi = array[i];

            b0[_0(fi)] ++;
            b1[_1(fi)] ++;
            b2[_2(fi)] ++;
        }

        // 2.  Sum the histograms -- each histogram entry records the number of values preceding itself.
        {
            uint32 sum0 = 0, sum1 = 0, sum2 = 0;
            uint32 tsum;
            for (i = 0; i < kHist; i++) {

                tsum = b0[i] + sum0;
                b0[i] = sum0 - 1;
                sum0 = tsum;

                tsum = b1[i] + sum1;
                b1[i] = sum1 - 1;
                sum1 = tsum;

                tsum = b2[i] + sum2;
                b2[i] = sum2 - 1;
                sum2 = tsum;
            }
        }

        // byte 0: floatflip entire value, read/write histogram, write out flipped
        for (i = 0; i < elements; i++) {

            uint32 fi = array[i];
            //FloatFlipX(fi);
            uint32 pos = _0(fi);

            pf2(array);
            sort[++b0[pos]] = fi;
        }

        // byte 1: read/write histogram, copy
        //   sorted -> array
        for (i = 0; i < elements; i++) {
            uint32 si = sort[i];
            uint32 pos = _1(si);
            pf2(sort);
            array[++b1[pos]] = si;
        }

        // byte 2: read/write histogram, copy & flip out
        //   array -> sorted
        for (i = 0; i < elements; i++) {
            uint32 ai = array[i];
            uint32 pos = _2(ai);

            pf2(array);
            //sort[++b2[pos]] = IFloatFlip(ai);
            sort[++b2[pos]] = ai;
        }

        // to write original:
        // memcpy(array, sorted, elements * 4);
        memcpy(array, sorted, elements * 4);
        delete[] sorted;
    }
}

#endif // !STEREOPSIS_H_
