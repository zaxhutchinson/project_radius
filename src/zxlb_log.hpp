#pragma once

#include<fstream>

#include"zxlb.hpp"

namespace zxlog {
    inline bool _debug;
    inline str const LOG_FILE("log/log.log");
    void Init(bool debug=false);
    void Error(str msg);
    void Debug(str msg);
    void Warning(str msg);
};