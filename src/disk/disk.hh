#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace disk {
    bool checkExistsFile(std::string filePath);
    void createFile(std::string filePath);
    void deleteFile(std::string filePath);
}