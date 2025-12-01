#pragma once

#include <string>
#include <vector>
#include <stdexcept>

namespace test {
    void runTest(void(* test)(), std::string testName, std::string logPath);
    void runTest(void(* test)(), std::string testName, int logFD);
    void logWrite(std::string logMessage, int logFD);
    void writeHeader(std::string header, int logFD);
    void writeFooter(std::string footer, int logFD);

    void checkTrue(bool val);
    void checkFalse(bool val);
    void checkError(int val);
    void checkBufferEqual(void *buf1, void *buf2, int length);
    void checkNumericEqual(int val1, int val2);
    void checkStringEqual(std::string val1, std::string val2);
};