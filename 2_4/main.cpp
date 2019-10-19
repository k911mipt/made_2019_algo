#include <iostream>
#include <fstream>
#include <algorithm>

namespace made {

    inline int findMaximum(int* left, int* right) {
        int maximum = *(left);
        for (; left < right; left++) {
            maximum = std::max(maximum, *left);
        }
        return maximum;
    }

    void solveWithQueue(const int n, int* arr, const int windowSize, int* answers) {
        int* left = arr;
        int* right = arr + windowSize;
        int* last = arr + n;
        int maximum = findMaximum(left, right);
        *answers = maximum;
        while (right < last) {
            if (*right >= maximum) {
                maximum = *right;
                left++;
                right++;
            }
            else if (maximum == *left) {
                left++;
                right++;
                maximum = findMaximum(left, right);
            }
            else if (maximum > *left) {
                left++;
                right++;
            }
            answers++;
            *answers = maximum;
        } 
    }

    void solve(const int n, int* arr, const int windowSize, int* answers) {
        if (windowSize > 1) {
            solveWithQueue(n, arr, windowSize, answers);
        }
        else {
            // Direct solution
            for (int i = 0; i <= n - windowSize; i++, arr++, answers++) {
                *answers = *arr;
            }
        }
    }
}

int main() {
    // load data
    std::ifstream in("input.txt");
    std::cin.rdbuf(in.rdbuf());
    int n;
    std::cin >> n;
    int* arr = new int[n];
    for (int i = 0; i < n; i++) {
        std::cin >> arr[i];
    }
    int windowSize;
    std::cin >> windowSize;
    
    // solving
    int answersSize = n - windowSize + 1;
    int* answers = new int[answersSize];
    made::solve(n, arr, windowSize, answers);

    //output
    for (int i = 0; i < answersSize; i++) {
        std::cout << answers[i] << " ";
    }

    delete answers;
    delete arr;
    return EXIT_SUCCESS;
}