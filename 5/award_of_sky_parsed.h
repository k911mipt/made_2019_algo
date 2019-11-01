#ifndef AWARD_OF_SKY_PARSED_H_
#define AWARD_OF_SKY_PARSED_H_
#include "sort.h"
#include <cstring>
#include <cstdint>
#include <cstdlib>
namespace award_parsed
{
//typedef uint8_t Digit;
////typedef uint16_t Digit;
//typedef uint32_t Element; // 0..10^9 value range
//typedef uint32_t ElementCounter; // 0..10^7 array size
//const uint8_t DIGIT_SIZE = sizeof(Digit);
//const uint32_t MAX_DIGIT_COUNT = 1 << (DIGIT_SIZE * 8);

/*
      Integer Radix LSD sort. Stable and out-of-place.
      ---Parameters---

      vector[] - Pointer to the orginal array of integers
      size     - Size of the array to sort

     ---List of optimizations implemented---
      For a list of all optimizations implemented check the github README.md
      over at https://github.com/AwardOfSky/Fast-Radix-Sort
     */
void award_of_sky_sort(register int vector[], register unsigned int size)
{

    /* Support for variable sized integers without overflow warnings */

    /* Define std preliminary, abosulte max value and if there are bytes left */

    /* Check for max and min integer in [a, b[ array segment */

    register int *helper;          /* Helper array */
    register int *s, *k, i;        /* Array iterators */
    register int exp = *vector;    /* Bits sorted */
    register int max = exp;        /* Maximun range in array */
    register unsigned char *n, *m; /* Iterator of a byte within an integer */
    int swap = 0;                  /* Tells where sorted array is (if, sorted is in vector) */
    int last_byte_sorted = 0;      /* 1 if we had to sort the last byte */
    int next = 0;                  /* If 1 we skip the byte (all buckets are the same) */
    unsigned int init_size = size; /* Copy (needed if doing subdivisions) */

    /* Preliminary value to retrieve some initial info from the array */
    const int prel = (size > (64 << 1)) ? 64 : (size >> 3);

    /* Get max value (to know how many bytes to sort) */
    for (s = &vector[1], k = &vector[prel]; s < k; ++s)
    {
        if (*s > max || *s < exp)
        {
            if (*s > max)
            {
                max = *s;
            }
            else
            {
                exp = *s;
            }
        }
    };
    for (s = &vector[size - prel], k = &vector[size]; s < k; ++s)
    {
        if (*s > max || *s < exp)
        {
            if (*s > max)
            {
                max = *s;
            }
            else
            {
                exp = *s;
            }
        }
    };
    if (((max < -exp) ? -exp : max) <= (((unsigned int)(~0) >> 1) >> 7) || (max - exp == 0))
    {
        for (s = &vector[prel], k = &vector[size - prel]; s < k; ++s)
        {
            if (*s > max || *s < exp)
            {
                if (*s > max)
                {
                    max = *s;
                }
                else
                {
                    exp = *s;
                }
            }
        };
    }
    unsigned int diff = max - exp;
    max = ((max < -exp) ? -exp : max);

    /* Set number of bytes to sort according to max */
    int bytes_to_sort = 0;
    if (diff != 0)
    {
        bytes_to_sort = 1;
        exp = 8;
        while (exp < (sizeof(int) << 3) && (max >> (exp - 1)) > 0)
        {
            bytes_to_sort++;
            exp += 8;
        }
    }
    else
    { /* 1 unique element */
        return;
    }

    /* Helper array initialization */
    helper = (int *)malloc(sizeof(int) * size);

    /* Core algorithm template:                                              */
    /* 1 - Fill the buckets (using byte iterators)                           */
    /* 2 - Check if there is 1 bucket w/ all elements (no need to sort byte) */
    /* 3 - Set the pointers to the helper array if setting the byte          */
    /* 4 - Iterate the bucket values within the orginal array and chnage the */
    /*     corresponding values in the helper                                */

    int* point[0x100] = {0}; /* Array of pointers to the helper array */

    if (bytes_to_sort > 1 && size > 1600000)
    { /* MSB order (size > 1.6M) */

        exp -= 8;

        /* old_point will serve as a copy of the initial values of "point" */
        /* Beggining of each subarray in 1st subdivision (256 subarrays) */
        int *old_point[0x100] = {0};

        /* Sort last byte */
        int bucket[0x100] = {0};
        n = (unsigned char *)(vector) + (exp >> 3);
        for (m = (unsigned char *)(&vector[size & (~0 << 3)]); n < m;)
        {
            ++bucket[*n];
            n += sizeof(int);
            ++bucket[*n];
            n += sizeof(int);
            ++bucket[*n];
            n += sizeof(int);
            ++bucket[*n];
            n += sizeof(int);
            ++bucket[*n];
            n += sizeof(int);
            ++bucket[*n];
            n += sizeof(int);
            ++bucket[*n];
            n += sizeof(int);
            ++bucket[*n];
            n += sizeof(int);
        }
        for (n = (unsigned char *)(&vector[size & (~0 << 3)]) + (exp >> 3), m = (unsigned char *)(&vector[size]); n < m;)
        {
            ++bucket[*n];
            n += sizeof(int);
        }
        s = helper;
        next = 0;
        if (size > 65535)
        {
            for (i = 0; i < 0x100 && !next; ++i)
            {
                if (bucket[i] == size)
                {
                    old_point[i] = s;
                    point[i] = old_point[i] + size;
                    next = 1;
                }
            }
        }
        if (!next)
        {
            if (exp != ((sizeof(int) << 3) - 8))
            {
                for (i = 0; i < 0x100; s += bucket[i++])
                {
                    old_point[i] = s;
                    point[i] = s;
                }
            }
            else
            {
                for (i = 128; i < 0x100; s += bucket[i++])
                {
                    old_point[i] = s;
                    point[i] = s;
                }
                for (i = 0; i < 128; s += bucket[i++])
                {
                    old_point[i] = s;
                    point[i] = s;
                }
            }
            for (s = vector, k = &vector[size & (~0 << 3)]; s < k;)
            {
                *point[(*s >> exp) & 0xFF]++ = *s;
                ++s;
                *point[(*s >> exp) & 0xFF]++ = *s;
                ++s;
                *point[(*s >> exp) & 0xFF]++ = *s;
                ++s;
                *point[(*s >> exp) & 0xFF]++ = *s;
                ++s;
                *point[(*s >> exp) & 0xFF]++ = *s;
                ++s;
                *point[(*s >> exp) & 0xFF]++ = *s;
                ++s;
                *point[(*s >> exp) & 0xFF]++ = *s;
                ++s;
                *point[(*s >> exp) & 0xFF]++ = *s;
                ++s;
            }
            for (s = &vector[size & (~0 << 3)], k = &vector[size]; s < k;)
            {
                *point[(*s >> exp) & 0xFF]++ = *s;
                ++s;
            }
            swap = 1 - swap;
        }
        exp += 8;
        ;
        bytes_to_sort--;
        if (exp == (sizeof(int) << 3))
        {
            last_byte_sorted = 1;
        }

        /* 2nd subdivision only for 3 bytes or more (and size > 512M) */
        register int j;
        if (bytes_to_sort > 1 && size > 512000000)
        {

            exp -= 16;

            /* Same purpose as "point" and old_point" but for 2nd subdivision */
            int *point_2msb[0x10000] = {0};
            int *old_point_2msb[0x10000] = {0};
            int swap_copy = swap; /* Reset value of swap after each subarray */

            /* Sort second to last byte in LSB order (256 subdivisions) */
            for (j = 0; j < 0x100; ++j)
            {
                size = point[j] - old_point[j];
                swap = swap_copy;

                /* Define temporary vector and helper according to current swap*/
                register int *sub_help, *sub_vec;
                if (swap)
                {
                    sub_vec = old_point[j];
                    sub_help = vector + (old_point[j] - helper);
                }
                else
                {
                    sub_vec = vector + (old_point[j] - helper);
                    sub_help = old_point[j];
                }

                /* Temporary for ea subdivision, these work as "array riders" */
                int **point_2msb_rider = point_2msb + (j << 8);
                int **old_point_2msb_rider = old_point_2msb + (j << 8);

                /* Actually sort the byte */
                int bucket1[0x100] = {0};
                n = (unsigned char *)(sub_vec) + (exp >> 3);
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
                            old_point_2msb_rider[i] = s;
                            point_2msb_rider[i] = old_point_2msb_rider[i] + size;
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
                            point_2msb_rider[i] = old_point_2msb_rider[i] = s;
                        }
                    }
                    else
                    {
                        for (i = 128; i < 0x100; s += bucket1[i++])
                        {
                            point_2msb_rider[i] = old_point_2msb_rider[i] = s;
                        }
                        for (i = 0; i < 128; s += bucket1[i++])
                        {
                            point_2msb_rider[i] = old_point_2msb_rider[i] = s;
                        }
                    }
                    for (s = sub_vec, k = &sub_vec[size & (~0 << 3)]; s < k;)
                    {
                        *point_2msb_rider[(*s >> exp) & 0xFF]++ = *s;
                        ++s;
                        *point_2msb_rider[(*s >> exp) & 0xFF]++ = *s;
                        ++s;
                        *point_2msb_rider[(*s >> exp) & 0xFF]++ = *s;
                        ++s;
                        *point_2msb_rider[(*s >> exp) & 0xFF]++ = *s;
                        ++s;
                        *point_2msb_rider[(*s >> exp) & 0xFF]++ = *s;
                        ++s;
                        *point_2msb_rider[(*s >> exp) & 0xFF]++ = *s;
                        ++s;
                        *point_2msb_rider[(*s >> exp) & 0xFF]++ = *s;
                        ++s;
                        *point_2msb_rider[(*s >> exp) & 0xFF]++ = *s;
                        ++s;
                    }
                    for (s = &sub_vec[size & (~0 << 3)], k = &sub_vec[size]; s < k;)
                    {
                        *point_2msb_rider[(*s >> exp) & 0xFF]++ = *s;
                        ++s;
                    }
                    swap = 1 - swap;
                }
                exp += 8;
                ;
                exp -= 8;

                /* Make sure the sorted array is in the original vector */
                if (swap)
                {
                    if (swap_copy)
                    {
                        memcpy(sub_help, sub_vec, sizeof(int) * size);
                    }
                    else
                    {
                        memcpy(sub_vec, sub_help, sizeof(int) * size);
                    }
                }
            }
            swap = 0; /* Because now sorted array is in vector*/
            bytes_to_sort--;

            /* Sort remaining bytes in LSB order (65536 subdivisions) */
            max = 1 << ((bytes_to_sort - 1) << 3);
            for (j = 0; j < 0x10000; ++j)
            {

                exp = 0;
                size = point_2msb[j] - old_point_2msb[j];
                swap = 0; /* Reset swap (last swap is always 0) */

                /* Define temp arrays according to wether the first MSB byte */
                /* was sorted or not (array pointed by old_point_2msb changes) */
                register int *sub_help, *sub_vec;
                if (swap_copy)
                {
                    sub_vec = old_point_2msb[j];
                    sub_help = helper + (old_point_2msb[j] - vector);
                }
                else
                {
                    sub_vec = vector + (old_point_2msb[j] - helper);
                    sub_help = old_point_2msb[j];
                }

                while (exp < (sizeof(int) << 3) && (max >> exp) > 0)
                { /* While there are remaining bytes */
                    if (exp)
                    {
                        if (swap)
                        {
                            int bucket1[0x100] = {0};
                            n = (unsigned char *)(sub_help) + (exp >> 3);
                            for (m = (unsigned char *)(&sub_help[size & (~0 << 3)]); n < m;)
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
                            for (n = (unsigned char *)(&sub_help[size & (~0 << 3)]) + (exp >> 3), m = (unsigned char *)(&sub_help[size]); n < m;)
                            {
                                ++bucket1[*n];
                                n += sizeof(int);
                            }
                            s = sub_vec;
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
                                        point[i] = s;
                                    }
                                }
                                else
                                {
                                    for (i = 128; i < 0x100; s += bucket1[i++])
                                    {
                                        point[i] = s;
                                    }
                                    for (i = 0; i < 128; s += bucket1[i++])
                                    {
                                        point[i] = s;
                                    }
                                }
                                for (s = sub_help, k = &sub_help[size & (~0 << 3)]; s < k;)
                                {
                                    *point[(*s >> exp) & 0xFF]++ = *s;
                                    ++s;
                                    *point[(*s >> exp) & 0xFF]++ = *s;
                                    ++s;
                                    *point[(*s >> exp) & 0xFF]++ = *s;
                                    ++s;
                                    *point[(*s >> exp) & 0xFF]++ = *s;
                                    ++s;
                                    *point[(*s >> exp) & 0xFF]++ = *s;
                                    ++s;
                                    *point[(*s >> exp) & 0xFF]++ = *s;
                                    ++s;
                                    *point[(*s >> exp) & 0xFF]++ = *s;
                                    ++s;
                                    *point[(*s >> exp) & 0xFF]++ = *s;
                                    ++s;
                                }
                                for (s = &sub_help[size & (~0 << 3)], k = &sub_help[size]; s < k;)
                                {
                                    *point[(*s >> exp) & 0xFF]++ = *s;
                                    ++s;
                                }
                                swap = 1 - swap;
                            }
                            exp += 8;
                            ;
                        }
                        else
                        { /* Note: won't happen in 32 bit integers */
                            int bucket1[0x100] = {0};
                            n = (unsigned char *)(sub_vec) + (exp >> 3);
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
                                        point[i] = s;
                                    }
                                }
                                else
                                {
                                    for (i = 128; i < 0x100; s += bucket1[i++])
                                    {
                                        point[i] = s;
                                    }
                                    for (i = 0; i < 128; s += bucket1[i++])
                                    {
                                        point[i] = s;
                                    }
                                }
                                for (s = sub_vec, k = &sub_vec[size & (~0 << 3)]; s < k;)
                                {
                                    *point[(*s >> exp) & 0xFF]++ = *s;
                                    ++s;
                                    *point[(*s >> exp) & 0xFF]++ = *s;
                                    ++s;
                                    *point[(*s >> exp) & 0xFF]++ = *s;
                                    ++s;
                                    *point[(*s >> exp) & 0xFF]++ = *s;
                                    ++s;
                                    *point[(*s >> exp) & 0xFF]++ = *s;
                                    ++s;
                                    *point[(*s >> exp) & 0xFF]++ = *s;
                                    ++s;
                                    *point[(*s >> exp) & 0xFF]++ = *s;
                                    ++s;
                                    *point[(*s >> exp) & 0xFF]++ = *s;
                                    ++s;
                                }
                                for (s = &sub_vec[size & (~0 << 3)], k = &sub_vec[size]; s < k;)
                                {
                                    *point[(*s >> exp) & 0xFF]++ = *s;
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
                        int bucket1[0x100] = {0};
                        n = (unsigned char *)(sub_vec) + (exp >> 3);
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
                                    point[i] = s;
                                }
                            }
                            else
                            {
                                for (i = 128; i < 0x100; s += bucket1[i++])
                                {
                                    point[i] = s;
                                }
                                for (i = 0; i < 128; s += bucket1[i++])
                                {
                                    point[i] = s;
                                }
                            }
                            for (s = sub_vec, k = &sub_vec[size & (~0 << 3)]; s < k;)
                            {
                                *point[(*s) & 0xFF]++ = *s;
                                ++s;
                                *point[(*s) & 0xFF]++ = *s;
                                ++s;
                                *point[(*s) & 0xFF]++ = *s;
                                ++s;
                                *point[(*s) & 0xFF]++ = *s;
                                ++s;
                                *point[(*s) & 0xFF]++ = *s;
                                ++s;
                                *point[(*s) & 0xFF]++ = *s;
                                ++s;
                                *point[(*s) & 0xFF]++ = *s;
                                ++s;
                                *point[(*s) & 0xFF]++ = *s;
                                ++s;
                            }
                            for (s = &sub_vec[size & (~0 << 3)], k = &sub_vec[size]; s < k;)
                            {
                                *point[(*s) & 0xFF]++ = *s;
                                ++s;
                            }
                            swap = 1 - swap;
                        }
                        exp += 8;
                        ;
                    }
                }

                if (swap)
                { /* Force sorted segments to be in original vector */
                    memcpy(sub_vec, sub_help, sizeof(int) * size);
                }
            }
            swap = 0;
        }
        else
        {

            /* Start sorting from LSB now */
            max = 1 << ((bytes_to_sort - 1) << 3);
            int swap_copy = swap; /* Once more, reset swap in ea subarray */
            for (j = 0; j < 0x100; ++j)
            {
                exp = 0;
                size = point[j] - old_point[j];
                swap = swap_copy;

                register int *sub_help, *sub_vec; /* Temprary arrays */
                if (swap)
                {
                    sub_help = vector + (old_point[j] - helper);
                    sub_vec = old_point[j];
                }
                else
                {
                    sub_help = old_point[j];
                    sub_vec = vector + (old_point[j] - helper);
                }

                int *temp_point[0x100]; /* Temp ptrs, just to sort this segment */
                while (exp < (sizeof(int) << 3) && (max >> exp) > 0)
                { /* While there are remaining bytes */
                    if (exp)
                    {
                        if (swap != swap_copy)
                        {
                            int bucket1[0x100] = {0};
                            n = (unsigned char *)(sub_help) + (exp >> 3);
                            for (m = (unsigned char *)(&sub_help[size & (~0 << 3)]); n < m;)
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
                            for (n = (unsigned char *)(&sub_help[size & (~0 << 3)]) + (exp >> 3), m = (unsigned char *)(&sub_help[size]); n < m;)
                            {
                                ++bucket1[*n];
                                n += sizeof(int);
                            }
                            s = sub_vec;
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
                            int bucket1[0x100] = {0};
                            n = (unsigned char *)(sub_vec) + (exp >> 3);
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
                        int buckett[0x100] = {0};
                        n = (unsigned char *)(sub_vec) + (exp >> 3);
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

                if (swap)
                { /* Again, make sure sorted array is the vector */
                    if (swap_copy)
                    {
                        memcpy(sub_help, sub_vec, sizeof(int) * size);
                    }
                    else
                    {
                        memcpy(sub_vec, sub_help, sizeof(int) * size);
                    }
                }
            }
            swap = 0;
        }
    }
    else if (bytes_to_sort > 0)
    { /* Use normal LSB radix (no subarrays) */

        exp = 0; /* Start at the first byte */

        max = 1 << ((bytes_to_sort - 1) << 3);
        while (exp < (sizeof(int) << 3) && (max >> exp) > 0)
        { /* Sort until there are no bytes left */
            if (exp)
            {
                if (swap)
                {
                    int bucket[0x100] = {0};
                    n = (unsigned char *)(helper) + (exp >> 3);
                    for (m = (unsigned char *)(&helper[size & (~0 << 3)]); n < m;)
                    {
                        ++bucket[*n];
                        n += sizeof(int);
                        ++bucket[*n];
                        n += sizeof(int);
                        ++bucket[*n];
                        n += sizeof(int);
                        ++bucket[*n];
                        n += sizeof(int);
                        ++bucket[*n];
                        n += sizeof(int);
                        ++bucket[*n];
                        n += sizeof(int);
                        ++bucket[*n];
                        n += sizeof(int);
                        ++bucket[*n];
                        n += sizeof(int);
                    }
                    for (n = (unsigned char *)(&helper[size & (~0 << 3)]) + (exp >> 3), m = (unsigned char *)(&helper[size]); n < m;)
                    {
                        ++bucket[*n];
                        n += sizeof(int);
                    }
                    s = vector;
                    next = 0;
                    if (size > 65535)
                    {
                        for (i = 0; i < 0x100 && !next; ++i)
                        {
                            if (bucket[i] == size)
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
                            for (i = 0; i < 0x100; s += bucket[i++])
                            {
                                point[i] = s;
                            }
                        }
                        else
                        {
                            for (i = 128; i < 0x100; s += bucket[i++])
                            {
                                point[i] = s;
                            }
                            for (i = 0; i < 128; s += bucket[i++])
                            {
                                point[i] = s;
                            }
                        }
                        for (s = helper, k = &helper[size & (~0 << 3)]; s < k;)
                        {
                            *point[(*s >> exp) & 0xFF]++ = *s;
                            ++s;
                            *point[(*s >> exp) & 0xFF]++ = *s;
                            ++s;
                            *point[(*s >> exp) & 0xFF]++ = *s;
                            ++s;
                            *point[(*s >> exp) & 0xFF]++ = *s;
                            ++s;
                            *point[(*s >> exp) & 0xFF]++ = *s;
                            ++s;
                            *point[(*s >> exp) & 0xFF]++ = *s;
                            ++s;
                            *point[(*s >> exp) & 0xFF]++ = *s;
                            ++s;
                            *point[(*s >> exp) & 0xFF]++ = *s;
                            ++s;
                        }
                        for (s = &helper[size & (~0 << 3)], k = &helper[size]; s < k;)
                        {
                            *point[(*s >> exp) & 0xFF]++ = *s;
                            ++s;
                        }
                        swap = 1 - swap;
                    }
                    exp += 8;
                    ;
                }
                else
                {
                    int bucket[0x100] = {0};
                    n = (unsigned char *)(vector) + (exp >> 3);
                    for (m = (unsigned char *)(&vector[size & (~0 << 3)]); n < m;)
                    {
                        ++bucket[*n];
                        n += sizeof(int);
                        ++bucket[*n];
                        n += sizeof(int);
                        ++bucket[*n];
                        n += sizeof(int);
                        ++bucket[*n];
                        n += sizeof(int);
                        ++bucket[*n];
                        n += sizeof(int);
                        ++bucket[*n];
                        n += sizeof(int);
                        ++bucket[*n];
                        n += sizeof(int);
                        ++bucket[*n];
                        n += sizeof(int);
                    }
                    for (n = (unsigned char *)(&vector[size & (~0 << 3)]) + (exp >> 3), m = (unsigned char *)(&vector[size]); n < m;)
                    {
                        ++bucket[*n];
                        n += sizeof(int);
                    }
                    s = helper;
                    next = 0;
                    if (size > 65535)
                    {
                        for (i = 0; i < 0x100 && !next; ++i)
                        {
                            if (bucket[i] == size)
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
                            for (i = 0; i < 0x100; s += bucket[i++])
                            {
                                point[i] = s;
                            }
                        }
                        else
                        {
                            for (i = 128; i < 0x100; s += bucket[i++])
                            {
                                point[i] = s;
                            }
                            for (i = 0; i < 128; s += bucket[i++])
                            {
                                point[i] = s;
                            }
                        }
                        for (s = vector, k = &vector[size & (~0 << 3)]; s < k;)
                        {
                            *point[(*s >> exp) & 0xFF]++ = *s;
                            ++s;
                            *point[(*s >> exp) & 0xFF]++ = *s;
                            ++s;
                            *point[(*s >> exp) & 0xFF]++ = *s;
                            ++s;
                            *point[(*s >> exp) & 0xFF]++ = *s;
                            ++s;
                            *point[(*s >> exp) & 0xFF]++ = *s;
                            ++s;
                            *point[(*s >> exp) & 0xFF]++ = *s;
                            ++s;
                            *point[(*s >> exp) & 0xFF]++ = *s;
                            ++s;
                            *point[(*s >> exp) & 0xFF]++ = *s;
                            ++s;
                        }
                        for (s = &vector[size & (~0 << 3)], k = &vector[size]; s < k;)
                        {
                            *point[(*s >> exp) & 0xFF]++ = *s;
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
                int bucket[0x100] = {0};
                n = (unsigned char *)(vector) + (exp >> 3);
                for (m = (unsigned char *)(&vector[size & (~0 << 3)]); n < m;)
                {
                    ++bucket[*n];
                    n += sizeof(int);
                    ++bucket[*n];
                    n += sizeof(int);
                    ++bucket[*n];
                    n += sizeof(int);
                    ++bucket[*n];
                    n += sizeof(int);
                    ++bucket[*n];
                    n += sizeof(int);
                    ++bucket[*n];
                    n += sizeof(int);
                    ++bucket[*n];
                    n += sizeof(int);
                    ++bucket[*n];
                    n += sizeof(int);
                }
                for (n = (unsigned char *)(&vector[size & (~0 << 3)]) + (exp >> 3), m = (unsigned char *)(&vector[size]); n < m;)
                {
                    ++bucket[*n];
                    n += sizeof(int);
                }
                s = helper;
                next = 0;
                if (size > 65535)
                {
                    for (i = 0; i < 0x100 && !next; ++i)
                    {
                        if (bucket[i] == size)
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
                        for (i = 0; i < 0x100; s += bucket[i++])
                        {
                            point[i] = s;
                        }
                    }
                    else
                    {
                        for (i = 128; i < 0x100; s += bucket[i++])
                        {
                            point[i] = s;
                        }
                        for (i = 0; i < 128; s += bucket[i++])
                        {
                            point[i] = s;
                        }
                    }
                    for (s = vector, k = &vector[size & (~0 << 3)]; s < k;)
                    {
                        *point[(*s) & 0xFF]++ = *s;
                        ++s;
                        *point[(*s) & 0xFF]++ = *s;
                        ++s;
                        *point[(*s) & 0xFF]++ = *s;
                        ++s;
                        *point[(*s) & 0xFF]++ = *s;
                        ++s;
                        *point[(*s) & 0xFF]++ = *s;
                        ++s;
                        *point[(*s) & 0xFF]++ = *s;
                        ++s;
                        *point[(*s) & 0xFF]++ = *s;
                        ++s;
                        *point[(*s) & 0xFF]++ = *s;
                        ++s;
                    }
                    for (s = &vector[size & (~0 << 3)], k = &vector[size]; s < k;)
                    {
                        *point[(*s) & 0xFF]++ = *s;
                        ++s;
                    }
                    swap = 1 - swap;
                }
                exp += 8;
                ;
            }

            if (exp == (sizeof(int) << 3))
            { /* Check if last byte was sorted */
                last_byte_sorted = 1;
            }
        }
    }

    /* Find the first negative element in the array in binsearch style */

    size = init_size; /* Restore size */
    int *v = vector;  /* Temporary values for the vfector and helper arrays */
    int *h = helper;
    /* In case the array has both negative and positive integers, find the    */
    /* index of the first negative integer and put those numbers in the start */
    if (!last_byte_sorted && (((*v ^ v[size - 1]) < 0 && !swap) ||
                              ((*h ^ h[size - 1]) < 0 && swap)))
    {
        /* If sorted array is in vector, use helper to re-order and vs */
        if (!swap)
        {
            int increment = size >> 1;
            int offset = increment;
            while ((v[offset] ^ v[offset - 1]) >= 0)
            {
                increment = (increment > 1) ? increment >> 1 : 1;
                offset = (v[offset] < 0) ? offset - increment : offset + increment;
            };

            int tminusoff = size - offset;
            if (offset < tminusoff)
            {
                memcpy(h, v, sizeof(int) * offset);
                memcpy(v, v + offset, sizeof(int) * tminusoff);
                memcpy(v + tminusoff, h, sizeof(int) * offset);
            }
            else
            {
                memcpy(h, v + offset, sizeof(int) * tminusoff);
                memmove(v + tminusoff, v, sizeof(int) * offset);
                memcpy(v, h, sizeof(int) * tminusoff);
            }
        }
        else
        {
            int increment = size >> 1;
            int offset = increment;
            while ((h[offset] ^ h[offset - 1]) >= 0)
            {
                increment = (increment > 1) ? increment >> 1 : 1;
                offset = (h[offset] < 0) ? offset - increment : offset + increment;
            };

            int tminusoff = size - offset;
            memcpy(v, h + offset, sizeof(int) * tminusoff);
            memcpy(v + tminusoff, h, sizeof(int) * (size - tminusoff));
        }
    }
    else if (swap)
    {
        memcpy(v, h, sizeof(int) * size);
    }

    /* Free helper array */
    free(helper);
}
void award_of_sky_sort(register uint32_t vector[], register unsigned int size)
{
    award_of_sky_sort((int *)vector, size);
}
} // namespace made
#endif // !AWARD_OF_SKY_PARSED_H_
       //void Sort(int *arr, int size)
       //{
       //    made::award_of_sky_sort((uint32_t*)arr, size);
       //}