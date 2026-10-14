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
    WriteError,
    InvalidPage
};

class Disk {
public:
    bool isOpen;
    int32 fd; /* File Descriptor of the instance's file */
    std::string filePath; /* file path of the DB file */
    int32 numPages; /* Pages in the file, valid ids are [0, numPages) */

    Disk(std::string filePath);
    ~Disk();

    DiskStatus checkPageId(int32 pageid);
    DiskStatus allocatePage(int32 *outId);
    
    DiskStatus openDB();
    DiskStatus createDB();
    DiskStatus closeDB();
    DiskStatus deleteDB();
    DiskStatus readPage(int32 pageid, byte *buf);
    DiskStatus writePage(int32 pageid, byte *buf);
};