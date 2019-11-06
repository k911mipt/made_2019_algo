#include <iostream>

#include "common.h"
#include "timer.h"
#include "three_way.h"
#include "made_sort.h"
#include "k911mipt_sort.h"
#include "k911mipt_avx.h"


#include "award_of_sky_parsed.h"
//#include "award_of_sky.h"
//#include "radix_sort.h"
// #include "gorset.h"
//#include "n_log_log_n.h"
//#include "radix_extra_memory.h"
//#include "custom_radix_extra_memory.h"
//#include "three_way.h"
//#include "stereopsis.h"


using namespace common;
using namespace made;


void run_tests() {
    //test(made::k9_sort);
    //common::test_size = 25000000;
    // common::test_size = 100;
    // test(nloglogn);
    //test(made::avx::k9_sort);
    // common::test_size = 10000;
    //test(made::avx::k9_sort);
}
//using bruteforce::nums;
//void brute(Element *begin, ElementCounter size) {
//    //const FE D4 = 5; const Digit D4shift = DMax - D4;
//    //const FE D3 = 5; const FE D3shift = D4shift - D3;
//    //const FE D2 = 10; const FE D2shift = D3shift - D2;
//    //const FE D1 = D2shift; const Digit D1shift = D2shift - D1;
//    int old_tests_count = tests_count;
//    tests_count = 50;
//    nums.D4shift = DMax - nums.D4;
//    nums.D3shift = nums.D4shift - nums.D3;
//    nums.D2shift = nums.D3shift - nums.D2;
//    nums.D1 = nums.D2shift;
//    nums.D1shift = nums.D2shift - nums.D1;
//    for (FE d4 = 5; d4 <= 7; ++d4) {
//        for (FE d3 = 6; d3 <= 8; ++d3) {
//            for (FE d2 = 8; d2 <= 9; ++d2) {
//                nums.D4 = d4; nums.D3 = d3; nums.D2 = d2;
//                nums.D4shift = DMax - nums.D4;
//                nums.D3shift = nums.D4shift - nums.D3;
//                nums.D2shift = nums.D3shift - nums.D2;
//                nums.D1 = nums.D2shift;
//                nums.D1shift = nums.D2shift - nums.D1;
//                if (nums.D1 > 16)
//                    continue;
//                if (abs(nums.D1 - nums.D2) > 2)
//                    continue;
//                //if ((abs(nums.D1 - nums.D2) > 2)||(abs(nums.D1 - nums.D3) > 2)||(abs(nums.D3 - nums.D2) > 2))
//                //    continue;
//                std::cout << "D4: " << d4 << "\tD3 " << d3 << "\tD2 " << d2 << "\tD1 " << nums.D1 << "\t time:\t";
//                TimeItWithoutTests(begin, size, bruteforce::MSBCaller);
//            }
//        }
//    }
//    //TimeItWithoutTests(begin, size, bruteforce::MSBCaller);
//    //for (int i =0; i < 1; ++i) {
//    //    Element *buffer = (Element *)malloc(sizeof(Element) * size);
//    //    MSB4(begin, size, buffer, nums);// currently da best
//    //    free(buffer);
//    //}
//    tests_count = old_tests_count;
//}
int main() {
    run_tests();
    tests_count = 1;
    ElementCounter n = 25000000;
    Element* arr = new Element[n];
    generate(arr, n);

    //brute(arr, n);
    //TimeItStd(arr, arr + n);
    // show_name = false;
    test_size = 25000000;
    // common::test_size = 25000000;
    TimeIt(arr, n, made::avx::k9_sort, "k9_avx");
    TimeIt(arr, n, made::bruteforce::MSBCaller, "regular 6 8 8 8");
    //test_size = 10000;
    TimeIt(arr, n, made::k9_sort, "k9_sort");
    TimeIt(arr, n, award_parsed::award_of_sky_sort, "award_of_sky");
    // TimeIt(arr, n, made::MSDSort, "MSD_LSD");
    TimeIt(arr, n, made::radix_sort, "mixed_radix");

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