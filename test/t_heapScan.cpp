#include <simpledb.hh>
#include <assert.h>

void testSimpleScan() {
    Disk *disk = new Disk("/home/ec2-user/simple-db/test/test_db_files/simple-scan");
    Buffer buffer = Buffer(3, *disk);

    disk->createDB();
    disk->openDB();

    /* Create and wire together three pages on disk, iterate over them 
     * and terminate the scan. Make sure that when the scan is finished, 
     * all pages are unpinned
     */
    
    Page *page = new Page();
    page->writeByte(50, 'A');
    page->writePageID(0);
    page->writeNextID(1);
    assert(disk->writePage(0, page->getData()) == DiskStatus::OK);
    page->writePageID(1);
    page->writeNextID(2);
    assert(disk->writePage(1, page->getData()) == DiskStatus::OK);
    page->writePageID(2);
    page->writeNextID(-1);
    assert(disk->writePage(2, page->getData()) == DiskStatus::OK);

    /* Open a scan, then validate that all of the pages were found */
    HeapScan *scan = new HeapScan(0, buffer);

    assert(scan->currid == 0);
    assert(scan->startScan() == BufferStatus::OK);
    assert(scan->currPage->readPageID() == 0);

    for (int i = 1; scan->hasNext(); i++) {
        assert(scan->next() == BufferStatus::OK);
        assert(scan->currPage->readPageID() == i);
    }

    scan->endScan();

    buffer.dump();

    disk->closeDB();
    disk->deleteDB();
}

int main() {
    testSimpleScan();
}