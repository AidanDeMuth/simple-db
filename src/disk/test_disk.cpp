#include "../util/error.hh"
#include "../util/test.hh"

#include "disk.hh"
#include "page.hh"

#include "../util/project_config.hh"

// Make sure a file can be opened, loaded, closed, and deleted
void testFileIO() {
    std::string dbFilePath = config::PROJECT_ROOT + "/test/test_db_files/test-file.sdb";

    disk::createFile(dbFilePath);
    test::checkTrue(disk::checkExistsDB(dbFilePath));

    // Load and close - make sure it's still a file after closing
    int fileDescriptor = disk::loadFile(dbFilePath);
    test::checkError(fileDescriptor);

    disk::closeFile(fileDescriptor);
    test::checkTrue(disk::checkExistsDB(dbFilePath));

    disk::deleteFile(dbFilePath);
    test::checkFalse(disk::checkExistsDB(dbFilePath));
} 

void testPages() {
    Page page1;

    unsigned char bytes[config::PAGE_SIZE] = {0};
    Page page2((void *) bytes);

    test::checkEqual((int) sizeof(page1), config::PAGE_SIZE);
    test::checkEqual((int) sizeof(page1), config::PAGE_SIZE);
    test::checkBufferEqual((void *) page1.getData(), (void *) page2.getData(), config::PAGE_SIZE);
    test::checkEqual(0, 1);
}

int main() {
    std::string logPath = config::PROJECT_ROOT + "/test/test_logs/test_init_db.txt";
    int logFD = open(logPath.c_str(), O_TRUNC | O_RDWR | O_CREAT, 0777);
    ERROR_KILL(logFD, "Tester open", exit);

    std::vector<void(*)()> tests = {
        testFileIO,
        testPages
    };
    test::runTest(
        tests, 
        "Test Creation, Deletion, Existence of files", 
        logFD
    );

    int closeCheck = close(logFD);
    ERROR_KILL(closeCheck, "Tester close", exit);
}