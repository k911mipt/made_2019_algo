#ifndef K9_AVX_H_
#define K9_AVX_H_

//#include "common.h"
#include "sort.h"
#include "stdint.h"
#include <algorithm>
#include <cstring>
#include <cassert>
//#include "timer.h"

const int tab32[32] = {
     0,  9,  1, 10, 13, 21,  2, 29,
    11, 14, 16, 18, 22, 25,  3, 30,
     8, 12, 20, 28, 15, 17, 24,  7,
    19, 27, 23,  6, 26,  5,  4, 31 };

inline int log2_32(uint32_t value) {
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    return tab32[(uint32_t)(value * 0x07C4ACDD) >> 27];
}

namespace made {
    typedef uint8_t Digit;
    //typedef uint_fast8_t Digit;
    //typedef uint32_t Digit;
    //typedef uint16_t Digit;
    // typedef uint32_t Element; // 0..10^9 value range
    // typedef uint32_t ElementCounter; // 0..10^7 array size
    typedef unsigned int Element; // 0..10^9 value range
    typedef unsigned int ElementCounter; // 0..10^7 array size
    typedef uint_fast32_t FE;
    //typedef uint32_t FE;
    const uint_fast8_t ELEMENT_SIZE = sizeof(Element);
    const uint_fast8_t DIGIT_SIZE = sizeof(Digit);
    const uint_fast32_t MAX_DIGIT_COUNT = 1 << (DIGIT_SIZE * 8);
    const uint_fast32_t LOWER_DIGIT_MASK = MAX_DIGIT_COUNT - 1; // digit = 1 byte: 0xFF, digit = 2 byte: 0xFFFF, etc.;

    typedef uint16_t Digit2Byte;
    const uint_fast8_t DIGIT_2BYTE_SIZE = sizeof(Digit2Byte);
    const uint_fast32_t MAX_DIGIT_2BYTE_COUNT = 1 << (DIGIT_2BYTE_SIZE * 8);
    const uint_fast8_t DIGIT_2BYTE_STEP = ELEMENT_SIZE / DIGIT_2BYTE_SIZE;
    //const ElementCounter L1_CACHE_SIZE = 32 * 1024; // 32 KB // 2^13 = 8192 integers
    //const ElementCounter L1_CACHE_SIZE = 64 * 1024; // 64 KB // 2^14 = 16384 integers
    const ElementCounter L1_CACHE_SIZE = 32 * 8 * 1024; // 256 KB // 2^16 = 65536 integers
    const ElementCounter L2_CACHE_SIZE = 2 * 1024 * 1024; // 2 MB 2^19 = 524288 integers
    const ElementCounter L3_CACHE_SIZE = 20 * 1024 * 1024; // 20 MB = 20 * 2^20 = 5242880 integers
}


#define UNROL_LOOP(begin, size, digit_number, TIterator, iterator, additional_initializers, body)\
    {uint_fast32_t size_trunc_8 = (size) & (~0 << 3);\
    TIterator iterator = (TIterator)(begin) + (digit_number);\
    TIterator dend = (TIterator)((begin) + size_trunc_8);\
    additional_initializers;\
    for (; iterator < dend;) { body; body; body; body; body; body; body; body; }\
    for (dend = (TIterator)((begin) + (size)); iterator < dend;) { body; }}

#define SORT_DIGIT(begin, size, buffer, digit_size, shift)\
            const FE DIGIT_AMOUNT = 1 << (digit_size);\
            const FE DIGIT_MASK = DIGIT_AMOUNT - 1;\
            FE count[DIGIT_AMOUNT] = { 0 };\
            Element *pends[DIGIT_AMOUNT];\
            /*Counting amounts*/UNROL_LOOP((begin), (size), 0, Element*, dp, , ++count[(*dp >> (shift)) & DIGIT_MASK]; dp++);\
            /*Calculating places*/UNROL_LOOP(0, DIGIT_AMOUNT, 0, FE, i, Element *q = (buffer); , pends[i] = q; q += count[i++]);\
            /*Sorting - moving to ordered places*/UNROL_LOOP((begin), (size), 0, Element*, dp, , *pends[(*dp >> shift) & DIGIT_MASK]++ = *dp; dp++;);\

// bool skiped = false;\
            //for (FE i = 0; i < DIGIT_AMOUNT; ++i) {skiped = skiped || (count[i] == size);}\
            // if (!skiped) {\
            ///*Calculating places*/UNROL_LOOP(0, DIGIT_AMOUNT, 0, FE, i, Element *q = (buffer); , pends[i] = q; q += count[i++]);\
            ///*Sorting - moving to ordered places*/UNROL_LOOP((begin), (size), 0, Element*, dp, , *pends[(*dp >> shift) & DIGIT_MASK]++ = *dp; dp++;);\
            // }


namespace made {
    namespace bruteforce {
        struct dnums {
            FE D1, D2, D3, D4;
            FE D1shift, D2shift, D3shift, D4shift;
        };
        const dnums nums = { 8, 8, 8, 6, 0, 8, 16, 24 };
        void LSB_1_2(Element *begin, ElementCounter size, Element *buffer) {
            {SORT_DIGIT(begin, size, buffer, nums.D1, nums.D1shift); }
            {SORT_DIGIT(buffer, size, begin, nums.D2, nums.D2shift); }
        }
        void LSB_3times(Element *begin, ElementCounter size, Element *buffer) {
            {SORT_DIGIT(begin, size, buffer, nums.D1, nums.D1shift); }
            {SORT_DIGIT(buffer, size, begin, nums.D2, nums.D2shift); }
            {SORT_DIGIT(begin, size, buffer, nums.D1, nums.D1shift); }
        }
        void MSB3(Element *begin, ElementCounter size, Element *buffer) {
            SORT_DIGIT(begin, size, buffer, nums.D3, nums.D3shift)
                for (FE i = 0; i < DIGIT_AMOUNT; ++i) {/*for each digit call subsort*/
                    ElementCounter sub_size = count[i];
                    if (sub_size != 0) {// skipping empty buckets
                        Element *pend = pends[i];
                        Element *pstart = pend - sub_size;
                        LSB_1_2(pstart, sub_size, begin + (pstart - buffer));
                    }
                }
        }

        void MSB4(Element *begin, ElementCounter size, Element *buffer) {
            SORT_DIGIT(begin, size, buffer, nums.D4, nums.D4shift)
                for (FE i = 0; i < DIGIT_AMOUNT; ++i) {/*for each digit call subsort*/
                    ElementCounter sub_size = count[i];
                    if (sub_size != 0) {// skipping empty buckets
                        Element *pend = pends[i];
                        Element *pstart = pend - sub_size;
                        MSB3(pstart, sub_size, begin + (pstart - buffer));
                        //LSB_3times(pstart, sub_size, begin + (pstart - buffer));
                    }
                }
        }
        void MSBCaller(Element *begin, ElementCounter size) {
            Element *buffer = (Element *)malloc(sizeof(Element) * size);
            MSB4(begin, size, buffer);
            free(buffer);
        }
    }

    namespace avx {
        void LSB(Element *begin, ElementCounter size, Element *buffer, Element maxshift) {
            Element *end = begin + size;
            FE count[MAX_DIGIT_COUNT];
            Element *pends[MAX_DIGIT_COUNT];
            Digit* dp;
            Digit* dend;
            for (Element shift = 0; shift <= maxshift; shift += 8) {
                //Element shift = 0;
                std::memset(count, 0, sizeof(count));
                dp = (Digit*)(begin)+(shift >> 3); // digit pointer
                for (dend = (Digit*)(begin + (size & (~0 << 3))); dp < dend;) {
                    ++count[*dp]; dp += ELEMENT_SIZE;
                    ++count[*dp]; dp += ELEMENT_SIZE;
                    ++count[*dp]; dp += ELEMENT_SIZE;
                    ++count[*dp]; dp += ELEMENT_SIZE;
                    ++count[*dp]; dp += ELEMENT_SIZE;
                    ++count[*dp]; dp += ELEMENT_SIZE;
                    ++count[*dp]; dp += ELEMENT_SIZE;
                    ++count[*dp]; dp += ELEMENT_SIZE;
                }
                for (dp = (Digit*)(begin + (size & (~0 << 3))) + (shift >> 3), dend = (Digit*)(begin + size); dp < dend;) {
                    ++count[*dp]; dp += ELEMENT_SIZE;
                }
                //for (Element shift = 0; shift <= maxshift; shift += 8) {
                Element *q = buffer;
                for (ElementCounter i = 0; i < MAX_DIGIT_COUNT; q += count[i++]) {
                    pends[i] = q;
                }
                // std::cout << "Starting sorting" << std::endl;
                Element* arr_ptr = begin;
                Element *arr_end = begin + (size & (~0 << 3));
                dp = (Digit*)(begin)+(shift >> 3);
                for (; arr_ptr < arr_end;) {
                    *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                }
                for (arr_ptr = begin + (size & (~0 << 3)), arr_end = begin + size; arr_ptr < arr_end;) {
                    *pends[*dp]++ = *arr_ptr; ++arr_ptr; dp += ELEMENT_SIZE;
                }
                std::swap(begin, buffer);
            }
        }

        void LSB1(Element *begin, ElementCounter size, Element *buffer) {
            FE size_trunc_8 = size & (~0 << 3);
            FE count[MAX_DIGIT_COUNT] = { 0 };
            Element *pends[MAX_DIGIT_COUNT];
            /*Counting amounts*/ {
                Digit *dp0 = (Digit*)(begin);
                Digit *dend = (Digit*)(begin + size_trunc_8);
                for (; dp0 < dend;) {
                    ++count[*dp0]; dp0 += ELEMENT_SIZE;
                    ++count[*dp0]; dp0 += ELEMENT_SIZE;
                    ++count[*dp0]; dp0 += ELEMENT_SIZE;
                    ++count[*dp0]; dp0 += ELEMENT_SIZE;
                    ++count[*dp0]; dp0 += ELEMENT_SIZE;
                    ++count[*dp0]; dp0 += ELEMENT_SIZE;
                    ++count[*dp0]; dp0 += ELEMENT_SIZE;
                    ++count[*dp0]; dp0 += ELEMENT_SIZE;
                }
                for (dp0 = (Digit*)(begin + size_trunc_8), dend = (Digit*)(begin + size); dp0 < dend;) {
                    ++count[*dp0]; dp0 += ELEMENT_SIZE;
                }}
            /*Calculating places*/ {
                Element *q = buffer;
                for (FE i = 0; i < MAX_DIGIT_COUNT; ) {
                    pends[i] = q; q += count[i++];
                    pends[i] = q; q += count[i++];
                    pends[i] = q; q += count[i++];
                    pends[i] = q; q += count[i++];
                    pends[i] = q; q += count[i++];
                    pends[i] = q; q += count[i++];
                    pends[i] = q; q += count[i++];
                    pends[i] = q; q += count[i++];
                }}
            /*Sorting - moving to ordered places*/ {
                Element* arr_ptr = begin;
                Element *arr_end = begin + size_trunc_8;
                Digit *dp = (Digit*)(begin);
                for (; arr_ptr < arr_end;) {
                    *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                }
                for (arr_ptr = begin + size_trunc_8, arr_end = begin + size; arr_ptr < arr_end;) {
                    *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                }
            }
        }

        void LSB2(Element *begin, ElementCounter size, Element *buffer) {
            FE size_trunc_8 = size & (~0 << 3);
            const Digit num_digits = 2;
            FE counts[num_digits][MAX_DIGIT_COUNT] = { 0 };
            FE(&count0)[MAX_DIGIT_COUNT] = counts[0];
            FE(&count1)[MAX_DIGIT_COUNT] = counts[1];
            Element *pends_arr[num_digits][MAX_DIGIT_COUNT];
            /*Counting amounts*/ {
                Digit *dp0 = (Digit*)(begin), *dp1 = (Digit*)(begin)+1;
                Digit *dend = (Digit*)(begin + size_trunc_8);
                for (; dp0 < dend;) {
                    ++count0[*dp0]; dp0 += ELEMENT_SIZE; ++count1[*dp1]; dp1 += ELEMENT_SIZE;
                    ++count0[*dp0]; dp0 += ELEMENT_SIZE; ++count1[*dp1]; dp1 += ELEMENT_SIZE;
                    ++count0[*dp0]; dp0 += ELEMENT_SIZE; ++count1[*dp1]; dp1 += ELEMENT_SIZE;
                    ++count0[*dp0]; dp0 += ELEMENT_SIZE; ++count1[*dp1]; dp1 += ELEMENT_SIZE;
                    ++count0[*dp0]; dp0 += ELEMENT_SIZE; ++count1[*dp1]; dp1 += ELEMENT_SIZE;
                    ++count0[*dp0]; dp0 += ELEMENT_SIZE; ++count1[*dp1]; dp1 += ELEMENT_SIZE;
                    ++count0[*dp0]; dp0 += ELEMENT_SIZE; ++count1[*dp1]; dp1 += ELEMENT_SIZE;
                    ++count0[*dp0]; dp0 += ELEMENT_SIZE; ++count1[*dp1]; dp1 += ELEMENT_SIZE;
                }
                for (dend = (Digit*)(begin + size); dp0 < dend;) {
                    ++count0[*dp0]; dp0 += ELEMENT_SIZE; ++count1[*dp1]; dp1 += ELEMENT_SIZE;
                }}
            /*Calculating places*/ {
                Element*(&pend0)[MAX_DIGIT_COUNT] = pends_arr[0];
                Element*(&pend1)[MAX_DIGIT_COUNT] = pends_arr[1];
                Element *q0 = buffer; Element *q1 = begin;
                for (FE i = 0; i < MAX_DIGIT_COUNT; ) {
                    pend0[i] = q0; q0 += count0[i]; pend1[i] = q1; q1 += count1[i++];
                    pend0[i] = q0; q0 += count0[i]; pend1[i] = q1; q1 += count1[i++];
                    pend0[i] = q0; q0 += count0[i]; pend1[i] = q1; q1 += count1[i++];
                    pend0[i] = q0; q0 += count0[i]; pend1[i] = q1; q1 += count1[i++];
                    pend0[i] = q0; q0 += count0[i]; pend1[i] = q1; q1 += count1[i++];
                    pend0[i] = q0; q0 += count0[i]; pend1[i] = q1; q1 += count1[i++];
                    pend0[i] = q0; q0 += count0[i]; pend1[i] = q1; q1 += count1[i++];
                    pend0[i] = q0; q0 += count0[i]; pend1[i] = q1; q1 += count1[i++];
                }}
            /*Sorting - moving to ordered places*/ {
                for (FE i = 0; i < num_digits; ++i) {
                    Element *arr_ptr = begin;
                    Element *arr_end = begin + size_trunc_8;
                    Element*(&pend)[MAX_DIGIT_COUNT] = pends_arr[i];
                    Digit *dp = (Digit*)(begin)+i;
                    for (; arr_ptr < arr_end;) {
                        *pend[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                        *pend[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                        *pend[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                        *pend[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                        *pend[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                        *pend[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                        *pend[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                        *pend[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    }
                    for (arr_ptr = begin + size_trunc_8, arr_end = begin + size; arr_ptr < arr_end;) {
                        *pend[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    }
                    std::swap(begin, buffer);
                }}
        }

        void LSB3(Element *begin, ElementCounter size, Element *buffer) {
            FE size_trunc_8 = size & (~0 << 3);
            const Digit num_digits = 3;
            FE counts[num_digits][MAX_DIGIT_COUNT] = { 0 };
            FE(&count0)[MAX_DIGIT_COUNT] = counts[0];
            FE(&count1)[MAX_DIGIT_COUNT] = counts[1];
            FE(&count2)[MAX_DIGIT_COUNT] = counts[2];
            Element *pends_arr[num_digits][MAX_DIGIT_COUNT];
            /*Counting amounts*/ {
                Digit *dp0 = (Digit*)(begin), *dp1 = (Digit*)(begin)+1, *dp2 = (Digit*)(begin)+2;
                Digit *dend = (Digit*)(begin + size_trunc_8);
                for (; dp0 < dend;) {
                    ++count0[*dp0]; dp0 += ELEMENT_SIZE; ++count1[*dp1]; dp1 += ELEMENT_SIZE; ++count2[*dp2]; dp2 += ELEMENT_SIZE;
                    ++count0[*dp0]; dp0 += ELEMENT_SIZE; ++count1[*dp1]; dp1 += ELEMENT_SIZE; ++count2[*dp2]; dp2 += ELEMENT_SIZE;
                    ++count0[*dp0]; dp0 += ELEMENT_SIZE; ++count1[*dp1]; dp1 += ELEMENT_SIZE; ++count2[*dp2]; dp2 += ELEMENT_SIZE;
                    ++count0[*dp0]; dp0 += ELEMENT_SIZE; ++count1[*dp1]; dp1 += ELEMENT_SIZE; ++count2[*dp2]; dp2 += ELEMENT_SIZE;
                    ++count0[*dp0]; dp0 += ELEMENT_SIZE; ++count1[*dp1]; dp1 += ELEMENT_SIZE; ++count2[*dp2]; dp2 += ELEMENT_SIZE;
                    ++count0[*dp0]; dp0 += ELEMENT_SIZE; ++count1[*dp1]; dp1 += ELEMENT_SIZE; ++count2[*dp2]; dp2 += ELEMENT_SIZE;
                    ++count0[*dp0]; dp0 += ELEMENT_SIZE; ++count1[*dp1]; dp1 += ELEMENT_SIZE; ++count2[*dp2]; dp2 += ELEMENT_SIZE;
                    ++count0[*dp0]; dp0 += ELEMENT_SIZE; ++count1[*dp1]; dp1 += ELEMENT_SIZE; ++count2[*dp2]; dp2 += ELEMENT_SIZE;
                }
                for (dp0 = (Digit*)(begin + size_trunc_8), dend = (Digit*)(begin + size); dp0 < dend;) {
                    ++count0[*dp0]; dp0 += ELEMENT_SIZE; ++count1[*dp1]; dp1 += ELEMENT_SIZE; ++count2[*dp2]; dp2 += ELEMENT_SIZE;
                }}
            /*Calculating places*/ {
                Element*(&pend0)[MAX_DIGIT_COUNT] = pends_arr[0];
                Element*(&pend1)[MAX_DIGIT_COUNT] = pends_arr[1];
                Element*(&pend2)[MAX_DIGIT_COUNT] = pends_arr[2];
                Element *q0 = buffer; Element *q1 = begin; Element *q2 = buffer;
                for (FE i = 0; i < MAX_DIGIT_COUNT; ) {
                    pend0[i] = q0; q0 += count0[i]; pend1[i] = q1; q1 += count1[i]; pend2[i] = q2; q2 += count2[i++];
                    pend0[i] = q0; q0 += count0[i]; pend1[i] = q1; q1 += count1[i]; pend2[i] = q2; q2 += count2[i++];
                    pend0[i] = q0; q0 += count0[i]; pend1[i] = q1; q1 += count1[i]; pend2[i] = q2; q2 += count2[i++];
                    pend0[i] = q0; q0 += count0[i]; pend1[i] = q1; q1 += count1[i]; pend2[i] = q2; q2 += count2[i++];
                    pend0[i] = q0; q0 += count0[i]; pend1[i] = q1; q1 += count1[i]; pend2[i] = q2; q2 += count2[i++];
                    pend0[i] = q0; q0 += count0[i]; pend1[i] = q1; q1 += count1[i]; pend2[i] = q2; q2 += count2[i++];
                    pend0[i] = q0; q0 += count0[i]; pend1[i] = q1; q1 += count1[i]; pend2[i] = q2; q2 += count2[i++];
                    pend0[i] = q0; q0 += count0[i]; pend1[i] = q1; q1 += count1[i]; pend2[i] = q2; q2 += count2[i++];
                }}
            /*Sorting - moving to ordered places*/ {
                for (FE i = 0; i < num_digits; ++i) {
                    Element *arr_ptr = begin;
                    Element *arr_end = begin + size_trunc_8;
                    Element*(&pend)[MAX_DIGIT_COUNT] = pends_arr[i];
                    Digit *dp = (Digit*)(begin)+i;
                    for (; arr_ptr < arr_end;) {
                        *pend[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                        *pend[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                        *pend[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                        *pend[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                        *pend[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                        *pend[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                        *pend[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                        *pend[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    }
                    for (arr_ptr = begin + size_trunc_8, arr_end = begin + size; arr_ptr < arr_end;) {
                        *pend[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    }
                    std::swap(begin, buffer);
                }}
        }

        void MSB_extra_mem_2(Element *begin, ElementCounter size, Element *buffer) {
            FE count[MAX_DIGIT_COUNT] = { 0 };
            Element *pends[MAX_DIGIT_COUNT];
            const FE size_trunc_8 = size & (~0 << 3);
            const FE shift = 8;
            const FE num_digit = shift >> 3;
            /*Counting amounts*/ {
                Digit *dp = (Digit*)(begin)+num_digit;
                Digit *dend = (Digit*)(begin + size_trunc_8);
                for (; dp < dend;) {
                    ++count[*dp]; dp += ELEMENT_SIZE;
                    ++count[*dp]; dp += ELEMENT_SIZE;
                    ++count[*dp]; dp += ELEMENT_SIZE;
                    ++count[*dp]; dp += ELEMENT_SIZE;
                    ++count[*dp]; dp += ELEMENT_SIZE;
                    ++count[*dp]; dp += ELEMENT_SIZE;
                    ++count[*dp]; dp += ELEMENT_SIZE;
                    ++count[*dp]; dp += ELEMENT_SIZE;
                }
                for (dp = (Digit*)(begin + size_trunc_8) + num_digit, dend = (Digit*)(begin + size); dp < dend;) {
                    ++count[*dp]; dp += ELEMENT_SIZE;
                }}
            /*Calculating places*/ {
                Element *q = buffer;
                for (FE i = 0; i < MAX_DIGIT_COUNT;) {
                    pends[i] = q; q += count[i++];
                    pends[i] = q; q += count[i++];
                    pends[i] = q; q += count[i++];
                    pends[i] = q; q += count[i++];
                    pends[i] = q; q += count[i++];
                    pends[i] = q; q += count[i++];
                    pends[i] = q; q += count[i++];
                    pends[i] = q; q += count[i++];
                }}
            /*Sorting - moving to ordered places*/ {
                Element *arr_ptr = begin;
                Element *arr_end = begin + size_trunc_8;
                Digit *dp = (Digit*)(begin)+num_digit;
                for (; arr_ptr < arr_end;) {
                    *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                }
                for (arr_ptr = begin + size_trunc_8, arr_end = begin + size; arr_ptr < arr_end;) {
                    *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                }}
            /*for each digit call subsort*/ {
                for (FE i = 0; i < MAX_DIGIT_COUNT; ++i) {
                    ElementCounter sub_size = count[i];
                    if (sub_size != 0) {// skipping empty buckets
                        Element *pend = pends[i];
                        Element *pstart = pend - sub_size;
                        LSB1(pstart, sub_size, begin + (pstart - buffer));
                        //InsertionSort(pstart, sub_size);
                    }
                }}
        }

        void MSB_extra_mem_3(Element *begin, ElementCounter size, Element *buffer) {
            const FE DIGIT_AMOUNT = MAX_DIGIT_COUNT;
            FE count[DIGIT_AMOUNT] = { 0 };
            Element *pends[DIGIT_AMOUNT];
            const FE size_trunc_8 = size & (~0 << 3);
            const FE shift = 16;
            const FE num_digit = shift >> 3;
            /*Counting amounts*/
            UNROL_LOOP(begin, size, num_digit, Digit*, dp, , ++count[*dp]; dp += ELEMENT_SIZE;);
            /*Calculating places*/
            UNROL_LOOP(0, DIGIT_AMOUNT, 0, FE, i, Element *q = buffer;, pends[i] = q; q += count[i++]);
            /*Sorting - moving to ordered places*/
            UNROL_LOOP(begin, size, 0, Element*, arr_ptr, Digit *dp = (Digit*)(begin)+num_digit; , *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;);
            /*for each digit call subsort*/ {
                for (FE i = 0; i < MAX_DIGIT_COUNT; ++i) {
                    ElementCounter sub_size = count[i];
                    if (sub_size != 0) {// skipping empty buckets
                        Element *pend = pends[i];
                        Element *pstart = pend - sub_size;
                        LSB2(pstart, sub_size, begin + (pstart - buffer));
                    }
                }}
        }

        void decide_steps(ElementCounter size, Element max) {
            const Element highest_bit = 30;//log2_32(max)+1;
            // divide by 2 to fit array copy
            const ElementCounter L1_SIZE = L1_CACHE_SIZE / sizeof(Element) / 2; // 8192 integers
            const ElementCounter L2_SIZE = L2_CACHE_SIZE / sizeof(Element) / 2; // 262144 integers
            if (size < L1_SIZE) {
                const FE maxshift = (highest_bit >> 3) << 2;
                //LSB from 0 to maxshift
            }
            else {
                // MSB step. After step check
            }
            /*
            size >
            */
        }

        void MSB_extra_mem_4(Element *begin, ElementCounter size, Element *buffer) {
            const FE DIGIT_AMOUNT = MAX_DIGIT_COUNT >> 2;
            FE count[DIGIT_AMOUNT] = { 0 };
            Element *pends[DIGIT_AMOUNT];
            //const FE size_trunc_8 = size & (~0 << 3);
            const FE shift = 24;
            const FE num_digit = shift >> 3; // 3
            FE max = *begin;
            /*Counting amounts and getting maximum*/
            UNROL_LOOP(begin, size, num_digit, Digit*, dp, Element *arr_ptr = begin; ,
                ++count[*dp]; dp += ELEMENT_SIZE; max = *arr_ptr > max ? *arr_ptr : max; ++arr_ptr;);
            /*Checking if we need to skip steps*/ {
                decide_steps(size, max);
                if (max < 1 << 16) {
                    LSB2(begin, size, buffer);
                    return;
                }
                else if (max < 1 << 24) {
                    MSB_extra_mem_3(begin, size, buffer);
                    memcpy(begin, buffer, size * 4);
                    return;
                }
            }
            //Calculating places
            UNROL_LOOP(0, DIGIT_AMOUNT, 0, FE, i, Element *q = buffer;, pends[i] = q; q += count[i++]);
            //Sorting - moving to ordered places
            UNROL_LOOP(begin, size, 0, Element*, arr_ptr, Digit *dp = (Digit*)(begin)+num_digit; , *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;);
            /*for each digit call subsort*/ {
                for (FE i = 0; i < DIGIT_AMOUNT; ++i) {
                    ElementCounter sub_size = count[i];
                    if (sub_size != 0) {// skipping empty buckets
                        Element *pend = pends[i];
                        Element *pstart = pend - sub_size;
                        MSB_extra_mem_3(pstart, sub_size, begin + (pstart - buffer));
                    }
                }}
        }

        void k9_sort(Element *begin, ElementCounter size) {
            Element *buffer = (Element *)malloc(sizeof(Element) * size);
            MSB_extra_mem_4(begin, size, buffer);// currently da best
            //memcpy(buffer, begin, size * sizeof(Element));
            free(buffer);
        }
    }

}

#ifndef MADE_CUSTOM_SORT_H_
#define MADE_CUSTOM_SORT_H_
void Sort(made::Element *arr, made::ElementCounter size) {
    made::avx::k9_sort(arr, size);
}
void Sort(int *arr, int size) {
    Sort((made::Element*)arr, (made::ElementCounter)size);
}
#endif // !MADE_CUSTOM_SORT_H_

#endif // !K9_AVX_H_