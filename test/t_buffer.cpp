#include <simpledb.hh>
#include <assert.h>

std::string path = "test/test_db_files/test_buffer_rw";
std::string freePath = "test/test_db_files/test_buffer_frees";
std::string evictPath = "test/test_db_files/test_buffer_evict";

void testLRUCacheSimple() {
    LRUCache cache = LRUCache(3);
    assert(cache.capacity == 3);
    
    Frame *evict = nullptr;
    for (int i = 0; i < 3; i++) {
        assert(cache.set(new Frame(Key(i)), &evict));
        assert(evict == nullptr);
    }

    assert(cache.size == 3);

    for (int i = 0; i < 3; i++) {
        Frame *check = cache.get(Key(i));
        assert(check->key.pageid == i);
    }

    /* The cache doesn't own frames, so clean up here */
    for (int i = 0; i < 3; i++) {
        Frame *f = cache.get(Key(i));
        cache.remove(f);
        delete f;
    }
    assert(cache.size == 0);
}

void testLRUCacheReplacement() {
    LRUCache cache = LRUCache(100);

    Frame *evict = nullptr;
    for (int i = 0; i < 100; i++) {
        cache.set(new Frame(Key(i)), &evict);
    }
    for (int i = 100; i < 200; i++) {
        cache.set(new Frame(Key(i)), &evict);
        assert(evict->key.pageid == (i - 100));
        delete evict; /* set() hands the evicted frame back to us */
    }

    // Ensure old ones got replaced
    assert(cache.size == 100);
    for (int i = 0; i < 100; i++) {
        assert(cache.get(Key(i)) == nullptr);
    }
    for (int i = 100; i < 200; i++) {
        Frame *check = cache.get(Key(i));

        assert(check->key.pageid == i);
    }

    for (int i = 100; i < 200; i++) {
        Frame *f = cache.get(Key(i));
        cache.remove(f);
        delete f;
    }
}

void testPins() {
    remove(path.c_str()); /* Clear leftovers from a crashed run */
    Disk *disk = new Disk(path);
    assert(disk->createDB() == DiskStatus::OK);
    assert(disk->openDB() == DiskStatus::OK);

    /* createDB() makes page 0, allocate 1-3 */
    int32 id;
    for (int i = 1; i < 4; i++) {
        assert(disk->allocatePage(&id) == DiskStatus::OK);
        assert(id == i);
    }

    Page *page = new Page();
    page->writeByte(5, 'A');

    for (int i = 0; i < 4; i++) {
        assert(disk->writePage(i, page->getData()) == DiskStatus::OK);
    }

    {
        Buffer buffer = Buffer(3, *disk);
        Page *myPage;
        assert(buffer.pinPage(0, &myPage) == BufferStatus::OK);
        assert(myPage->readByte(5) == 'A');
        assert(buffer.pinPage(1, &myPage) == BufferStatus::OK);
        assert(myPage->readByte(5) == 'A');
        assert(buffer.pinPage(2, &myPage) == BufferStatus::OK);
        assert(buffer.pinPage(2, &myPage) == BufferStatus::OK);
        assert(myPage->readByte(5) == 'A');

        struct stat st;
        assert(stat(disk->filePath.c_str(), &st) != -1);
        assert(st.st_size == (PAGE_SIZE * 4));

        assert(buffer.pinPage(3, &myPage) == BufferStatus::AllPinned);
        assert(myPage->readByte(5) == 'A');

        assert(buffer.unpinPage(0) == BufferStatus::OK);

        assert(buffer.pinPage(3, &myPage) == BufferStatus::OK);
        assert(myPage->readByte(5) == 'A');

        assert(buffer.unpinPage(1) == BufferStatus::OK);
        assert(buffer.unpinPage(2) == BufferStatus::OK);
        assert(buffer.unpinPage(2) == BufferStatus::OK);
        assert(buffer.unpinPage(3) == BufferStatus::OK);

        /* Everything is unpinned now, one more is an error and changes nothing */
        assert(buffer.unpinPage(3) == BufferStatus::NegativePins);
        assert(buffer.cache.get(Key(3))->pinCount == 0);
    } /* ~Buffer() runs here, before the disk it references goes away */

    delete page;
    disk->closeDB();
    disk->deleteDB();
    delete disk;
}

void testBufferFrees() {
    remove(freePath.c_str()); /* Clear leftovers from a crashed run */
    Disk *disk = new Disk(freePath);
    assert(disk->createDB() == DiskStatus::OK);
    assert(disk->openDB() == DiskStatus::OK);

    /* createDB() makes page 0, allocate the rest */
    int32 id;
    for (int i = 1; i < 5; i++) {
        assert(disk->allocatePage(&id) == DiskStatus::OK);
    }

    Page *page = new Page();
    for (int i = 0; i < 5; i++) {
        assert(disk->writePage(i, page->getData()) == DiskStatus::OK);
    }

    {
        Buffer buffer = Buffer(3, *disk);
        Page *myPage;

        /* Pages 0 and 1 get removed, 2-4 are left in the cache */
        for (int i = 0; i < 5; i++) {
            assert(buffer.pinPage(i, &myPage) == BufferStatus::OK);
            assert(buffer.unpinPage(i) == BufferStatus::OK);
        }
        assert(buffer.cache.size == 3);

        /* Leave one pinned too, the destructor should still free it */
        assert(buffer.pinPage(4, &myPage) == BufferStatus::OK);
    } // buffer is removed

    delete page;
    disk->closeDB();
    disk->deleteDB();
    delete disk;
}

int main() {
    testLRUCacheSimple();
    testLRUCacheReplacement();
    testPins();
    testBufferFrees();
}