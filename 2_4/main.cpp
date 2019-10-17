#include <iostream>
#include <fstream>
//#include <cstring>
//#include <algorithm>

void naiveSolution(const int &n, int* &arr, const int &windowSize) {
    //O(n^2)
    int start = 0;
    for (int end = windowSize; end < n; end++) {
        int max = arr[start];
        for (int i = start + 1; i < end; i++) {
            if (arr[i] > max) {
                max = arr[i];
            }
        }
        std::cout << max << " ";
        start++;
    }

}
void solve(int n) {
    int* arr = new int[n];
    for (int i = 0; i < n; i++) {
        std::cin >> arr[i];
    }
    int windowSize;
    std::cin >> windowSize;
    naiveSolution(n, arr, windowSize);

    std::cout << windowSize << std::endl;
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