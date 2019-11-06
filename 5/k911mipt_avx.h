#ifndef K9_AVX_H_
#define K9_AVX_H_

//#include "common.h"
#include "sort.h"
#include "stdint.h"
#include <algorithm>
#include <cstring>
#include <cassert>

namespace made {
    typedef uint8_t Digit;
    //typedef uint_fast8_t Digit;
    //typedef uint32_t Digit;
    //typedef uint16_t Digit;
    // typedef uint32_t Element; // 0..10^9 value range
    // typedef uint32_t ElementCounter; // 0..10^7 array size
    typedef unsigned int Element; // 0..10^9 value range
    typedef unsigned int ElementCounter; // 0..10^7 array size
    //typedef uint_fast32_t FE;
    typedef uint32_t FE;
    const uint_fast8_t ELEMENT_SIZE = sizeof(Element);
    const uint_fast8_t DIGIT_SIZE = sizeof(Digit);
    const uint_fast32_t MAX_DIGIT_COUNT = 1 << (DIGIT_SIZE * 8);
    const uint_fast32_t LOWER_DIGIT_MASK = MAX_DIGIT_COUNT - 1; // digit = 1 byte: 0xFF, digit = 2 byte: 0xFFFF, etc.;

    typedef uint16_t Digit2Byte;
    const uint_fast8_t DIGIT_2BYTE_SIZE = sizeof(Digit2Byte);
    const uint_fast32_t MAX_DIGIT_2BYTE_COUNT = 1 << (DIGIT_2BYTE_SIZE * 8);
    const uint_fast8_t DIGIT_2BYTE_STEP = ELEMENT_SIZE / DIGIT_2BYTE_SIZE;
    const uint32_t L1_SIZE = 64 * 1024; // 64 KB // 2^16 = 16384 integers
    const uint32_t L2_SIZE = 2 * 1024 * 1024; // 2 MB 2^19 = 524288 integers
    const uint32_t L3_SIZE = 20 * 1024 * 1024; // 20 MB = 20 * 2^20 = 5242880 integers

    const Digit DMax = 30;
    const FE D4 = 6; const Digit D4shift = DMax - D4;
    const FE D3 = 8; const Digit D3shift = D4shift - D3;
    const FE D2 = 8; const Digit D2shift = D3shift - D2;
    const FE D1 = D2shift; const Digit D1shift = D2shift - D1;
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
    namespace avx {
        void LSB(Element *begin, ElementCounter size, Element *buffer, Element maxshift) {
            Element *end = begin + size;
            //Element *end1 = buffer + size;
            FE count[MAX_DIGIT_COUNT];
            Element *pends[MAX_DIGIT_COUNT];
            Digit* dp;
            Digit* dend;
            for (Element shift = 0; shift <= maxshift; shift += 8)
            {
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
        void LSBUniversal(Element *begin, ElementCounter size, Element *buffer) {
            const Digit num_digits = 2;
            const Digit shift = 8;
            const Digit maxshift = shift << 1;
            const FE DIGIT_AMOUNT = 1 << shift;
            const FE DIGIT_MASK = DIGIT_AMOUNT - 1;
            FE count[DIGIT_AMOUNT];
            Element *pends[DIGIT_AMOUNT];
            for (Digit d = 0; d < num_digits; ++d) {
                std::memset(count, 0, sizeof(count));
                const Digit cur_shift = d * shift;
                Element *dp = begin, *dend = begin + size;
                for (; dp != dend;) {
                    ++count[(*dp >> cur_shift) & DIGIT_MASK]; dp++;
                }
                Element *q = buffer;
                for (FE i = 0; i != DIGIT_AMOUNT; q += count[i++]) {
                    pends[i] = q;
                }
                for (dp = begin; dp != dend;) {
                    *pends[(*dp >> cur_shift) & DIGIT_MASK]++ = *dp; dp++;
                }
                std::swap(begin, buffer);
            }
        }

        void LSB_3(Element *begin, ElementCounter size, Element *buffer) {
            SORT_DIGIT(begin, size, buffer, D3, D3shift)
        }

        void LSB_2(Element *begin, ElementCounter size, Element *buffer) {
            const Digit digit_size = D2;
            const FE shift = D2shift;
            const FE DIGIT_AMOUNT = 1 << digit_size;
            const FE DIGIT_MASK = DIGIT_AMOUNT - 1;
            FE count[DIGIT_AMOUNT] = { 0 };
            Element *pends[DIGIT_AMOUNT];
            UNROL_LOOP(begin, size, 0, Element*, dp, , ++count[(*dp >> shift) & DIGIT_MASK]; dp++);
            UNROL_LOOP(0, DIGIT_AMOUNT, 0, FE, i, Element *q = buffer; , pends[i] = q; q += count[i++]);
            UNROL_LOOP(begin, size, 0, Element*, dp, , *pends[(*dp >> shift) & DIGIT_MASK]++ = *dp; dp++;);
        }

        void LSB_1(Element *begin, ElementCounter size, Element *buffer) {
            const Digit digit_size = D1;
            const Digit shift = D1shift;
            const FE DIGIT_AMOUNT = 1 << digit_size;
            const FE DIGIT_MASK = DIGIT_AMOUNT - 1;
            FE count[DIGIT_AMOUNT] = { 0 };
            Element *pends[DIGIT_AMOUNT];
            UNROL_LOOP(begin, size, 0, Element*, dp, , ++count[(*dp >> shift) & DIGIT_MASK]; dp++);
            UNROL_LOOP(0, DIGIT_AMOUNT, 0, FE, i, Element *q = buffer; , pends[i] = q; q += count[i++]);
            UNROL_LOOP(begin, size, 0, Element*, dp, , *pends[(*dp >> shift) & DIGIT_MASK]++ = *dp; dp++;);
            LSB_2(buffer, size, begin);
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

        void MSB2(Element *begin, ElementCounter size, Element *buffer) {
            const Digit digit_size = D2 + D1;
            const FE shift = D1shift;
            const FE DIGIT_AMOUNT = 1 << digit_size;
            const FE DIGIT_MASK = DIGIT_AMOUNT - 1;
            FE count[DIGIT_AMOUNT] = { 0 };
            Element *pends[DIGIT_AMOUNT];
            UNROL_LOOP(begin, size, 0, Element*, dp, , ++count[(*dp >> shift) & DIGIT_MASK]; dp++);
            UNROL_LOOP(0, DIGIT_AMOUNT, 0, FE, i, Element *q = buffer; , pends[i] = q; q += count[i++]);
            UNROL_LOOP(begin, size, 0, Element*, dp, , *pends[(*dp >> shift) & DIGIT_MASK]++ = *dp; dp++;);
            // for (FE i = 0; i < DIGIT_AMOUNT; ++i) {/*for each digit call subsort*/
            //     ElementCounter sub_size = count[i];
            //     if (sub_size != 0) {// skipping empty buckets
            //         Element *pend = pends[i];
            //         Element *pstart = pend - sub_size;
            //         // LSB_1(pstart, sub_size, begin + (pstart - buffer));
            //         // LSB2(pstart, sub_size, begin + (pstart - buffer));
            //     }
            // }
        }

        void MSB3(Element *begin, ElementCounter size, Element *buffer) {
            SORT_DIGIT(begin, size, buffer, D3, D3shift)
            for (FE i = 0; i < DIGIT_AMOUNT; ++i) {/*for each digit call subsort*/
                ElementCounter sub_size = count[i];
                if (sub_size != 0) {// skipping empty buckets
                    Element *pend = pends[i];
                    Element *pstart = pend - sub_size;
                    LSB_1(pstart, sub_size, begin + (pstart - buffer));
                    // {SORT_DIGIT(pstart, sub_size, begin + (pstart - buffer), D1, D1shift)}
                    // {SORT_DIGIT(begin + (pstart - buffer), sub_size, pstart, D2, D2shift)}
                    // LSB_1(pstart, sub_size, begin + (pstart - buffer));
                    // LSB2(pstart, sub_size, begin + (pstart - buffer));
                    // MSB2(pstart, sub_size, begin + (pstart - buffer));
                }
            }
        }

        void MSB4(Element *begin, ElementCounter size, Element *buffer) {
            SORT_DIGIT(begin, size, buffer, D4, D4shift)
            for (FE i = 0; i < DIGIT_AMOUNT; ++i) {/*for each digit call subsort*/
                ElementCounter sub_size = count[i];
                if (sub_size != 0) {// skipping empty buckets
                    Element *pend = pends[i];
                    Element *pstart = pend - sub_size;
                    MSB3(pstart, sub_size, begin + (pstart - buffer));
                    // {SORT_DIGIT(pstart, sub_size, begin + (pstart - buffer), D1, D1shift)}
                    // {SORT_DIGIT(begin + (pstart - buffer), sub_size, pstart, D2, D2shift)}
                    // {SORT_DIGIT(pstart, sub_size, begin + (pstart - buffer), D3, D3shift)}
                }
            }
            // memcpy(begin, buffer, size * sizeof(Element));
        }

        void MSB34(Element *begin, ElementCounter size, Element *buffer) {
            const Digit digit_size = D4+D3;
            const Digit shift = D3shift;
            const FE DIGIT_AMOUNT = 1 << digit_size;
            const FE DIGIT_MASK = DIGIT_AMOUNT - 1;
            FE count[DIGIT_AMOUNT] = { 0 };
            Element *pends[DIGIT_AMOUNT];
            FE size_trunc_8 = size & (~0 << 3);
            Element *dp = begin, *dend = begin + size_trunc_8;
            for (; dp != dend;) {
                ++count[(*dp >> shift) & DIGIT_MASK]; dp++;
                ++count[(*dp >> shift) & DIGIT_MASK]; dp++;
                ++count[(*dp >> shift) & DIGIT_MASK]; dp++;
                ++count[(*dp >> shift) & DIGIT_MASK]; dp++;
                ++count[(*dp >> shift) & DIGIT_MASK]; dp++;
                ++count[(*dp >> shift) & DIGIT_MASK]; dp++;
                ++count[(*dp >> shift) & DIGIT_MASK]; dp++;
                ++count[(*dp >> shift) & DIGIT_MASK]; dp++;
            }
            for (dend = begin + size; dp != dend;) {
                ++count[(*dp >> shift) & DIGIT_MASK]; dp++;
            }
            Element *q = buffer;
            for (FE i = 0; i != DIGIT_AMOUNT; q += count[i++]) {
                pends[i] = q;
            }
            for (dp = begin, dend = begin + size_trunc_8; dp != dend;) {
                *pends[(*dp >> shift) & DIGIT_MASK]++ = *dp; dp++;
                *pends[(*dp >> shift) & DIGIT_MASK]++ = *dp; dp++;
                *pends[(*dp >> shift) & DIGIT_MASK]++ = *dp; dp++;
                *pends[(*dp >> shift) & DIGIT_MASK]++ = *dp; dp++;
                *pends[(*dp >> shift) & DIGIT_MASK]++ = *dp; dp++;
                *pends[(*dp >> shift) & DIGIT_MASK]++ = *dp; dp++;
                *pends[(*dp >> shift) & DIGIT_MASK]++ = *dp; dp++;
                *pends[(*dp >> shift) & DIGIT_MASK]++ = *dp; dp++;
            }
            for (dend = begin + size; dp != dend;) {
                *pends[(*dp >> shift) & DIGIT_MASK]++ = *dp; dp++;
            }
            for (FE i = 0; i < DIGIT_AMOUNT; ++i) {/*for each digit call subsort*/
                ElementCounter sub_size = count[i];
                if (sub_size != 0) {// skipping empty buckets
                    Element *pend = pends[i];
                    Element *pstart = pend - sub_size;
                    LSB_1(pstart, sub_size, begin + (pstart - buffer));
                }
            }
            memcpy(begin, buffer, size * sizeof(Element));
        }

        void LSB2_9bits(Element *begin, ElementCounter size, Element *buffer) {
            const FE MAX_HIGH_DIGIT_COUNT = MAX_DIGIT_COUNT << 1;
            const FE MASK = 0x1FF;
            const FE size_trunc_8 = size & (~0 << 3);
            const Digit2Byte num_digits = 2;
            FE counts[num_digits][MAX_HIGH_DIGIT_COUNT] = { 0 };
            FE(&count0)[MAX_HIGH_DIGIT_COUNT] = counts[0];
            FE(&count1)[MAX_HIGH_DIGIT_COUNT] = counts[1];
            Element *pends_arr[num_digits][MAX_HIGH_DIGIT_COUNT];
            /*Counting amounts*/ {
                Element *dp = begin;
                Element *dend = (begin + size_trunc_8);
                for (; dp < dend;) {
                    ++count0[*dp & MASK]; ++count1[(*dp >> 9) & MASK]; dp++;
                    ++count0[*dp & MASK]; ++count1[(*dp >> 9) & MASK]; dp++;
                    ++count0[*dp & MASK]; ++count1[(*dp >> 9) & MASK]; dp++;
                    ++count0[*dp & MASK]; ++count1[(*dp >> 9) & MASK]; dp++;
                    ++count0[*dp & MASK]; ++count1[(*dp >> 9) & MASK]; dp++;
                    ++count0[*dp & MASK]; ++count1[(*dp >> 9) & MASK]; dp++;
                    ++count0[*dp & MASK]; ++count1[(*dp >> 9) & MASK]; dp++;
                    ++count0[*dp & MASK]; ++count1[(*dp >> 9) & MASK]; dp++;
                }
                for (dend = (begin + size); dp < dend;) {
                    ++count0[*dp & MASK]; ++count1[(*dp >> 9) & MASK]; dp++;
                }}
            Element*(&pend0)[MAX_HIGH_DIGIT_COUNT] = pends_arr[0];
            Element*(&pend1)[MAX_HIGH_DIGIT_COUNT] = pends_arr[1];
            /*Calculating places*/ {
                Element *q0 = buffer; Element *q1 = begin;
                for (FE i = 0; i < MAX_HIGH_DIGIT_COUNT; ) {
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
                    Element *dp = begin;
                    Element *dend = (begin + size_trunc_8);
                    for (; dp < dend;) {
                        *pend0[*dp & MASK]++ = *dp++;
                        *pend0[*dp & MASK]++ = *dp++;
                        *pend0[*dp & MASK]++ = *dp++;
                        *pend0[*dp & MASK]++ = *dp++;
                        *pend0[*dp & MASK]++ = *dp++;
                        *pend0[*dp & MASK]++ = *dp++;
                        *pend0[*dp & MASK]++ = *dp++;
                        *pend0[*dp & MASK]++ = *dp++;
                    }
                    for (dend = (begin + size); dp < dend;) {
                        *pend0[*dp & MASK]++ = *dp++;
                    }
                    std::swap(begin, buffer);
                    dp = begin;
                    dend = (begin + size_trunc_8);
                    Element*(&pend)[MAX_HIGH_DIGIT_COUNT] = pends_arr[1];
                    for (; dp < dend;) {
                        *pend1[(*dp >> 9) & MASK]++ = *dp++;
                        *pend1[(*dp >> 9) & MASK]++ = *dp++;
                        *pend1[(*dp >> 9) & MASK]++ = *dp++;
                        *pend1[(*dp >> 9) & MASK]++ = *dp++;
                        *pend1[(*dp >> 9) & MASK]++ = *dp++;
                        *pend1[(*dp >> 9) & MASK]++ = *dp++;
                        *pend1[(*dp >> 9) & MASK]++ = *dp++;
                        *pend1[(*dp >> 9) & MASK]++ = *dp++;
                    }
                    for (dend = (begin + size); dp < dend;) {
                        *pend1[(*dp >> 9) & MASK]++ = *dp++;
                    }
                }
        }

        void MSB_inplace_3(Element *begin, ElementCounter size, Element *buffer) {
            FE count[MAX_DIGIT_COUNT] = { 0 };
            Element *pends[MAX_DIGIT_COUNT];
            Element *pstarts[MAX_DIGIT_COUNT];
            const FE size_trunc_8 = size & (~0 << 3);
            const FE shift = 16;
            const FE num_digit = shift >> 3;
            /*Counting amounts*/ {
                Digit* dp = (Digit*)(begin)+num_digit;
                Digit* dend = (Digit*)(begin + size_trunc_8);
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
                Element *q = begin;
                for (FE i = 0; i < MAX_DIGIT_COUNT;) {
                    pstarts[i] = q; q += count[i]; pends[i++] = q;
                    pstarts[i] = q; q += count[i]; pends[i++] = q;
                    pstarts[i] = q; q += count[i]; pends[i++] = q;
                    pstarts[i] = q; q += count[i]; pends[i++] = q;
                    pstarts[i] = q; q += count[i]; pends[i++] = q;
                    pstarts[i] = q; q += count[i]; pends[i++] = q;
                    pstarts[i] = q; q += count[i]; pends[i++] = q;
                    pstarts[i] = q; q += count[i]; pends[i++] = q;
                }}
            /*Sorting inplace - moving to ordered places*/ {
                for (FE i = 0; i < MAX_DIGIT_COUNT; ++i) {
                    while (pstarts[i] != pends[i]) {
                        FE value = *pstarts[i];
                        FE y = (value >> shift) & LOWER_DIGIT_MASK;
                        while (i != y) {
                            FE temp = *pstarts[y];
                            *(pstarts[y]++) = value;
                            value = temp;
                            y = (value >> shift) & LOWER_DIGIT_MASK;
                        }
                        *(pstarts[i]++) = value;
                    }
                }
            }
            /*for each digit call subsort in LSB order*/ {
                for (FE i = 0; i < MAX_DIGIT_COUNT; ++i) {
                    ElementCounter sub_size = count[i];
                    if (sub_size != 0) {// skipping empty buckets
                        Element *pend = pends[i];
                        Element *pstart = pend - sub_size;
                        LSB2(pstart, sub_size, buffer + (pstart - begin));
                    }
                }}
        }

        void MSB_inplace_4(Element *begin, ElementCounter size, Element *buffer) {
            const FE MAX_HIGH_DIGIT_COUNT = MAX_DIGIT_COUNT >> 2; // 10^9 is the highest number in data
            FE count[MAX_HIGH_DIGIT_COUNT] = { 0 };
            Element *pends[MAX_HIGH_DIGIT_COUNT];
            Element *pstarts[MAX_HIGH_DIGIT_COUNT];
            const FE size_trunc_8 = size & (~0 << 3);
            const FE shift = 24;
            const FE num_digit = shift >> 3;
            /*Counting amounts*/ {
                Digit* dp = (Digit*)(begin)+num_digit;
                Digit* dend = (Digit*)(begin + size_trunc_8);
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
                Element *q = begin;
                for (FE i = 0; i < MAX_HIGH_DIGIT_COUNT;) {
                    pstarts[i] = q; q += count[i]; pends[i++] = q;
                    pstarts[i] = q; q += count[i]; pends[i++] = q;
                    pstarts[i] = q; q += count[i]; pends[i++] = q;
                    pstarts[i] = q; q += count[i]; pends[i++] = q;
                    pstarts[i] = q; q += count[i]; pends[i++] = q;
                    pstarts[i] = q; q += count[i]; pends[i++] = q;
                    pstarts[i] = q; q += count[i]; pends[i++] = q;
                    pstarts[i] = q; q += count[i]; pends[i++] = q;
                }}
            /*Sorting inplace - moving to ordered places*/ {
                for (FE i = 0; i < MAX_HIGH_DIGIT_COUNT; ++i) {
                    while (pstarts[i] != pends[i]) {
                        FE value = *pstarts[i];
                        FE y = (value >> shift) & LOWER_DIGIT_MASK;
                        while (i != y) {
                            FE temp = *pstarts[y];
                            *(pstarts[y]++) = value;
                            value = temp;
                            y = (value >> shift) & LOWER_DIGIT_MASK;
                        }
                        *(pstarts[i]++) = value;
                    }
                }
            }
            /*for each digit call subsort*/ {
                for (FE i = 0; i < MAX_HIGH_DIGIT_COUNT; ++i) {
                    ElementCounter sub_size = count[i];
                    if (sub_size != 0) {// skipping empty buckets
                        Element *pend = pends[i];
                        Element *pstart = pend - sub_size;
                        LSB3(pstart, sub_size, buffer + (pstart - begin));
                    }
                }}
            memcpy(begin, buffer, size * 4);
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
            UNROL_LOOP(begin, size, num_digit, Digit*, dp, Element *arr_ptr = begin;, ++count[*dp]; dp += ELEMENT_SIZE;);
            /*Calculating places*/
            UNROL_LOOP(0, DIGIT_AMOUNT, 0, FE, i, Element *q = buffer; , pends[i] = q; q += count[i++]);
            /*Sorting - moving to ordered places*/
            UNROL_LOOP(begin, size, 0, Element*, arr_ptr, Digit *dp = (Digit*)(begin)+num_digit;, *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;);
            /*for each digit call subsort*/ {
                for (FE i = 0; i < MAX_DIGIT_COUNT; ++i) {
                    ElementCounter sub_size = count[i];
                    if (sub_size != 0) {// skipping empty buckets
                        Element *pend = pends[i];
                        Element *pstart = pend - sub_size;
                        LSB_1(pstart, sub_size, begin + (pstart - buffer));
                        // LSB1(pstart, sub_size, begin + (pstart - buffer));
                        // LSB_2(begin + (pstart - buffer), sub_size, pstart);
                        // LSB(pstart, sub_size, begin + (pstart - buffer), 8);
                        // LSBUniversal(pstart, sub_size, begin + (pstart - buffer));
                        //MSB_extra_mem_2(pstart, sub_size, begin + (pstart - buffer));
                        //MSB_inplace_3(pstart, sub_size, begin + (pstart - buffer));
                    }
                }}
        }

        void MSB_extra_mem_4(Element *begin, ElementCounter size, Element *buffer) {
            const FE DIGIT_AMOUNT = MAX_DIGIT_COUNT >> 2;
            FE count[DIGIT_AMOUNT] = { 0 };
            Element *pends[DIGIT_AMOUNT];
            const FE size_trunc_8 = size & (~0 << 3);
            const FE shift = 24;
            const FE num_digit = shift >> 3;
            FE max = *begin;
            /*Counting amounts and getting maximum*/
            UNROL_LOOP(begin, size, num_digit, Digit*, dp, Element *arr_ptr = begin;,
                ++count[*dp]; dp += ELEMENT_SIZE; max = *arr_ptr > max ? *arr_ptr : max; ++arr_ptr;);
            /*Checking if we need to skip steps*/ {
                if (max < 1 << 16) {
                    LSB_1(begin, size, buffer);
                    return;
                }
                else if (max < 1 << 24) {
                    MSB_extra_mem_3(begin, size, buffer);
                    memcpy(begin, buffer, size * 4);
                    return;
                }
            }
            //Calculating places
            UNROL_LOOP(0, DIGIT_AMOUNT, 0, FE, i, Element *q = buffer; , pends[i] = q; q += count[i++]);
            //Sorting - moving to ordered places
            UNROL_LOOP(begin, size, 0, Element*, arr_ptr, Digit *dp = (Digit*)(begin)+num_digit;, *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;);
            /*for each digit call subsort*/ {
                for (FE i = 0; i < DIGIT_AMOUNT; ++i) {
                    ElementCounter sub_size = count[i];
                    if (sub_size != 0) {// skipping empty buckets
                        Element *pend = pends[i];
                        Element *pstart = pend - sub_size;
                        //LSB3(pstart, sub_size, begin + (pstart - buffer));
                        //MSB_inplace_3(pstart, sub_size, begin + (pstart - buffer));
                        MSB_extra_mem_3(pstart, sub_size, begin + (pstart - buffer));
                    }
                }}
        }

        void MSB_extra_mem_3_7to3bits(Element *begin, ElementCounter size, Element *buffer) {
            const FE MAX_HIGH_DIGIT_COUNT = MAX_DIGIT_COUNT >> 3;
            const FE MASK = 0x1F;
            FE count[MAX_HIGH_DIGIT_COUNT] = { 0 };
            Element *pends[MAX_HIGH_DIGIT_COUNT];
            const FE size_trunc_8 = size & (~0 << 3);
            //const FE shift = 24;
            //const FE num_digit = shift >> 3;
            const FE num_digit = 2;
            /*Counting amounts and getting maximum*/ {
                Element *arr_ptr = begin;
                Digit *dp = (Digit*)(begin)+num_digit;
                Digit *dend = (Digit*)(begin + size_trunc_8);
                for (; dp < dend;) {
                    ++count[(*dp >> 2) & MASK]; dp += ELEMENT_SIZE;
                    ++count[(*dp >> 2) & MASK]; dp += ELEMENT_SIZE;
                    ++count[(*dp >> 2) & MASK]; dp += ELEMENT_SIZE;
                    ++count[(*dp >> 2) & MASK]; dp += ELEMENT_SIZE;
                    ++count[(*dp >> 2) & MASK]; dp += ELEMENT_SIZE;
                    ++count[(*dp >> 2) & MASK]; dp += ELEMENT_SIZE;
                    ++count[(*dp >> 2) & MASK]; dp += ELEMENT_SIZE;
                    ++count[(*dp >> 2) & MASK]; dp += ELEMENT_SIZE;
                }
                for (dp = (Digit*)(begin + size_trunc_8) + num_digit, dend = (Digit*)(begin + size); dp < dend;) { // todo remove dp definition! it already has this value
                    ++count[(*dp >> 2) & MASK]; dp += ELEMENT_SIZE;
                }}
            /*Calculating places*/ {
                Element *q = buffer;
                for (FE i = 0; i < MAX_HIGH_DIGIT_COUNT;) {
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
                    *pends[(*dp >> 2) & MASK]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[(*dp >> 2) & MASK]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[(*dp >> 2) & MASK]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[(*dp >> 2) & MASK]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[(*dp >> 2) & MASK]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[(*dp >> 2) & MASK]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[(*dp >> 2) & MASK]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[(*dp >> 2) & MASK]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                }
                for (arr_ptr = begin + size_trunc_8, arr_end = begin + size; arr_ptr < arr_end;) {
                    *pends[(*dp >> 2) & MASK]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                }}
            /*for each digit call subsort*/ {
                for (FE i = 0; i < MAX_HIGH_DIGIT_COUNT; ++i) {
                    ElementCounter sub_size = count[i];
                    if (sub_size != 0) {// skipping empty buckets
                        Element *pend = pends[i];
                        Element *pstart = pend - sub_size;
                        //LSB3(pstart, sub_size, begin + (pstart - buffer));
                        //MSB_inplace_3(pstart, sub_size, begin + (pstart - buffer));
                        LSB2(pstart, sub_size, begin + (pstart - buffer));
                        //LSB2_9bits(pstart, sub_size, begin + (pstart - buffer));
                    }
                }}
        }

        void MSB_extra_mem_3_7bits(Element *begin, ElementCounter size, Element *buffer) {
            const FE MAX_HIGH_DIGIT_COUNT = MAX_DIGIT_COUNT >> 1;
            const FE MASK = 0x7F;
            FE count[MAX_HIGH_DIGIT_COUNT] = { 0 };
            Element *pends[MAX_HIGH_DIGIT_COUNT];
            const FE size_trunc_8 = size & (~0 << 3);
            //const FE shift = 24;
            //const FE num_digit = shift >> 3;
            const FE num_digit = 2;
            /*Counting amounts and getting maximum*/ {
                Element *arr_ptr = begin;
                Digit *dp = (Digit*)(begin)+num_digit;
                Digit *dend = (Digit*)(begin + size_trunc_8);
                for (; dp < dend;) {
                    ++count[(*dp & MASK)]; dp += ELEMENT_SIZE;
                    ++count[(*dp & MASK)]; dp += ELEMENT_SIZE;
                    ++count[(*dp & MASK)]; dp += ELEMENT_SIZE;
                    ++count[(*dp & MASK)]; dp += ELEMENT_SIZE;
                    ++count[(*dp & MASK)]; dp += ELEMENT_SIZE;
                    ++count[(*dp & MASK)]; dp += ELEMENT_SIZE;
                    ++count[(*dp & MASK)]; dp += ELEMENT_SIZE;
                    ++count[(*dp & MASK)]; dp += ELEMENT_SIZE;
                }
                for (dp = (Digit*)(begin + size_trunc_8) + num_digit, dend = (Digit*)(begin + size); dp < dend;) { // todo remove dp definition! it already has this value
                    ++count[(*dp & MASK)]; dp += ELEMENT_SIZE;
                }}
            /*Calculating places*/ {
                Element *q = buffer;
                for (FE i = 0; i < MAX_HIGH_DIGIT_COUNT;) {
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
                    *pends[(*dp & MASK)]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[(*dp & MASK)]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[(*dp & MASK)]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[(*dp & MASK)]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[(*dp & MASK)]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[(*dp & MASK)]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[(*dp & MASK)]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                    *pends[(*dp & MASK)]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                }
                for (arr_ptr = begin + size_trunc_8, arr_end = begin + size; arr_ptr < arr_end;) {
                    *pends[(*dp & MASK)]++ = *arr_ptr++; dp += ELEMENT_SIZE;
                }}
            /*for each digit call subsort*/ {
                for (FE i = 0; i < MAX_HIGH_DIGIT_COUNT; ++i) {
                    ElementCounter sub_size = count[i];
                    if (sub_size != 0) {// skipping empty buckets
                        Element *pend = pends[i];
                        Element *pstart = pend - sub_size;
                        //LSB3(pstart, sub_size, begin + (pstart - buffer));
                        //MSB_inplace_3(pstart, sub_size, begin + (pstart - buffer));
                        LSB2(pstart, sub_size, begin + (pstart - buffer));
                        //LSB2_9bits(pstart, sub_size, begin + (pstart - buffer));
                    }
                }}
        }

        void MSB_extra_mem_4_7bits(Element *begin, ElementCounter size, Element *buffer) {
            const FE MAX_HIGH_DIGIT_COUNT = MAX_DIGIT_COUNT >> 1;
            const FE MASK = 0x7F;
            FE count[MAX_HIGH_DIGIT_COUNT] = { 0 };
            //FE y = (value >> shift) & LOWER_DIGIT_MASK;
            Element *pends[MAX_HIGH_DIGIT_COUNT];
            const FE size_trunc_8 = size & (~0 << 3);
            //const FE shift = 24;
            //const FE num_digit = shift >> 3;
            const FE num_digit = 1;
            FE max = *begin;
            /*Counting amounts and getting maximum*/ {
                Element *arr_ptr = begin;
                Digit2Byte *dp = (Digit2Byte*)(begin)+num_digit;
                Digit2Byte *dend = (Digit2Byte*)(begin + size_trunc_8);
                for (; dp < dend;) {
                    ++count[((*dp >> 7) & MASK)]; dp += DIGIT_2BYTE_STEP; max = *arr_ptr > max ? *arr_ptr : max; ++arr_ptr;
                    ++count[((*dp >> 7) & MASK)]; dp += DIGIT_2BYTE_STEP; max = *arr_ptr > max ? *arr_ptr : max; ++arr_ptr;
                    ++count[((*dp >> 7) & MASK)]; dp += DIGIT_2BYTE_STEP; max = *arr_ptr > max ? *arr_ptr : max; ++arr_ptr;
                    ++count[((*dp >> 7) & MASK)]; dp += DIGIT_2BYTE_STEP; max = *arr_ptr > max ? *arr_ptr : max; ++arr_ptr;
                    ++count[((*dp >> 7) & MASK)]; dp += DIGIT_2BYTE_STEP; max = *arr_ptr > max ? *arr_ptr : max; ++arr_ptr;
                    ++count[((*dp >> 7) & MASK)]; dp += DIGIT_2BYTE_STEP; max = *arr_ptr > max ? *arr_ptr : max; ++arr_ptr;
                    ++count[((*dp >> 7) & MASK)]; dp += DIGIT_2BYTE_STEP; max = *arr_ptr > max ? *arr_ptr : max; ++arr_ptr;
                    ++count[((*dp >> 7) & MASK)]; dp += DIGIT_2BYTE_STEP; max = *arr_ptr > max ? *arr_ptr : max; ++arr_ptr;
                }
                for (dp = (Digit2Byte*)(begin + size_trunc_8) + num_digit, dend = (Digit2Byte*)(begin + size); dp < dend;) { // todo remove dp definition! it already has this value
                    ++count[((*dp >> 7) & MASK)]; dp += DIGIT_2BYTE_STEP; max = *arr_ptr > max ? *arr_ptr : max; ++arr_ptr;
                }}
            /*Checking if we need to skip steps*/ {
                if (max < 1 << 16) {
                    LSB2(begin, size, buffer);
                    return;
                }
                else if (max < 1 << 23) {
                    MSB_extra_mem_3_7bits(begin, size, buffer);
                    memcpy(begin, buffer, size * 4);
                    return;
                }
                else if (max < 1 << 24) {
                    MSB_extra_mem_3(begin, size, buffer);
                    memcpy(begin, buffer, size * 4);
                    return;
                }
            }
            /*Calculating places*/ {
                Element *q = buffer;
                for (FE i = 0; i < MAX_HIGH_DIGIT_COUNT;) {
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
                Digit2Byte *dp = (Digit2Byte*)(begin)+num_digit;
                for (; arr_ptr < arr_end;) {
                    *pends[((*dp >> 7) & MASK)]++ = *arr_ptr++; dp += DIGIT_2BYTE_STEP;
                    *pends[((*dp >> 7) & MASK)]++ = *arr_ptr++; dp += DIGIT_2BYTE_STEP;
                    *pends[((*dp >> 7) & MASK)]++ = *arr_ptr++; dp += DIGIT_2BYTE_STEP;
                    *pends[((*dp >> 7) & MASK)]++ = *arr_ptr++; dp += DIGIT_2BYTE_STEP;
                    *pends[((*dp >> 7) & MASK)]++ = *arr_ptr++; dp += DIGIT_2BYTE_STEP;
                    *pends[((*dp >> 7) & MASK)]++ = *arr_ptr++; dp += DIGIT_2BYTE_STEP;
                    *pends[((*dp >> 7) & MASK)]++ = *arr_ptr++; dp += DIGIT_2BYTE_STEP;
                    *pends[((*dp >> 7) & MASK)]++ = *arr_ptr++; dp += DIGIT_2BYTE_STEP;
                }
                for (arr_ptr = begin + size_trunc_8, arr_end = begin + size; arr_ptr < arr_end;) {
                    *pends[((*dp >> 7) & MASK)]++ = *arr_ptr++; dp += DIGIT_2BYTE_STEP;
                }}
            /*for each digit call subsort*/ {
                for (FE i = 0; i < MAX_HIGH_DIGIT_COUNT; ++i) {
                    ElementCounter sub_size = count[i];
                    if (sub_size != 0) {// skipping empty buckets
                        Element *pend = pends[i];
                        Element *pstart = pend - sub_size;
                        //LSB3(pstart, sub_size, begin + (pstart - buffer));
                        //MSB_inplace_3(pstart, sub_size, begin + (pstart - buffer));
                        MSB_extra_mem_3(pstart, sub_size, begin + (pstart - buffer));
                        // MSB_extra_mem_3_7bits(pstart, sub_size, begin + (pstart - buffer));
                        //MSB_extra_mem_3_7to3bits(pstart, sub_size, begin + (pstart - buffer));
                    }
                }}
        }

        void MSB_extra_mem_4_14bits(Element *begin, ElementCounter size, Element *buffer) {
            const FE MAX_HIGH_DIGIT_COUNT = MAX_DIGIT_COUNT << 6;
            const FE MASK = 0x3FFF;
            FE count[MAX_HIGH_DIGIT_COUNT] = { 0 };
            //FE y = (value >> shift) & LOWER_DIGIT_MASK;
            Element *pends[MAX_HIGH_DIGIT_COUNT];
            const FE size_trunc_8 = size & (~0 << 3);
            //const FE shift = 24;
            //const FE num_digit = shift >> 3;
            const FE num_digit = 1;
            FE max = *begin;
            /*Counting amounts and getting maximum*/ {
                Element *arr_ptr = begin;
                Digit2Byte *dp = (Digit2Byte*)(begin)+num_digit;
                Digit2Byte *dend = (Digit2Byte*)(begin + size_trunc_8);
                for (; dp < dend;) {
                    ++count[*dp & MASK]; dp += DIGIT_2BYTE_STEP; max = *arr_ptr > max ? *arr_ptr : max; ++arr_ptr;
                    ++count[*dp & MASK]; dp += DIGIT_2BYTE_STEP; max = *arr_ptr > max ? *arr_ptr : max; ++arr_ptr;
                    ++count[*dp & MASK]; dp += DIGIT_2BYTE_STEP; max = *arr_ptr > max ? *arr_ptr : max; ++arr_ptr;
                    ++count[*dp & MASK]; dp += DIGIT_2BYTE_STEP; max = *arr_ptr > max ? *arr_ptr : max; ++arr_ptr;
                    ++count[*dp & MASK]; dp += DIGIT_2BYTE_STEP; max = *arr_ptr > max ? *arr_ptr : max; ++arr_ptr;
                    ++count[*dp & MASK]; dp += DIGIT_2BYTE_STEP; max = *arr_ptr > max ? *arr_ptr : max; ++arr_ptr;
                    ++count[*dp & MASK]; dp += DIGIT_2BYTE_STEP; max = *arr_ptr > max ? *arr_ptr : max; ++arr_ptr;
                    ++count[*dp & MASK]; dp += DIGIT_2BYTE_STEP; max = *arr_ptr > max ? *arr_ptr : max; ++arr_ptr;
                }
                for (dp = (Digit2Byte*)(begin + size_trunc_8) + num_digit, dend = (Digit2Byte*)(begin + size); dp < dend;) { // todo remove dp definition! it already has this value
                    ++count[*dp & MASK]; dp += DIGIT_2BYTE_STEP; max = *arr_ptr > max ? *arr_ptr : max; ++arr_ptr;
                }}
            /*Checking if we need to skip steps*/ {
                if (max < 1 << 16) {
                    LSB2(begin, size, buffer);
                    return;
                }
                else if (max < 1 << 23) {
                    MSB_extra_mem_3_7bits(begin, size, buffer);
                    memcpy(begin, buffer, size * 4);
                    return;
                }
                else if (max < 1 << 24) {
                    MSB_extra_mem_3(begin, size, buffer);
                    memcpy(begin, buffer, size * 4);
                    return;
                }
            }
            /*Calculating places*/ {
                Element *q = buffer;
                for (FE i = 0; i < MAX_HIGH_DIGIT_COUNT;) {
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
                Digit2Byte *dp = (Digit2Byte*)(begin)+num_digit;
                for (; arr_ptr < arr_end;) {
                    *pends[*dp & MASK]++ = *arr_ptr++; dp += DIGIT_2BYTE_STEP;
                    *pends[*dp & MASK]++ = *arr_ptr++; dp += DIGIT_2BYTE_STEP;
                    *pends[*dp & MASK]++ = *arr_ptr++; dp += DIGIT_2BYTE_STEP;
                    *pends[*dp & MASK]++ = *arr_ptr++; dp += DIGIT_2BYTE_STEP;
                    *pends[*dp & MASK]++ = *arr_ptr++; dp += DIGIT_2BYTE_STEP;
                    *pends[*dp & MASK]++ = *arr_ptr++; dp += DIGIT_2BYTE_STEP;
                    *pends[*dp & MASK]++ = *arr_ptr++; dp += DIGIT_2BYTE_STEP;
                    *pends[*dp & MASK]++ = *arr_ptr++; dp += DIGIT_2BYTE_STEP;
                }
                for (arr_ptr = begin + size_trunc_8, arr_end = begin + size; arr_ptr < arr_end;) {
                    *pends[*dp & MASK]++ = *arr_ptr++; dp += DIGIT_2BYTE_STEP;
                }}
            /*for each digit call subsort*/ {
                for (FE i = 0; i < MAX_HIGH_DIGIT_COUNT; ++i) {
                    ElementCounter sub_size = count[i];
                    if (sub_size != 0) {// skipping empty buckets
                        Element *pend = pends[i];
                        Element *pstart = pend - sub_size;
                        //LSB3(pstart, sub_size, begin + (pstart - buffer));
                        //MSB_inplace_3(pstart, sub_size, begin + (pstart - buffer));
                        //MSB_extra_mem_3(pstart, sub_size, begin + (pstart - buffer));
                        //MSB_extra_mem_3_7bits(pstart, sub_size, begin + (pstart - buffer));
                        //MSB_extra_mem_3_7to3bits(pstart, sub_size, begin + (pstart - buffer));
                        LSB2(pstart, sub_size, begin + (pstart - buffer));
                    }
                }}
            memcpy(begin, buffer, size * 4);
        }

        void StepTwo(Element *begin, ElementCounter size, Element *buffer) {
            SORT_DIGIT(begin, size, buffer, D3, D3shift)
            for (FE i = 0; i < DIGIT_AMOUNT; ++i) {/*for each digit call subsort*/
                ElementCounter sub_size = count[i];
                if (sub_size != 0) {// skipping empty buckets
                    Element *pend = pends[i];
                    Element *pstart = pend - sub_size;
                    {SORT_DIGIT(pstart, sub_size, begin + (pstart - buffer), D1, D1shift)}
                    {SORT_DIGIT(begin + (pstart - buffer), sub_size, pstart, D2, D2shift)}
                    // LSB_1(pstart, sub_size, begin + (pstart - buffer));
                    // LSB2(pstart, sub_size, begin + (pstart - buffer));
                    // MSB2(pstart, sub_size, begin + (pstart - buffer));
                }
            }
        }

        void StepOne(Element *begin, ElementCounter size, Element *buffer) {
            SORT_DIGIT(begin, size, buffer, D4, D4shift)
            FE max = count[1];
            for (FE i = 2; i < DIGIT_AMOUNT; ++i) {
                max = count[i] > max ? count[i] : max;
            }
            for (FE i = 0; i < DIGIT_AMOUNT; ++i) {/*for each digit call subsort*/
                ElementCounter sub_size = count[i];
                if (sub_size != 0) {// skipping empty buckets
                    Element *pend = pends[i];
                    Element *pstart = pend - sub_size;
                    MSB3(pstart, sub_size, begin + (pstart - buffer));
                    // {SORT_DIGIT(pstart, sub_size, begin + (pstart - buffer), D1, D1shift)}
                    // {SORT_DIGIT(begin + (pstart - buffer), sub_size, pstart, D2, D2shift)}
                    // {SORT_DIGIT(pstart, sub_size, begin + (pstart - buffer), D3, D3shift)}
                }
            }
            // memcpy(begin, buffer, size * sizeof(Element));
        }

        void k9_sort(Element *begin, ElementCounter size) {
            Element *buffer = (Element *)malloc(sizeof(Element) * size);
            // StepOne(begin, size, buffer);
            // MSB4(begin, size, buffer);
            // memcpy(begin, buffer, size * sizeof(Element));
            // MSB34(begin, size, buffer);
            MSB_extra_mem_4(begin, size, buffer); // currently da best
            // MSB_extra_mem_4_7bits(begin, size, buffer);
            //MSB_extra_mem_4_14bits(begin, size, buffer);
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
// void Sort(unsigned int *arr, unsigned int size) {
//     Sort((made::Element*)arr, (made::ElementCounter)size);
// }
#endif // !MADE_CUSTOM_SORT_H_

#endif // !K9_AVX_H_