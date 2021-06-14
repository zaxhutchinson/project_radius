#include"log.hpp"

namespace zxlog {
    void Init(bool debug) {
        // Erase the files.
        std::ofstream out(LOG_FILE,std::ofstream::out);
        out.close();
        _debug = debug;
    }
    void Error(str msg) {
        std::ofstream out(LOG_FILE,std::ofstream::app);
        out << "ERROR:  " << msg << std::endl;
        out.close();
    }
    void Debug(str msg) {
        if(_debug) {
            std::ofstream out(LOG_FILE,std::ofstream::app);
            out << "DEBUG:  " << msg << std::endl;
            out.close();
        }
    }

    void Warning(str msg) {
        std::ofstream out(LOG_FILE,std::ofstream::app);
        out << "WARNING:  " << msg << std::endl;
        out.close();
    }
};