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
    printf("Past new...");
    int closeCheck = close(newFD);
    ERROR_KILL(closeCheck, "close", exit);
    printf("Past close!...");


}

void deleteFile(std::string filePath) {
    if (!checkExistsDB(filePath)) {
        throw std::runtime_error("File deletion error: file does not exist!");
    }

    int removeError = remove(filePath.c_str());
    printf("remove code: %d", removeError);
    ERROR_THROW(removeError, std::runtime_error, "Failed to remove SimpleDB file!");
}

void openDB(std::string filePath) {
    if (!checkExistsDB(filePath))  {
        throw std::runtime_error("Cannot open file that does not exist!");
    }
}