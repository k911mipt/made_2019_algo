#include <iostream>
#include <fstream>
#include <algorithm>

inline void incModal(int* &cur, int* &left, int* &right) {
    if (++cur == right)
        cur = left;
}

inline int findMaximum(int* &skippedCur, int* &left, int* &right) {
    int maximum;
    if (skippedCur == left) {
        maximum = *(left + 1);
    } else {
        maximum = *left;
    }
    for (int* p = left + 1; p < skippedCur; p++)
        if (maximum < *p)
            maximum = *p;
    for (int* p = skippedCur + 1; p < right; p++)
        if (maximum < *p)
            maximum = *p;
    return maximum;
}

void solveWithQueue(const int &n, int* &arr, const int &windowSize) {
    int* left = arr;
    int* right = arr + windowSize;
    int* next = right;
    int* last = arr + n;
    int maximum = std::max(*left, findMaximum(left, left, right));
    std::cout << maximum << " ";
    int* cur = left;
    while (next < last) {
        if (maximum == *cur) {
            maximum = findMaximum(cur, left, right);
        }
        *cur = *next;
        maximum = std::max(*cur, maximum);
        std::cout << maximum << " ";
        incModal(cur, left, right);
        next++;
    }
}

void solve(int n) {
    int* arr = new int[n];
    for (int i = 0; i < n; i++) {
        std::cin >> arr[i];
    }
    int windowSize;
    std::cin >> windowSize;
    if (windowSize == 1) {
        for (int* p = arr; p < arr + n; p++) {
            std::cout << *p << " ";
        }
    } else {
        solveWithQueue(n, arr, windowSize);
    }
    delete arr;
}

int main() {
    std::ifstream in("input.txt");
    std::cin.rdbuf(in.rdbuf());
    int n;
    std::cin >> n;
    solve(n);
    return EXIT_SUCCESS;
}