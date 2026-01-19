#include <disk.hh>

Disk::Disk(std::string filePath) {
    this->isOpen = false;
    this->filePath = filePath;
}

Disk::~Disk() {
    if (this->fd != SYSERR) { close(this->fd); }
}

void Disk::checkPageId(int32 pageid) {
    /* Some condition to validate bounds */
}

DiskStatus Disk::openDB() {
    if (this->isOpen) { return DiskStatus::FileAlreadyOpen; } // error already open?

    this->fd = open(this->filePath.c_str(), O_RDWR, 0666);
    if (this->fd == SYSERR) { 
        perror("open");
        return DiskStatus::IOError;
    }

    this->isOpen = true;
    return DiskStatus::OK;
}

DiskStatus Disk::createDB() {
    // Don't create if the file already exists
    if (access(this->filePath.c_str(), F_OK) != SYSERR) {
        // printf("\n%d\n", access(this->filePath.c_str(), F_OK));
        return DiskStatus::FileAlreadyExists;
    }
    
    // Create a new file with READ/WRITE
    this->fd = open(this->filePath.c_str(), O_RDWR | O_CREAT, 0666);
    if (this->fd == SYSERR) {
        perror("open");
        return DiskStatus::IOError;
    }

    /* INIT */
    byte buf[PAGE_SIZE] = {0};
    pwrite(this->fd, buf, PAGE_SIZE, 0);
    /* OTHER INIT STUFF???? */

    close(this->fd);
    return DiskStatus::OK;
}

DiskStatus Disk::closeDB() {
    if (this->fd != -1) { close(this->fd); }

    this->fd = SYSERR;
    this->isOpen = false;

    return DiskStatus::OK;
}

// TODO
DiskStatus Disk::deleteDB() {
    if (this->isOpen) { return DiskStatus::FileAlreadyOpen; }

    if (remove(this->filePath.c_str()) == SYSERR) {
        return DiskStatus::IOError;
    }

    return DiskStatus::OK;
}

DiskStatus Disk::readPage(int32 pageid, byte *buf) {
    if (!this->isOpen) { return DiskStatus::FileNotOpen; }

    int32 bytes = pread(this->fd, buf, PAGE_SIZE, (pageid * PAGE_SIZE));

    if (bytes == SYSERR) { return DiskStatus::IOError; }

    if (bytes < PAGE_SIZE) {
        // Corrupted file?
        // kill(1); // Not sure what to do about this, honestly need to keep thinking
    }

    return DiskStatus::OK;
}

/* writePage()
 * 
 * pwrite is NOT safe in that it can increase the file size if a large input
 * is given. We should verify that the pageid is within the number of pages 
 * managed by the database.
 */
DiskStatus Disk::writePage(int32 pageid, byte *buf) {
    if (!this->isOpen) { return DiskStatus::FileNotOpen; }

    int32 bytes = pwrite(this->fd, buf, PAGE_SIZE, (pageid * PAGE_SIZE));

    if (bytes == SYSERR) {
        perror("pwrite");
        return DiskStatus::WriteError;
    }

    return DiskStatus::OK;
}