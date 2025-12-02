#include <buffer.hh>

void testLRUCache() {
    LRUCache cache = LRUCache(100);
    cache.set(new Frame(Key(1), new HeapPage()));
    cache.set(new Frame(Key(2), new HeapPage()));
    cache.set(new Frame(Key(102), new HeapPage()));
    cache.get(Key(2));
    // cache.dump();
}

void testBuffer() {
    Buffer *buf = new Buffer(100);
    buf->pinPage(Key(1));
    // buf->dump();


    buf->unpinPage(Key(1));
    // buf->dump();
}

int main() {
    testLRUCache();
    testBuffer();
}