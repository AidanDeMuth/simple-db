#include <simpledb.hh>
#include <assert.h>

std::string path = "test/test_db_files/test_disk.txt";

void testCreateOpenClose() {
    remove(path.c_str());
    Disk *disk = new Disk(path);
    DiskStatus create = disk->createDB();
    assert(create == DiskStatus::OK);

    assert(disk->createDB() == DiskStatus::FileAlreadyExists);

    struct stat st;
    assert(stat(disk->filePath.c_str(), &st) != -1);
    assert(st.st_size == PAGE_SIZE);

    DiskStatus open = disk->openDB();
    assert(open == DiskStatus::OK);
    assert(disk->openDB() == DiskStatus::FileAlreadyOpen);

    assert(disk->fd != SYSERR);

    /* Can't delete a file that's still open */
    assert(disk->deleteDB() == DiskStatus::FileAlreadyOpen);

    DiskStatus close = disk->closeDB();
    assert(close == DiskStatus::OK);

    DiskStatus remove = disk->deleteDB();
    assert(remove == DiskStatus::OK);
    assert(stat(disk->filePath.c_str(), &st) == -1);

    delete disk;
}

void testReadAndWrite() {
    remove(path.c_str());
    Disk *disk = new Disk(path);
    assert(disk->createDB() == DiskStatus::OK);
    assert(disk->openDB() == DiskStatus::OK);

    Page p;
    p.data[5] = 'A';
    assert(disk->writePage(0, p.getData()) == DiskStatus::OK);
    Page q;
    assert(disk->readPage(0, q.getData()) == DiskStatus::OK);
    assert(q.data[5] == 'A');
    // q.dump();

    /* Closed DB can't be read or written */
    disk->closeDB();
    assert(disk->readPage(0, q.getData()) == DiskStatus::FileNotOpen);
    assert(disk->writePage(0, p.getData()) == DiskStatus::FileNotOpen);

    disk->deleteDB();
    delete disk;
}

/* Only pages that exist can be read or written, and allocatePage()
 * is how new ones get made
 */
void testPageBounds() {
    remove(path.c_str());
    Disk *disk = new Disk(path);
    assert(disk->createDB() == DiskStatus::OK);

    Page p;
    assert(disk->checkPageId(0) == DiskStatus::FileNotOpen);

    assert(disk->openDB() == DiskStatus::OK);
    assert(disk->numPages == 1); /* createDB() makes one page */

    assert(disk->checkPageId(0) == DiskStatus::OK);
    assert(disk->checkPageId(1) == DiskStatus::InvalidPage);
    assert(disk->checkPageId(-1) == DiskStatus::InvalidPage);
    assert(disk->writePage(1, p.getData()) == DiskStatus::InvalidPage);
    assert(disk->readPage(-1, p.getData()) == DiskStatus::InvalidPage);

    // Bound check!!!!!!
    assert(disk->readPage(1 << 20, p.getData()) == DiskStatus::InvalidPage);

    int32 id;
    assert(disk->allocatePage(&id) == DiskStatus::OK);
    assert(id == 1);
    assert(disk->numPages == 2);
    assert(disk->writePage(1, p.getData()) == DiskStatus::OK);

    struct stat st;
    assert(stat(disk->filePath.c_str(), &st) != -1);
    assert(st.st_size == (PAGE_SIZE * 2));

    /* numPages is rebuilt from the file size on open */
    disk->closeDB();
    assert(disk->numPages == 0);
    assert(disk->openDB() == DiskStatus::OK);
    assert(disk->numPages == 2);

    disk->closeDB();
    disk->deleteDB();
    delete disk;
}

int main() {
    testCreateOpenClose();
    testReadAndWrite();
    testPageBounds();
}