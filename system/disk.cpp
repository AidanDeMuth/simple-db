#include <disk.hh>

/* Compute in 64 bits so that a page offset is always guaranteed
 * to not overflow
 */
static off_t pageOffset(int32 pageid) {
    return ((off_t) pageid) * PAGE_SIZE;
}

Disk::Disk(std::string filePath) {
    this->isOpen = false;
    this->filePath = filePath;
    this->fd = SYSERR; /* Default it to something invalid */
    this->numPages = 0;
}

Disk::~Disk() {
    if (this->fd != SYSERR) { close(this->fd); }
}

/* checkPageId()
 * pageid: page to validate
 *
 * A page is valid if the DB is open and the page exists in the file. New
 * pages must be created with allocatePage() before they can be read or written
 */
DiskStatus Disk::checkPageId(int32 pageid) {
    if (!this->isOpen) { return DiskStatus::FileNotOpen; }
    if (pageid < 0 || pageid >= this->numPages) { return DiskStatus::InvalidPage; }

    return DiskStatus::OK;
}

/* allocatePage()
 * outId: set to the id of the new page
 *
 * Only way to change the size of the file
 */
DiskStatus Disk::allocatePage(int32 *outId) {
    if (!this->isOpen) { return DiskStatus::FileNotOpen; }

    // Default a page, set it right at the end of the file
    byte buf[PAGE_SIZE] = {0};
    ssize_t bytes = pwrite(this->fd, buf, PAGE_SIZE, pageOffset(this->numPages));
    if (bytes != PAGE_SIZE) {
        perror("pwrite");
        return DiskStatus::WriteError;
    }

    *outId = this->numPages;
    this->numPages++;
    return DiskStatus::OK;
}

DiskStatus Disk::openDB() {
    if (this->isOpen) { return DiskStatus::FileAlreadyOpen; } // error already open?

    this->fd = open(this->filePath.c_str(), O_RDWR, 0666);
    if (this->fd == SYSERR) { 
        perror("open");
        return DiskStatus::IOError;
    }

    /* Work out how many pages the file already has so we know where to
       allocate the next ones */
    struct stat st;
    if (fstat(this->fd, &st) == SYSERR) {
        perror("fstat");
        close(this->fd);
        this->fd = SYSERR;
        return DiskStatus::IOError;
    }
    this->numPages = st.st_size / PAGE_SIZE;

    this->isOpen = true;
    return DiskStatus::OK;
}

DiskStatus Disk::createDB() {
    // Don't create if the file already exists
    if (access(this->filePath.c_str(), F_OK) != SYSERR) {
        // printf("\n%d\n", access(this->filePath.c_str(), F_OK));
        return DiskStatus::FileAlreadyExists;
    }
    
    // Create a new, empty file with r/w
    int32 newFd = open(this->filePath.c_str(), O_RDWR | O_CREAT, 0666);
    if (newFd == SYSERR) {
        perror("open");
        return DiskStatus::IOError;
    }
    close(newFd);

    // Formally open so we can allocate a page
    DiskStatus openCheck = openDB();
    if (openCheck != DiskStatus::OK) { return openCheck; }

    int32 initPage;
    DiskStatus pageCheck = allocatePage(&initPage);
    closeDB();

    return pageCheck;
}

DiskStatus Disk::closeDB() {
    if (this->fd != SYSERR) { close(this->fd); }

    this->fd = SYSERR;
    this->isOpen = false;
    this->numPages = 0;

    return DiskStatus::OK;
}

DiskStatus Disk::deleteDB() {
    if (this->isOpen) { return DiskStatus::FileAlreadyOpen; }

    if (remove(this->filePath.c_str()) == SYSERR) {
        return DiskStatus::IOError;
    }

    return DiskStatus::OK;
}

DiskStatus Disk::readPage(int32 pageid, byte *buf) {
    DiskStatus check = this->checkPageId(pageid);
    if (check != DiskStatus::OK) { return check; }

    ssize_t bytes = pread(this->fd, buf, PAGE_SIZE, pageOffset(pageid));

    if (bytes == SYSERR) { return DiskStatus::IOError; }

    if (bytes < PAGE_SIZE) {
        return DiskStatus::IOError;
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
    DiskStatus check = this->checkPageId(pageid);
    if (check != DiskStatus::OK) { return check; }

    ssize_t bytes = pwrite(this->fd, buf, PAGE_SIZE, pageOffset(pageid));

    if (bytes == SYSERR) {
        perror("pwrite");
        return DiskStatus::WriteError;
    }

    return DiskStatus::OK;
}