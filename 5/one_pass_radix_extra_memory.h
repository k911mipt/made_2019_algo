#ifndef ONE_PASS_RADIX_EXTRA_MEMORY_H_
#define ONE_PASS_RADIX_EXTRA_MEMORY_H_

#include <cstdint>
namespace one_pass_radix_extra_memory {
    typedef uint8_t iterator;
    //typedef uint16_t iterator;
    const uint8_t ITERATOR_SIZE = sizeof(iterator);
    const uint32_t COUNTER_SIZE = 1 << (ITERATOR_SIZE * 8);

    template<typename UINT>
    void Radix(const uint8_t ByteIndex, const UINT* Source, UINT* Dest, const unsigned SourceSize)
    {
        const uint8_t step = sizeof(UINT) / ITERATOR_SIZE;
        const uint8_t n_counters = 4;
        unsigned ByteCounters[n_counters][COUNTER_SIZE];
        // set the counter array to zero
        std::memset(ByteCounters, 0, sizeof(ByteCounters));

        // start at the propper byte index
        const uint8_t *curBytes[4];
        for (int j = 0; j < n_counters; ++j) {
            curBytes[j] = reinterpret_cast<const uint8_t*>(Source) + ByteIndex + j;
        }

        // loop over all of the bytes in the sorce, incrementing the index
        for (unsigned i = 0; i < SourceSize; ++i) {
            for (int j = 0; j < n_counters; ++j) {
                ++ByteCounters[j][*curBytes[j]];
                curBytes[j] += step;
            }
        }

        // use the byte counts that I have to make indexes
        unsigned indexStarts[n_counters];// this where the specific byte starts in the dest array
        std::memset(indexStarts, 0, sizeof(indexStarts));
        unsigned tempCounts[n_counters];
        for (unsigned i = 0; i < COUNTER_SIZE; ++i)
        {
            for (int j = 0; j < n_counters; ++j) {
                tempCounts[j] = ByteCounters[j][i];
                ByteCounters[j][i] = indexStarts[j];
                indexStarts[j] += tempCounts[j]; // this will effectively set up the gaps needed in the
                // final array to fill with bytes
            }
        }

        // now that you have indexes, copy the values over
        for (int j = 0; j < n_counters; ++j) {
            curBytes[j] = reinterpret_cast<const uint8_t*>(Source) + ByteIndex + j;
        }
        const UINT* SourcePtr = Source;
        for (unsigned i = 0; i < SourceSize; ++i, ++SourcePtr)
        {
            for (int j = 0; j < 1; ++j) {
                unsigned *countPtr = ByteCounters[j] + *curBytes[j];
                Dest[*countPtr] = *SourcePtr;
                ++(*countPtr);
                curBytes[j] += step;
            }
        }
    }

    template<typename UINT>
    void RadixSort(UINT *data, const unsigned size)
    {
        UINT *tempData = new UINT[size];
        if (sizeof(UINT) == 1)
        {
            Radix(0, data, tempData, size);
            memcpy(data, tempData, size);
        }
        else
        {
            for (unsigned i = 0; i < sizeof(UINT); i += 2)
            {
                Radix(i + 0, data, tempData, size);
                Radix(i + 1, tempData, data, size);
            }
        }
        delete[] tempData;
    }

    template<typename UINT>
    void make_random(UINT *data, unsigned N)
    {
        for (; N > 0; --N, ++data)
        {
            *data = 0;
            for (unsigned i = 0; i < sizeof(UINT); ++i)
                *data |= (rand() & 0xFF) << (i * 8);
        }
    }

    template<typename UINT>
    void check_order(const UINT *data, unsigned N)
    {
        for (--N; N > 0; --N, ++data)
            assert(data[0] <= data[1]);
    }
}
#endif ONE_PASS_RADIX_EXTRA_MEMORY_H_