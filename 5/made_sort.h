#ifndef MADE_SORT_H_
#define MADE_SORT_H_

#include <cstdint>
namespace made {
    typedef uint8_t Digit;
    //typedef uint16_t Digit;
    typedef uint32_t Element; // 0..10^9 value range
    typedef uint32_t ElementCounter; // 0..10^7 array size
    const uint8_t DIGIT_SIZE = sizeof(Digit);
    const uint32_t MAX_DIGIT_COUNT = 1 << (DIGIT_SIZE * 8);

    template<typename Element>
    void LSDSort(Element* arr, const uint32_t size)
    {
        Element *buffer = new Element[size];
        const uint8_t step = sizeof(Element) / DIGIT_SIZE;
        const uint8_t num_digits = step;
        ElementCounter digit_counters[num_digits][MAX_DIGIT_COUNT]; std::memset(digit_counters, 0, sizeof(digit_counters));
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

        // Count start indexes for each digit. Store in digit_counter
        unsigned start_indexes[num_digits];// this where the specific byte starts in the dest array
        std::memset(start_indexes, 0, sizeof(start_indexes));
        ElementCounter temp_counts[num_digits];
        ElementCounter temp;
        for (unsigned i_e = 0; i_e < MAX_DIGIT_COUNT; ++i_e) { // i_element
            for (int j_d = 0; j_d < num_digits; ++j_d) { // j_digit
                temp_counts[j_d] = digit_counters[j_d][i_e];
                digit_start_indexes[j_d][i_e] = start_indexes[j_d];
                start_indexes[j_d] += temp_counts[j_d];
            }
            //temp = digit_counters[0][i_element];
            //digit_start_indexes[0][i_element] = start_indexes[0];
            //start_indexes[0] += temp;

            //temp = digit_counters[1][i_element];
            //digit_start_indexes[1][i_element] = start_indexes[1];
            //start_indexes[1] += temp;

            //temp = digit_counters[2][i_element];
            //digit_start_indexes[2][i_element] = start_indexes[2];
            //start_indexes[2] += temp;

            //temp = digit_counters[3][i_element];
            //digit_start_indexes[3][i_element] = start_indexes[3];
            //start_indexes[3] += temp;
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
}

void Sort(made::Element *arr, made::ElementCounter size)
{
    made::LSDSort(arr, size);
}

#endif MADE_SORT_H_
