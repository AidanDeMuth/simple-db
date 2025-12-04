#include <simpledb.hh>
#include <assert.h>

void testLRUCacheSimple() {
    LRUCache cache = LRUCache(3);
    assert(cache.capacity == 3);
    
    for (int i = 0; i < 3; i++) {
        cache.set(new Frame(Key(i)));
    }

    assert(cache.size == 3);

    for (int i = 0; i < 3; i++) {
        Frame *check = cache.get(Key(i));
        assert(check->key.pageid == i);
    }
}

void testLRUCacheReplacement() {
    LRUCache cache = LRUCache(100);
    for (int i = 0; i < 200; i++) {
        cache.set(new Frame(Key(i)));
    }
    // Ensure old ones got replaced
    assert(cache.size == 100);
    for (int i = 100; i < 200; i++) {
        Frame *check = cache.get(Key(i));
        assert(check->key.pageid == i);
    }
}

void testReadWriteFlush() {
    Disk *disk = new Disk("/home/ec2-user/simple-db/test/test_db_files/test_buffer_rw");
    Buffer buffer = Buffer(3, *disk);

    disk->createDB();
    disk->openDB();

    Page *page = new Page();
    page->writeByte(5, 'A');

    assert(disk->writePage(0, page->getData()) == DiskStatus::OK);
    disk->writePage(1, page->getData());
    disk->writePage(2, page->getData());
    disk->writePage(3, page->getData());

    Page *myPage;
    assert(buffer.pinPage(0, &myPage) == BufferStatus::OK);
    assert(myPage->readByte(5) == 'A');
    assert(buffer.pinPage(1, &myPage) == BufferStatus::OK);
    assert(myPage->readByte(5) == 'A');
    assert(buffer.pinPage(2, &myPage) == BufferStatus::OK);
    assert(buffer.pinPage(2, &myPage) == BufferStatus::OK);
    assert(myPage->readByte(5) == 'A');

    struct stat st;
    if (stat(disk->filePath.c_str(), &st) == -1) { assert(1 == 2); }
    assert(st.st_size == (PAGE_SIZE * 4));

    assert(buffer.pinPage(3, &myPage) == BufferStatus::OK);
    assert(myPage->readByte(5) == 'A');

    assert(buffer.unpinPage(1) == BufferStatus::OK);
    assert(buffer.unpinPage(2) == BufferStatus::OK);
    assert(buffer.unpinPage(2) == BufferStatus::OK);
    assert(buffer.unpinPage(3) == BufferStatus::OK);

    // buffer.dump();
    
    disk->closeDB();
    disk->deleteDB();
}

int main() {
    testLRUCacheSimple();
    testLRUCacheReplacement();
    testReadWriteFlush();
}