#pragma once

#include <page.hh>

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <string>
#include <cstring>
#include <stdexcept>

enum class DiskStatus {
    OK,
    FileNotOpen,
    FileAlreadyExists,
    FileAlreadyOpen,
    IOError,
    ReadError,
    WriteError
};

class Disk {
public:
    bool isOpen;
    int32 fd; /* File Descriptor of the instance's file */
    std::string filePath; /* file path of the DB file */

    Disk(std::string filePath);
    ~Disk();

    void checkPageId(int32 pageid);
    
    DiskStatus openDB();
    DiskStatus createDB();
    DiskStatus closeDB();
    DiskStatus deleteDB();
    DiskStatus readPage(int32 pageid, byte *buf);
    DiskStatus writePage(int32 pageid, byte *buf);
};