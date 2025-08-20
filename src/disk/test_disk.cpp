#include "disk.hh"
#include "page.hh"

#include "../util/error.hh"
#include "../util/test.hh"

#include "../util/project_config.hh"

// Make sure a file can be opened/closed
void testDisk() {
    std::string dbFilePath = config::PROJECT_ROOT + "/test/test_db_files/test-file.sdb";

    createFile(dbFilePath);
    Tester::checkTrue(checkExistsDB(dbFilePath), "File is valid after creation");

    // Load and close - make sure it's still a file after closing
    int fileDescriptor = loadFile(dbFilePath);
    Tester::checkError(fileDescriptor, "File can open after creation");

    closeFile(fileDescriptor);
    Tester::checkTrue(checkExistsDB(dbFilePath), "File is valid after closing");

    deleteFile(dbFilePath);
    Tester::checkFalse(checkExistsDB(dbFilePath), "File is invalid after deletion");
} 

int main() {
    std::vector<void(*)()> tests = {
        testDisk
    };
    Tester::runTest(
        tests, 
        "Test Creation, Deletion, Existence of files", 
        config::PROJECT_ROOT + "/test/test_logs/test_init_db.txt"
    );
}