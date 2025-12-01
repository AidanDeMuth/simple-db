#include <unistd.h>
#include <fcntl.h>
#include <iostream>

// Chec exists dir and create dir
#include <sys/types.h>
#include <sys/stat.h>

// CPP
#include <string>
#include <stdexcept>

#include "disk.hh"
#include "../util/error.hh"

namespace disk {
    // maybe replace with this
    //
    //if (stat().c_str(), &base) != -1) {
         //   throw std::runtime_error("Project creation error: project path already exists!");
       // }
    bool checkExistsFile(std::string filePath) {
        return (access(filePath.c_str(), F_OK) != -1);
    }

    void createFile(std::string filePath) {
        if (checkExistsFile(filePath)) {
            throw std::runtime_error("File creation error: file already exists!");
        }

        int newFD = open(filePath.c_str(), O_CREAT, 0700);
        ERROR_THROW(newFD, std::runtime_error, "Failed to open new file!");

        int closeCheck = close(newFD);
        ERROR_KILL(closeCheck, "createFile close error", exit);
    }

    void deleteFile(std::string filePath) {
        if (!checkExistsFile(filePath)) {
            throw std::runtime_error("File deletion error: file does not exist!");
        }

        int removeError = remove(filePath.c_str());
        ERROR_THROW(removeError, std::runtime_error, "Failed to remove file!");
    }

    
}