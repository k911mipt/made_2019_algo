#ifndef CUSTOM_RADIX_EXTRA_MEMORY_H_
#define CUSTOM_RADIX_EXTRA_MEMORY_H_

#include <cstdint>
#include <cassert>
#include "xmmintrin.h"
#include <emmintrin.h>
namespace custom_radix_extra_memory {
    typedef uint8_t iterator;
    //typedef uint16_t iterator;
    const uint8_t ITERATOR_SIZE = sizeof(iterator);
    const uint32_t COUNTER_SIZE = 1 << (ITERATOR_SIZE * 8);

    template<typename UINT>
    void Radix(const uint8_t ByteIndex, const UINT* Source, UINT* Dest, const unsigned SourceSize)
    {
        const uint8_t step = sizeof(UINT) / ITERATOR_SIZE;
        const uint8_t ten_step = step * 10; const uint8_t ts = ten_step;
        unsigned ByteCounter[COUNTER_SIZE];
        // set the counter array to zero
        std::memset(ByteCounter, 0, sizeof(ByteCounter));

        // start at the propper byte index
        const iterator *curByte = reinterpret_cast<const iterator*>(Source) + ByteIndex;
        //// loop over all of the bytes in the sorce, incrementing the index
        //for (unsigned i = 0; i < SourceSize; ++i, curByte += step) {
        //    //assert(*curByte <= COUNTER_SIZE);
        //    ++ByteCounter[*curByte];
        //}

        //_MM_HINT_T0 = 3, L1
        //_MM_HINT_T1 = 2, L2
        //_MM_HINT_T2 = 1, L3
        //_MM_HINT_NTA = 0, non neccessary http://rus-linux.net/lib.php?name=/MyLDP/hard/memory/memory-6-7.html
        // VECTORIZING
        //const iterator *curByte = reinterpret_cast<const iterator*>(Source) + ByteIndex;
        const uint8_t steps[10] = { ts, ts, ts, ts, ts, ts, ts, ts, ts, ts };
        //__m128i *curByteSSE, *stepSSE;
        //__m128i s;
        //curByteSSE = (__m128i *)curByte;
        //stepSSE = (__m128i *)steps;
        //__m128i *curByteSSE = reinterpret_cast<__m128*>(curByte);
        //__m128i *curByteSSE = (__m128i*) curByte;
        //__m128i *stepSSE = (__m128i*) steps;

        __m128 *curByteSSE = (__m128*) curByte;
        __m128 *stepSSE = (__m128*) steps;


        //s = mm_set_ps1(0);
        _mm_prefetch((char *)&curByteSSE[10], _MM_HINT_NTA);
        _mm_prefetch((char *)&stepSSE[10], _MM_HINT_T0);
        /*curByteSSE = _mm_add_ps(curByteSSE, stepSSE);*/
        //for (unsigned i = 0; i < SourceSize / 10; ++i) {
        //    *curByteSSE= _mm_add_ps(curByteSSE, stepSSE);
        //}

        // start at the propper byte index
        const iterator *curByte0 = curByte;
        const iterator *curByte1 = curByte0 + step;
        const iterator *curByte2 = curByte1 + step;
        const iterator *curByte3 = curByte2 + step;
        const iterator *curByte4 = curByte3 + step;
        const iterator *curByte5 = curByte4 + step;
        const iterator *curByte6 = curByte5 + step;
        const iterator *curByte7 = curByte6 + step;
        const iterator *curByte8 = curByte7 + step;
        const iterator *curByte9 = curByte8 + step;
        const iterator *curByte10 = curByte9 + step;
        //*curByteSSE = _mm_add_ps(*curByteSSE, *stepSSE);
        curByteSSE++;
        // ITERATOR_SIZE = 8 bit;
        //alignas(ITERATOR_SIZE) const iterator* curBytes[] = {
        //    curByte0,
        //    curByte1,
        //    curByte2,
        //    curByte3,
        //    curByte4,
        //    curByte5,
        //    curByte6,
        //    curByte7,
        //    curByte8,
        //    curByte9
        //};
        //alignas(ITERATOR_SIZE) const iterator curSteps[] = {
        //    ten_step,
        //    ten_step,
        //    ten_step,
        //    ten_step,
        //    ten_step,
        //    ten_step,
        //    ten_step,
        //    ten_step,
        //    ten_step,
        //    ten_step
        //};

        // loop over all of the bytes in the sorce, incrementing the index
        for (unsigned i = 0; i < SourceSize / 10; ++i) {
            ++ByteCounter[*curByte0]; curByte0 += ten_step;
            ++ByteCounter[*curByte1]; curByte1 += ten_step;
            ++ByteCounter[*curByte2]; curByte2 += ten_step;
            ++ByteCounter[*curByte3]; curByte3 += ten_step;
            ++ByteCounter[*curByte4]; curByte4 += ten_step;
            ++ByteCounter[*curByte5]; curByte5 += ten_step;
            ++ByteCounter[*curByte6]; curByte6 += ten_step;
            ++ByteCounter[*curByte7]; curByte7 += ten_step;
            ++ByteCounter[*curByte8]; curByte8 += ten_step;
            ++ByteCounter[*curByte9]; curByte9 += ten_step;
        }
        // !VECTORIZING

        // use the byte counts that I have to make indexes
        unsigned indexStart = 0; // this where the specific byte starts in the dest array
        for (unsigned i = 0; i < COUNTER_SIZE; ++i)
        {
            unsigned tempCount = ByteCounter[i];
            ByteCounter[i] = indexStart;
            indexStart += tempCount; // this will effectively set up the gaps needed in the
            // final array to fill with bytes
        }

        // now that you have indexes, copy the values over
        curByte = reinterpret_cast<const iterator*>(Source) + ByteIndex;
        for (unsigned i = 0; i < SourceSize; ++i, ++Source, curByte += step)
        {
            unsigned *countPtr = ByteCounter + *curByte;
            Dest[*countPtr] = *Source;
            ++(*countPtr);
        }
    }

    template<typename UINT>
    void RadixSort(UINT *data, const unsigned size)
    {
        const uint8_t step = sizeof(UINT) / ITERATOR_SIZE;
        UINT *tempData = new UINT[size];
        if (step == 1)
        {
            Radix(0, data, tempData, size);
            memcpy(data, tempData, size);
        }
        else
        {
            for (unsigned i = 0; i < step; i += 2)
            {
                Radix(i + 0, data, tempData, size);
                Radix(i + 1, tempData, data, size);
            }
        }
        delete[] tempData;
    }
}
#endif CUSTOM_RADIX_EXTRA_MEMORY_H_