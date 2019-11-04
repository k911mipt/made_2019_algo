#ifndef K9_AVX_H_
#define K9_AVX_H_

//#include "common.h"
#include "sort.h"
#include "stdint.h"
#include <algorithm>
#include <cstring>
#include <cassert>

namespace made {
    const uint32_t L1_SIZE = 64 * 1024; // 64 KB // 2^16 = 16384 integers
    const uint32_t L2_SIZE = 2 * 1024 * 1024; // 2 MB 2^19 = 524288 integers
    const uint32_t L3_SIZE = 20 * 1024 * 1024; // 20 MB = 20 * 2^20 = 20971520 integers

    // typedef uint8_t Digit;
    typedef uint_fast8_t Digit;
    //typedef uint16_t Digit;
    // typedef uint32_t Element; // 0..10^9 value range
    // typedef uint32_t ElementCounter; // 0..10^7 array size
    typedef unsigned int Element; // 0..10^9 value range
    typedef unsigned int ElementCounter; // 0..10^7 array size
    typedef uint_fast32_t FE;
    const uint_fast8_t ELEMENT_SIZE = sizeof(Element);
    const uint_fast8_t DIGIT_SIZE = sizeof(Digit);
    const uint_fast32_t MAX_DIGIT_COUNT = 1 << (DIGIT_SIZE * 8);
    const uint_fast32_t LOWER_DIGIT_MASK = MAX_DIGIT_COUNT - 1; // digit = 1 byte: 0xFF, digit = 2 byte: 0xFFFF, etc.;
}

namespace made {
    namespace avx {
        //void LSB(Element *begin, ElementCounter size, Element *buffer, Element maxshift) {
        //    Element *end = begin + size;
        //    //Element *end1 = buffer + size;
        //    FE count[MAX_DIGIT_COUNT];
        //    Element *pends[MAX_DIGIT_COUNT];
        //    Digit* dp;
        //    Digit* dend;
        //    for (Element shift = 0; shift <= maxshift; shift += 8)
        //    {
        //        std::memset(count, 0, sizeof(count));
        //        dp = (Digit*)(begin)+(shift >> 3); // digit pointer
        //        for (dend = (Digit*)(begin + (size & (~0 << 3))); dp < dend;) {
        //            ++count[*dp]; dp += ELEMENT_SIZE;
        //            ++count[*dp]; dp += ELEMENT_SIZE;
        //            ++count[*dp]; dp += ELEMENT_SIZE;
        //            ++count[*dp]; dp += ELEMENT_SIZE;
        //            ++count[*dp]; dp += ELEMENT_SIZE;
        //            ++count[*dp]; dp += ELEMENT_SIZE;
        //            ++count[*dp]; dp += ELEMENT_SIZE;
        //            ++count[*dp]; dp += ELEMENT_SIZE;
        //        }
        //        // uint_fast8_t digit = (shift >> 3);
        //        // __v32qu *vp = (__v32qu *)begin; // yeah baby!
        //        // __v32qu *vend = (__v32qu *)(begin + (size & (~0 << 3))); // yeah baby!
        //        // Element *ap = begin;
        //        // int i = 0;
        //        // for (; vp < vend; ++vp, ap+=8) { // array pointer
        //        //     _mm_prefetch((vp+1), _MM_HINT_T0);
        //        //     // _mm_prefetch(vp, _MM_HINT_NTA);
        //        //     ++count[(*vp)[digit + 0]];
        //        //     ++count[(*vp)[digit + 4]];
        //        //     ++count[(*vp)[digit + 8]];
        //        //     ++count[(*vp)[digit + 12]];
        //        //     ++count[(*vp)[digit + 16]];
        //        //     ++count[(*vp)[digit + 20]];
        //        //     ++count[(*vp)[digit + 24]];
        //        //     ++count[(*vp)[digit + 28]];
        //        // }
        //        // std::cout << "finish unrolling" << std::endl;
        //        for (dp = (Digit*)(begin + (size & (~0 << 3))) + (shift >> 3), dend = (Digit*)(begin + size); dp < dend;) {
        //            ++count[*dp]; dp += ELEMENT_SIZE;
        //        }
        //        Element *q = buffer;
        //        for (ElementCounter i = 0; i < MAX_DIGIT_COUNT; q += count[i++]) {
        //            pends[i] = q;
        //            // if (q > buffer_end)
        //            //     assert(q <= buffer_end);
        //        }
        //        // std::cout << "Starting sorting" << std::endl;
        //        Element* arr_ptr = begin;
        //        Element *arr_end = begin + (size & (~0 << 3));
        //        dp = (Digit*)(begin)+(shift >> 3);
        //        for (; arr_ptr < arr_end;) {
        //            *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
        //            *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
        //            *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
        //            *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
        //            *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
        //            *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
        //            *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
        //            *pends[*dp]++ = *arr_ptr++; dp += ELEMENT_SIZE;
        //        }
        //        for (arr_ptr = begin + (size & (~0 << 3)), arr_end = begin + size; arr_ptr < arr_end;) {
        //            *pends[*dp]++ = *arr_ptr; ++arr_ptr; dp += ELEMENT_SIZE;
        //        }
        //        std::swap(begin, buffer);
        //    }
        //}

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
                for (dp0 = (Digit*)(begin + size_trunc_8), dend = (Digit*)(begin + size); dp0 < dend;) {
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
            FE count[MAX_DIGIT_COUNT] = { 0 };
            Element *pends[MAX_DIGIT_COUNT];
            const FE size_trunc_8 = size & (~0 << 3);
            const FE shift = 16;
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
                        LSB2(pstart, sub_size, begin + (pstart - buffer));
                        //MSB_extra_mem_2(pstart, sub_size, begin + (pstart - buffer));
                        //MSB_inplace_3(pstart, sub_size, begin + (pstart - buffer));
                    }
                }}
        }

        void MSB_extra_mem_4(Element *begin, ElementCounter size, Element *buffer) {
            const FE MAX_HIGH_DIGIT_COUNT = MAX_DIGIT_COUNT;
            FE count[MAX_HIGH_DIGIT_COUNT] = { 0 };
            Element *pends[MAX_HIGH_DIGIT_COUNT];
            const FE size_trunc_8 = size & (~0 << 3);
            const FE shift = 24;
            const FE num_digit = shift >> 3;
            FE max = *begin;
            /*Counting amounts and getting maximum*/ {
                Element *arr_ptr = begin;
                Digit *dp = (Digit*)(begin)+num_digit;
                Digit *dend = (Digit*)(begin + size_trunc_8);
                for (; dp < dend;) {
                    ++count[*dp]; dp += ELEMENT_SIZE; max = *arr_ptr > max ? *arr_ptr : max; ++arr_ptr;
                    ++count[*dp]; dp += ELEMENT_SIZE; max = *arr_ptr > max ? *arr_ptr : max; ++arr_ptr;
                    ++count[*dp]; dp += ELEMENT_SIZE; max = *arr_ptr > max ? *arr_ptr : max; ++arr_ptr;
                    ++count[*dp]; dp += ELEMENT_SIZE; max = *arr_ptr > max ? *arr_ptr : max; ++arr_ptr;
                    ++count[*dp]; dp += ELEMENT_SIZE; max = *arr_ptr > max ? *arr_ptr : max; ++arr_ptr;
                    ++count[*dp]; dp += ELEMENT_SIZE; max = *arr_ptr > max ? *arr_ptr : max; ++arr_ptr;
                    ++count[*dp]; dp += ELEMENT_SIZE; max = *arr_ptr > max ? *arr_ptr : max; ++arr_ptr;
                    ++count[*dp]; dp += ELEMENT_SIZE; max = *arr_ptr > max ? *arr_ptr : max; ++arr_ptr;
                }
                for (dp = (Digit*)(begin + size_trunc_8) + num_digit, dend = (Digit*)(begin + size); dp < dend;) {
                    ++count[*dp]; dp += ELEMENT_SIZE; max = *arr_ptr > max ? *arr_ptr : max; ++arr_ptr;
                }}
            /*Checking if we need to skip steps*/ {
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
                for (FE i = 0; i < MAX_HIGH_DIGIT_COUNT; ++i) {
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

        void k9_sort(Element *begin, ElementCounter size) {
            Element *buffer = (Element *)malloc(sizeof(Element) * size);
            MSB_extra_mem_4(begin, size, buffer);
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