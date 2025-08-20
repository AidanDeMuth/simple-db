#include <unistd.h>
#include <fcntl.h>
#include <iostream>

#include <string>

#include "page.hh"
#include "../util/error.hh"

bool checkExistsDB(std::string filePath) {
    return (access(filePath.c_str(), F_OK) != -1);
}

void createFile(std::string filePath) {
    if (checkExistsDB(filePath)) {
        throw std::runtime_error("File creation error: file already exists!");
    }

    int newFD = open(filePath.c_str(), O_CREAT, 0777);
    ERROR_THROW(newFD, std::runtime_error, "Failed to open new SimpleDB file!");

    int closeCheck = close(newFD);
    ERROR_KILL(closeCheck, "close", exit);
}

void deleteFile(std::string filePath) {
    if (!checkExistsDB(filePath)) {
        throw std::runtime_error("File deletion error: file does not exist!");
    }

    int removeError = remove(filePath.c_str());
    ERROR_THROW(removeError, std::runtime_error, "Failed to remove SimpleDB file!");
}

/* loadFile()
 * 
 * Given a filepath, will return an open file descriptor
 */
int loadFile(std::string filePath) {
    if (!checkExistsDB(filePath)) {
        throw std::runtime_error("File open error: file does not exist!");
    }

    int loaded = open(filePath.c_str(), O_RDWR, 0777);

    ERROR_THROW(loaded, std::runtime_error, "Failed to open SimpleDB file!");

    return loaded;
}

/* closeFile()
 *
 * Will close an open file descriptor. An error is thrown if it cannot be closed,
 * indicating a critical error that should be killed.
 */
void closeFile(int fileDesc) {
    int closeCheck = close(fileDesc);
    ERROR_KILL(closeCheck, "close", exit);
}