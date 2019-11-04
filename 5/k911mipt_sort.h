#ifndef K9_SORT_H_
#define K9_SORT_H_

#include "common.h"
#include <cassert>
#define UNROL_LOOP 1
namespace made {
    void LSB(Element *begin, ElementCounter size, Element *buffer, Element maxshift) {
        Element *end = begin + size;
        Element *end1 = buffer + size;
        ElementCounter count[MAX_DIGIT_COUNT];
        Element *pends[MAX_DIGIT_COUNT];
        Digit *dp;
        Digit *dend;
        for (Element shift = 0; shift <= maxshift; shift += 8) {
            // std::cout << "Starting counting" << std::endl;
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
            for (dp = (Digit*)(begin + (size & (~0 << 3))) + (shift >> 3), dend = (uint8_t*)(begin + size); dp < dend;) {
                ++count[*dp]; dp += ELEMENT_SIZE;
            }
            Element *q = buffer;
            for (int i = 0; i < MAX_DIGIT_COUNT; q += count[i++]) {
                pends[i] = q;
            }
            Element* arr_ptr = begin;
            Element *arr_end = begin + (size & (~0 << 3));
            dp = (Digit*)(begin)+(shift >> 3);
            for (; arr_ptr < arr_end;) {
                *pends[*dp]++ = *arr_ptr; ++arr_ptr; dp += ELEMENT_SIZE;
                *pends[*dp]++ = *arr_ptr; ++arr_ptr; dp += ELEMENT_SIZE;
                *pends[*dp]++ = *arr_ptr; ++arr_ptr; dp += ELEMENT_SIZE;
                *pends[*dp]++ = *arr_ptr; ++arr_ptr; dp += ELEMENT_SIZE;
                *pends[*dp]++ = *arr_ptr; ++arr_ptr; dp += ELEMENT_SIZE;
                *pends[*dp]++ = *arr_ptr; ++arr_ptr; dp += ELEMENT_SIZE;
                *pends[*dp]++ = *arr_ptr; ++arr_ptr; dp += ELEMENT_SIZE;
                *pends[*dp]++ = *arr_ptr; ++arr_ptr; dp += ELEMENT_SIZE;
            }
            for (arr_ptr = begin + (size & (~0 << 3)), arr_end = begin + size; arr_ptr < arr_end;) {
                *pends[*dp]++ = *arr_ptr; ++arr_ptr; dp += ELEMENT_SIZE;
            }
            std::swap(begin, buffer);
            std::swap(end, end1);
        }
    }

    void MSB(Element *begin, ElementCounter size, Element *buffer) {
        const ElementCounter MAX_HIGH_DIGIT_COUNT = MAX_DIGIT_COUNT >> 2;
        const Element HIGHER_DIGIT_MASK = MAX_HIGH_DIGIT_COUNT - 1;
        const Element shift = 24;
        ElementCounter count[MAX_HIGH_DIGIT_COUNT] = { 0 };
        Element *end = begin + size;
        Digit* dp;
        Digit* dend;
        dp = (Digit*)(begin)+(shift >> 3);
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
        for (dp = (Digit*)(begin + (size & (~0 << 3))) + (shift >> 3), dend = (uint8_t*)(begin + size); dp < dend;) {
            ++count[*dp]; dp += ELEMENT_SIZE;
        }
        Element *pends[MAX_HIGH_DIGIT_COUNT], *pstarts[MAX_HIGH_DIGIT_COUNT], *q = buffer;
        for (ElementCounter i = 0; i < MAX_HIGH_DIGIT_COUNT; q += count[i++]) {
            pstarts[i] = pends[i] = q;
        }
        Element* arr_ptr = begin;
        Element *arr_end = begin + (size & (~0 << 3));
        dp = (Digit*)(begin)+(shift >> 3);
        for (; arr_ptr < arr_end;) {
            *pends[*dp]++ = *arr_ptr; ++arr_ptr; dp += ELEMENT_SIZE;
            *pends[*dp]++ = *arr_ptr; ++arr_ptr; dp += ELEMENT_SIZE;
            *pends[*dp]++ = *arr_ptr; ++arr_ptr; dp += ELEMENT_SIZE;
            *pends[*dp]++ = *arr_ptr; ++arr_ptr; dp += ELEMENT_SIZE;
            *pends[*dp]++ = *arr_ptr; ++arr_ptr; dp += ELEMENT_SIZE;
            *pends[*dp]++ = *arr_ptr; ++arr_ptr; dp += ELEMENT_SIZE;
            *pends[*dp]++ = *arr_ptr; ++arr_ptr; dp += ELEMENT_SIZE;
            *pends[*dp]++ = *arr_ptr; ++arr_ptr; dp += ELEMENT_SIZE;
        }
        for (arr_ptr = begin + (size & (~0 << 3)), arr_end = begin + size; arr_ptr < arr_end;) {
            // *pends[(*arr_ptr >> shift) & HIGHER_DIGIT_MASK]++ = *arr_ptr; ++arr_ptr;
            *pends[*dp]++ = *arr_ptr; ++arr_ptr; dp += ELEMENT_SIZE;
        }
        for (ElementCounter i = 0; i < MAX_HIGH_DIGIT_COUNT; ++i) {
            Element *pstart = pstarts[i];
            // std::cout << "LSB #" << i << std::endl;
            Element *pend = pends[i];
            if (pend > pstart) // skipping empty buckets
                LSB(pstart, pend - pstart, begin + (pstart - buffer), shift - 8);
        }
    }

    void k9_sort(Element *begin, ElementCounter size)
    {
        //Element *end = begin + size;
        // Element *buffer = new Element[size];
        Element *buffer = (Element *)malloc(sizeof(Element) * size);
        MSB(begin, size, buffer);
        // memcpy(begin, buffer, size * 4);
        free(buffer);
        // delete[] buffer;
    }
}
#endif // !K9_SORT_H_