#include <iostream>

#include "common.h"
#include "timer.h"
#include "three_way.h"
#include "made_sort.h"
#include "k911mipt_sort.h"


#include "award_of_sky_parsed.h"
//#include "award_of_sky.h"
//#include "radix_sort.h"
//#include "gorset.h"
//#include "radix_extra_memory.h"
//#include "custom_radix_extra_memory.h"
//#include "three_way.h"
//#include "stereopsis.h"


using namespace common;
using namespace made;

void run_tests() {
    test(made::k9_sort);
}

int main() {
    run_tests();
    tests_count = 1;
    ElementCounter n = 25000000;
    Element* arr = new Element[n];
    generate(arr, n);
    //TimeItStd(arr, arr + n);
    //show_name = false;
    TimeIt(arr, n, award_parsed::award_of_sky_sort, "award_of_sky");
    TimeIt(arr, n, made::k9_sort, "k9_sort");
    TimeIt(arr, n, made::radix_sort, "mixed_radix");
    //TimeIt(arr, n, made::MSDSort, "MSD_LSD");
    //TimeIt(arr, n, made::LSDSort, "LSD");
    //TimeIt(arr, n, radix_inplace::sort, "radix_inplace");
    //TimeIt(arr, n, gorset::sort, "gorset");
    //TimeIt(arr, n, radix_extra_memory::RadixSort, "radix_extra_memory");
    //TimeIt(arr, n, custom_radix_extra_memory::RadixSort, "LSD_SIMD_try");
    //TimeIt(arr, n, three_way::sort, "three_way_quicksort");
    //TimeIt(arr, n, stereopsis::RadixSort11, "stereopsis");
    delete[] arr;
    return 0;
}