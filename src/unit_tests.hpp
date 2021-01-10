#pragma once

#include"tester.hpp"
#include"vec3.hpp"
#include"vec_s.hpp"


void UnitTests() {

    // TEST(Test_Vec3_A, "Vec3 equality");
    // TEST(Test_Vec3_B, "Vec3 distance equality", 8, 8, 8);

    TEST(T_VecS_1, "VecS Distance");
    TEST(T_VecS_2, "VecS HeadingTo");
    TEST(T_VecS_3, "VecS Movement");

    TEST_SUMMARY();
}