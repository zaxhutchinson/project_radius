#pragma once

#include"tester.hpp"
#include"vec3.hpp"



void UnitTests() {

    TEST(Test_Vec3_A, "Vec3 equality");
    TEST(Test_Vec3_B, "Vec3 distance equality", 8, 8, 8);

    TEST_SUMMARY();
}