#pragma once
#ifndef THREE_WAY_H_
#define THREE_WAY_H_
namespace three_way {
#ifdef INLINE_SWAP
#undef INLINE_SWAP
#endif
#define INLINE_SWAP(a, b) {const int __SWAP_TEMP__ = a; a = b; b = __SWAP_TEMP__;}
    /* This function partitions a[] in three parts
       a) a[l..i] contains all elements smaller than pivot
       b) a[i+1..j-1] contains all occurrences of pivot
       c) a[j..r] contains all elements greater than pivot */
    void partition(size_type a[], int l, int r, int &i, int &j)
    {
        i = l - 1, j = r;
        int p = l - 1, q = r;
        size_type v = a[r];

        while (true)
        {
            // From left, find the first element greater than 
            // or equal to v. This loop will definitely terminate 
            // as v is last element 
            while (a[++i] < v);

            // From right, find the first element smaller than or 
            // equal to v 
            while (v < a[--j])
                if (j == l)
                    break;

            // If i and j cross, then we are done 
            if (i >= j) break;

            // Swap, so that smaller goes on left greater goes on right 
            INLINE_SWAP(a[i], a[j]);

            // Move all same left occurrence of pivot to beginning of 
            // array and keep count using p 
            if (a[i] == v)
            {
                p++;
                INLINE_SWAP(a[p], a[i]);
            }

            // Move all same right occurrence of pivot to end of array 
            // and keep count using q 
            if (a[j] == v)
            {
                q--;
                INLINE_SWAP(a[j], a[q]);
            }
        }

        // Move pivot element to its correct index 
        INLINE_SWAP(a[i], a[r]);

        // Move all left same occurrences from beginning 
        // to adjacent to arr[i] 
        j = i - 1;
        for (int k = l; k < p; k++, j--)
            INLINE_SWAP(a[k], a[j]);

        // Move all right same occurrences from end 
        // to adjacent to arr[i] 
        i = i + 1;
        for (int k = r - 1; k > q; k--, i++)
            INLINE_SWAP(a[i], a[k]);
    }

    // 3-way partition based quick sort 
    void quicksort(size_type a[], int l, int r)
    {
        if (r <= l) return;

        int i, j;

        // Note that i and j are passed as reference 
        partition(a, l, r, i, j);

        // Recur 
        quicksort(a, l, j);
        quicksort(a, i, r);
    }
}
#undef INLINE_SWAP
#endif THREE_WAY_H_