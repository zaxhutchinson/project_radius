/* TESTer
    by zax DEC 2020
    v 0.1 - Init

    Variadic template inspirated by: 
        https://stackoverflow.com/questions/24949700/how-to-print-input-c-
        function-parameter-values-automatically
*/


#pragma once

#include<iostream>

#include"zxlb.hpp"

enum class Eval {
    EQ,
    NEQ,
    LT,
    LTEQ,
    GT,
    GTEQ
};

static int test_num = 0;
static int tests_passed = 0;
static int tests_failed = 0;

// print function. Probably a better way to both unpack and print
// the arg values.
template<typename T>
static void pp(T t) {
    std::cout << t << " ";
}


// TEST function
// Pass in the function, and the name of the test along with the arguments
// that the function needs.
template<class Fun, class... Args>
static void TEST(Fun&& f, std::string name, Args&&... args) {

    UTRTN rtn = std::forward<Fun>(f)(std::forward<Args>(args)...);

    if(rtn.first) tests_passed++;
    else tests_failed++;

    std::cout << (rtn.first ? "\033[32mPASSED\033[0m" : "\033[31mFAILED\033[0m") 
        << " --- ";

    std::cout << "TEST# " << test_num++ << " --- NAME: " << name;

    if(sizeof...(args) > 0) {
        std::cout << " --- ARGS: ";
        [[maybe_unused]] int dummy[] = {0, ((void) pp(std::forward<Args>(args)),0) ... };
    } else {
        std::cout << " --- ARGS: None ";
    }
    std::cout << std::endl;

    if(!rtn.first) {
        std::cout << "\tRESULT: " << rtn.second << std::endl;
    }
}

void TEST_SUMMARY();