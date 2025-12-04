#include <simpledb.hh>
#include <assert.h>

void testCreateOpenClose() {
    Disk *disk = new Disk("/home/ec2-user/simple-db/test/test_db_files/test_disk.txt");
    DiskStatus create = disk->createDB();
    assert(create == DiskStatus::OK);

    struct stat st;
    if (stat(disk->filePath.c_str(), &st) == -1) { assert(1 == 2); }
    assert(st.st_size == PAGE_SIZE);

    DiskStatus open = disk->openDB();
    assert(open == DiskStatus::OK);

    assert(disk->fd != SYSERR);

    DiskStatus close = disk->closeDB();
    assert(close == DiskStatus::OK);

    DiskStatus remove = disk->deleteDB();
    assert(remove == DiskStatus::OK);
}

void testReadAndWrite() {
    Disk *disk = new Disk("/home/ec2-user/simple-db/test/test_db_files/test_disk.txt");
    disk->createDB();
    disk->openDB();

    Page p;
    p.buf[5] = 'A';
    disk->writePage(0, p.buf);
    Page q;
    disk->readPage(0, q.buf);
    assert(q.buf[5] == 'A');
    // q.dump();

    disk->closeDB();
    disk->deleteDB();
}

int main() {
    testCreateOpenClose();
    testReadAndWrite();
}