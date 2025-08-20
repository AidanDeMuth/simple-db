#ifndef TEST_ENGINE
#define TEST_ENGINE

#include <assert.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdlib.h>
#include <limits.h>

#include <string>
#include <vector>

#include "../util/error.hh"

class Tester {
    public:
    static void runTest(std::vector<void(*)()> &tests, std::string testName, std::string logPath) {
        int logFD = open(logPath.c_str(), O_TRUNC | O_RDWR | O_CREAT, 0777);
        ERROR_KILL(logFD, "Tester open", exit);

        Tester::writeHeader(testName, logFD);

        // Absorb the error, so we can run many tests with this object
        for (auto& test : tests) {
            try {
                test();
            }
            catch (std::runtime_error& err) {
                printf("%s", err.what());
                Tester::logWrite(err.what(), logFD);
            }
        }

        int closeCheck = close(logFD);
        ERROR_KILL(closeCheck, "Tester close", exit);
    }

    static void logWrite(std::string logMessage, int logFD) {
        write(logFD, logMessage.c_str(), logMessage.length());
        write(logFD, "\n", 1);    
    }

    static void writeHeader(std::string header, int logFD) {
        write(logFD, header.c_str(), header.length());
        write(logFD, "\n", 1);
    }

    /* checkTrue()
     * val
     * testName: give the assert statement a name, so if it fails you can track it.
     *
     * Operates as an assert.isTrue(<boolean-value>)
     */
    static void checkTrue(bool val, std::string testName) {
        if (val) { return; }
        
        throw std::runtime_error("FAILED TRUTHY CHECK - " + testName);
    }

    /* checkFalse()
     * val
     * testName: give the assert statement a name, so if it fails you can track it.
     *
     * Operates as an assert.isFalse(<boolean-value>)
     */
    static void checkFalse(bool val, std::string testName) {
        if (!val) { return; }

        throw std::runtime_error("FAILED FALSY CHECK - " + testName);
    }

    /* checkError()
     * val
     * testName
     *
     * Checks if typical POSIX functions return less than zero, which
     * typically returns an error. Useful for checking things like loading
     * files.
     */
    static void checkError(int val, std::string testName) {
        if (val > 0) { return; }

        throw std::runtime_error("FAILED ERROR CHECK - " + testName);
    }
};

#endif
