#ifndef TEST_ENGINE
#define TEST_ENGINE

#include <assert.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdlib.h>
#include <limits.h>

#include <string>

#include "../util/error.hh"

class Tester {
    private:
    int logFD;
    std::string logPath;

    public:
    Tester(std::string logPath) {
        this->logPath = logPath;
    }

    void startTest() {
        this->logFD = open(this->logPath.c_str(), O_TRUNC | O_RDWR | O_CREAT, 0777);
        ERROR_KILL(this->logFD, "openc", exit);
    }

    void endTest() {
        int closeCheck = close(this->logFD);
        ERROR_KILL(closeCheck, "close", exit);
    }

    void logWrite(std::string log) {
        write(this->logFD, log.c_str(), log.length());
        write(this->logFD, "\n", 1);    
    }

    void writeHeader(std::string header) {
        write(this->logFD, header.c_str(), header.length());
        write(this->logFD, "\n", 1);
    }

    void checkTrue(bool val, std::string testName) {
        if (val) { 
            logWrite("PASSED TRUTHY CHECK"); 
        }
        else { 
            logWrite("FAILED - EXPECTED TRUE");
        }

        logWrite("\t" + testName);
    }

    void checkFalse(bool val, std::string testName) {
        if (!val) { 
            logWrite("PASSED FALSITY CHECK"); 
        }
        else { 
            logWrite("FAILED - EXPECTED FALSE"); 
        }

        logWrite("\t" + testName);
    }
};

#endif
