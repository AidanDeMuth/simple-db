#include <simpledb.hh>
#include <assert.h>

std::string path = "test/test_db_files/simple-scan";

/* Wire pages 0 -> 1 -> 2 together on disk */
void writeChain(Disk *disk) {
    /* createDB() makes page 0, allocate 1 and 2 */
    int32 id;
    assert(disk->allocatePage(&id) == DiskStatus::OK);
    assert(disk->allocatePage(&id) == DiskStatus::OK);

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
    delete page;
}

void testSimpleScan() {
    remove(path.c_str()); /* Clear leftovers from a crashed run */
    Disk *disk = new Disk(path);
    assert(disk->createDB() == DiskStatus::OK);
    assert(disk->openDB() == DiskStatus::OK);

    /* Create and wire together three pages on disk, iterate over them
     * and terminate the scan. Make sure that when the scan is finished,
     * all pages are unpinned
     */
    writeChain(disk);

    {
        Buffer buffer = Buffer(3, *disk);

        /* Open a scan, then validate that all of the pages were found */
        HeapScan *scan = new HeapScan(0, buffer);

        assert(scan->currid == 0);
        assert(scan->startScan() == BufferStatus::OK);
        assert(scan->currPage->readPageID() == 0);

        int pagesSeen = 1;
        for (int i = 1; scan->hasNext(); i++) {
            assert(scan->next() == BufferStatus::OK);
            assert(scan->currPage->readPageID() == i);
            pagesSeen++;
        }
        assert(pagesSeen == 3);

        assert(scan->endScan() == BufferStatus::OK);
        assert(scan->currPage == nullptr);
        assert(scan->endScan() == BufferStatus::OK); /* second call is harmless */

        for (int i = 0; i < 3; i++) {
            assert(buffer.cache.get(Key(i))->pinCount == 0);
        }

        delete scan;
    } /* ~Buffer() runs here, before the disk it references goes away */

    disk->closeDB();
    disk->deleteDB();
    delete disk;
}

/* A scan that's never ended should give its pin back when destroyed */
void testScanDestructorUnpins() {
    remove(path.c_str()); /* Clear leftovers from a crashed run */
    Disk *disk = new Disk(path);
    assert(disk->createDB() == DiskStatus::OK);
    assert(disk->openDB() == DiskStatus::OK);
    writeChain(disk);

    {
        Buffer buffer = Buffer(3, *disk);

        HeapScan *scan = new HeapScan(0, buffer);
        assert(scan->startScan() == BufferStatus::OK);
        assert(scan->next() == BufferStatus::OK);
        assert(buffer.cache.get(Key(1))->pinCount == 1);

        delete scan; /* no endScan() */
        assert(buffer.cache.get(Key(0))->pinCount == 0);
        assert(buffer.cache.get(Key(1))->pinCount == 0);
    }

    disk->closeDB();
    disk->deleteDB();
    delete disk;
}

/* Starting on a page that doesn't exist fails cleanly, with nothing pinned */
void testScanInvalidStart() {
    remove(path.c_str()); /* Clear leftovers from a crashed run */
    Disk *disk = new Disk(path);
    assert(disk->createDB() == DiskStatus::OK);
    assert(disk->openDB() == DiskStatus::OK);

    {
        Buffer buffer = Buffer(3, *disk);

        HeapScan *scan = new HeapScan(7, buffer);
        assert(scan->startScan() == BufferStatus::InvalidPage);
        assert(scan->currPage == nullptr);
        assert(!scan->hasNext());
        assert(scan->next() == BufferStatus::InvalidState);
        assert(buffer.cache.size == 0);

        delete scan;
    }

    disk->closeDB();
    disk->deleteDB();
    delete disk;
}

int main() {
    testSimpleScan();
    testScanDestructorUnpins();
    testScanInvalidStart();
}