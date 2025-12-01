#include <fcntl.h>
#include <unistd.h>

#include <stdlib.h>
#include <limits.h>

#include <string>
#include <vector>
#include <stdexcept>

#include "./test.hh"
#include "../util/error.hh"

namespace test {
    /* runTest()
     * tests: A vector of test functions to be run
     * testName: Test group name
     * logPath: Filename to write logs to
     *
     * Writes to the logfile the results of this test
     */
    void runTest(void(* test)(), std::string testName, std::string logPath) {
        int logFD = open(logPath.c_str(), O_TRUNC | O_RDWR | O_CREAT, 0700);
        ERROR_KILL(logFD, "Tester open", exit);

        try {
            test();
        }
        catch (std::runtime_error& err) {
            printf("%s", err.what());
            logWrite(err.what(), logFD);
        }

        writeFooter("Tests Finished", logFD);

        int closeCheck = close(logFD);
        ERROR_KILL(closeCheck, "Tester close", exit);
    }
    
    /* runTest()
     * tests: A vector of test functions to be run
     * testName: Test group name
     * logFD: open file descriptor in which a log file should be written to
     */
    void runTest(void(* test)(), std::string testName, int logFD) {
        writeHeader(testName, logFD);

        // Absorb the error, so we can run many tests with this object
        try {
            test();
        }
        catch (std::runtime_error& err) {
            printf("%s", err.what());
            logWrite(err.what(), logFD);
        }
        

        writeFooter("Tests Finished", logFD);
    }

    void logWrite(std::string logMessage, int logFD) {
        write(logFD, logMessage.c_str(), logMessage.length());
        write(logFD, "\n", 1);    
    }

    void writeHeader(std::string header, int logFD) {
        write(logFD, header.c_str(), header.length());
        write(logFD, "\n", 1);
    }

    void writeFooter(std::string footer, int logFD) {
        write(logFD, footer.c_str(), footer.length());
        write(logFD, "\n\n", 2);
    }

    /* checkTrue()
     * val
     * testName: give the assert statement a name, so if it fails you can track it.
     *
     * Operates as an assert.isTrue(<boolean-value>)
     */
    void checkTrue(bool val) {
        if (val) { return; }
        
        throw std::runtime_error("FAILED TRUTHY CHECK");
    }

    /* checkFalse()
     * val
     * testName: give the assert statement a name, so if it fails you can track it.
     *
     * Operates as an assert.isFalse(<boolean-value>)
     */
    void checkFalse(bool val) {
        if (!val) { return; }

        throw std::runtime_error("FAILED FALSY CHECK");
    }

    /* checkError()
     * val
     * testName
     *
     * Checks if typical POSIX functions return less than zero, which
     * typically returns an error. Useful for checking things like loading
     * files.
     */
    void checkError(int val) {
        if (val > 0) { return; }

        throw std::runtime_error("FAILED ERROR CHECK");
    }

    /* checkBufferEqual()
     * buf1: output buffer to compare
     * buf2: control buffer
     * length: how long to compare the buffers
     *
     * It is often the case that we want to check if every byte of
     * a buffer is equal to every byte in another one, especially
     * with pages.
     */
    void checkBufferEqual(void *buf1, void *buf2, int length) {
        unsigned char *b1 = (unsigned char *) buf1; 
        unsigned char *b2 = (unsigned char *) buf2;

        for (int i = 0; i < length; i++) {
            if (b1[i] != b2[i]) {
                throw std::runtime_error("FAILED BUFFER CHECK");
            }
        }

        return;
    }

    void checkNumericEqual(int val1, int val2) {
        if (val1 == val2) { return; }

        std::string err = "GOT VALUE " + std::to_string(val1) + ", BUT EXPECTED " + std::to_string(val2);
        throw std::runtime_error("FAILED NUMERIC EQUALITY CHECK" + err);
    }

    void checkStringEqual(std::string val1, std::string val2) {
        if (val1 == val2) { return; }

        std::string err = "GOT VALUE " + val1 + ", BUT EXPECTED " + val2;
        throw std::runtime_error("FAILED STRING EQUALITY CHECK: " + err);
    }
}