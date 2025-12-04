#include <simpledb.hh>
#include <assert.h>

void testLRUCacheSimple() {
    LRUCache cache = LRUCache(3);
    assert(cache.capacity == 3);
    
    for (int i = 0; i < 3; i++) {
        cache.set(new Frame(Key(i), new HeapPage()));
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
        cache.set(new Frame(Key(i), new HeapPage()));
    }
    // Ensure old ones got replaced
    assert(cache.size == 100);
    for (int i = 100; i < 200; i++) {
        Frame *check = cache.get(Key(i));
        assert(check->key.pageid == i);
    }
}

int main() {
    testLRUCacheSimple();
    testLRUCacheReplacement();
}