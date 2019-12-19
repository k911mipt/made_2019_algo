#include <vector>
#include <iostream>
#include <functional>
#include <vector>

#include "memcheck_crt.h"
#include "common.h"

#include "a_solver.hpp"
#include "puzzle15.hpp"

using TestFunc = std::function<bool()>;
using TestGetter = std::function<std::vector<TestFunc>()>;
using std::vector;

vector<TestFunc> GetTests();

bool CheckSolution(vector<uint32> init, int req_steps) {
    std::cout << "Solving, needed " << req_steps << std::endl;
    Position pos(init);
    pos.Print();
    const auto result = ASolver::SolvePuzzle(init);
    for (auto r : result.second) {
        switch (r) {
        case 'L':
            pos = pos.Move(Left);
            continue;
        case 'R':
            pos = pos.Move(Right);
            continue;
        case 'U':
            pos = pos.Move(Up);
            continue;
        case 'D':
            pos = pos.Move(Down);
            continue;
        default:
            assert(false);
        }
    }
    if (result.second.size() != req_steps) {
        std::cout << "got " << result.second.size() << " steps, expected " << req_steps << "!";
    }
    return pos.IsFinish();
}


std::vector<TestFunc> GetTests() {
    return {
        []() {return CheckSolution(vector<uint32>(
            {
                 1,  2,  3,  0,
                 5,  6,  7,  4,
                 9, 10, 11,  8,
                13, 14, 15, 12
            }), 3); },
        []() {return CheckSolution(vector<uint32>(
            {
                 5,  1,  7, 15,
                14,  9, 10,  3,
                 2, 11,  4,  6,
                13, 12,  8,  0
            }), 40); },
        []() {return CheckSolution(vector<uint32>(
            {
                 0, 12,  9, 13,
                15, 11, 10, 14,
                 3,  7,  2,  5,
                 4,  8,  6, 1,
            }), 80); },
        []() {return CheckSolution(vector<uint32>(
            {
                 4,  6, 13, 15,
                 7, 14, 11, 10,
                12,  8,  2,  1,
                 3,  0,  9,  5,
            }), 66); },
    };
}

int RunTests(const TestGetter& tests_getter) {
    std::vector<TestFunc> tests = tests_getter();
    std::size_t tests_count = tests.size();
    bool failed = false;
    bool test_result = false;
    std::string error_msg;
    for (std::size_t i = 0; i < tests_count; ++i) {
        std::cout << "Running test " << i + 1 << "/" << tests_count << "... ";
        error_msg = "";
        try {
            test_result = tests[i]();
        }
        catch (std::exception & e) {
            error_msg = e.what();
            test_result = false;
        }
        failed = failed || !test_result;
        if (test_result) {
            std::cout << " OK";
        }
        else {
            std::cerr << std::endl << "Failed!" << std::endl;
            if (error_msg != "") {
                std::cerr << error_msg;
            }
        }
        std::cout << std::endl;
    }
    if (!failed) {
        std::cout << "All tests passed" << std::endl;
    }
    else {
        std::cout << "Some tests failed, check log for more details" << std::endl;
    }
    return 0;
}


int main() {
    ENABLE_CRT;
    RunTests(&GetTests);
}