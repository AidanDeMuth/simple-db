#include "disk.hh"

#include "../util/error.hh"
#include "../util/test.hh"

void testCreateAndDelete(Tester test, std::string testName) {
    test.writeHeader(testName);

    std::string dbFilePath = "/test/test_db_files/test-file.sdb";

    createFile(dbFilePath);
    test.checkTrue(checkExistsDB(dbFilePath), "File is valid after creation");

    printf("%d", checkExistsDB(dbFilePath));
   
    deleteFile(dbFilePath);
    test.checkFalse(checkExistsDB(dbFilePath), "File is invalid after deletion");
}

int main() {
    Tester test("test/test_logs/test_init_db.txt");
    test.startTest();

    try {
        testCreateAndDelete(test, "Test the log!");
    }
    catch (std::runtime_error err) {
        printf("%s", err.what());
    }
    printf("Ending the log");

    test.endTest();

    // createDB("./db_files/test");
}