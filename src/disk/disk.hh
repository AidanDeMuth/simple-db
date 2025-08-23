#ifndef DISK
#define DISK

#include <string>

namespace disk {
    bool checkExistsDB(std::string filePath);
    void createFile(std::string filePath);
    void deleteFile(std::string filePath);
    int loadFile(std::string filePath);
    void closeFile(int fileDesc);
}

#endif