#ifndef RADIX_EXTRA_MEMORY_H_
#define RADIX_EXTRA_MEMORY_H_

#include <cstdint>
namespace radix_extra_memory {
    template<typename UINT>
    void Radix(const uint8_t ByteIndex, const UINT* Source, UINT* Dest, const unsigned SourceSize)
    {
        unsigned ByteCounter[256];
        // set the counter array to zero
        std::memset(ByteCounter, 0, sizeof(ByteCounter));

        // start at the propper byte index
        const uint8_t *curByte = reinterpret_cast<const uint8_t*>(Source) + ByteIndex;
        // loop over all of the bytes in the sorce, incrementing the index
        for (unsigned i = 0; i < SourceSize; ++i, curByte += sizeof(UINT))
            ++ByteCounter[*curByte];

        // use the byte counts that I have to make indexes
        unsigned indexStart = 0; // this where the specific byte starts in the dest array
        for (unsigned i = 0; i < 256; ++i)
        {
            unsigned tempCount = ByteCounter[i];
            ByteCounter[i] = indexStart;
            indexStart += tempCount; // this will effectively set up the gaps needed in the
            // final array to fill with bytes
        }

        // now that you have indexes, copy the values over
        curByte = reinterpret_cast<const uint8_t*>(Source) + ByteIndex;
        for (unsigned i = 0; i < SourceSize; ++i, ++Source, curByte += sizeof(UINT))
        {
            unsigned *countPtr = ByteCounter + *curByte;
            Dest[*countPtr] = *Source;
            ++(*countPtr);
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
#endif // !RADIX_EXTRA_MEMORY_H_