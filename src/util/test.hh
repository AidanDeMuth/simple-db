#ifndef TEST
#define TEST

#include <assert.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdlib.h>
#include <limits.h>

#include <string>
#include <vector>

#include "../util/error.hh"

namespace test{
    void runTest(std::vector<void(*)()> &tests, std::string testName, int logFD);
    void logWrite(std::string logMessage, int logFD);
    void writeHeader(std::string header, int logFD);
    void writeFooter(std::string footer, int logFD);

    void checkTrue(bool val, std::string testName);
    void checkFalse(bool val, std::string testName);
    void checkError(int val, std::string testName);
    template <typename T> void checkEqual(T val, T expected);
    void checkBufferEqual(void *buf1, void *buf2, int length);
    
    /* runTest()
     * tests: A vector of test functions to be run
     * testName: Test group name
     * logFD: open file descriptor in which a log file should be written to
     */
    void runTest(std::vector<void(*)()> &tests, std::string testName, int logFD) {
        writeHeader(testName, logFD);

        // Absorb the error, so we can run many tests with this object
        for (auto& test : tests) {
            try {
                test();
            }
            catch (std::runtime_error& err) {
                printf("%s", err.what());
                logWrite(err.what(), logFD);
            }
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

    /* checkEqual()
     * val: the value we assert to be equal
     * expected: static value we desire
     *
     * Generalized comparison function, asserts that the first value
     * is equal to the second one.
     */
    template <typename T>
    void checkEqual(T val, T expected) {
        if (val == expected) { return; }

        throw std::runtime_error("FAILED EQUALITY CHECK");
    }

    /* checkBuffers()
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
};

#endif
