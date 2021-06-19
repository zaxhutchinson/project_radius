#pragma once

#include<fstream>
#include<json.hpp>
#include"zxlb.hpp"

namespace config {
    inline double CORRECT_EXPECTED;
    inline double INCORRECT_EXPECTED;
    inline i64 TASK_DURATION;

    void InitConfig();
};