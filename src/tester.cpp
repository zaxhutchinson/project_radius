#include"tester.hpp"

void TEST_SUMMARY() {
    std::cout << "==========================================================\n";
    std::cout << "TOTAL TESTS: " << test_num << std::endl;
    std::cout << "  PASSED: " << tests_passed << std::endl;
    std::cout << "  FAILED: " << tests_failed << std::endl;
    std::cout << "==========================================================\n";
}