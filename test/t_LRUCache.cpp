#include <simpledb.hh>

void testLL() {
    LL *ll = new LL();
    LLNode t1 = {1, new HeapPage(), nullptr, nullptr}; 
    LLNode t2 = {2, new HeapPage(), nullptr, nullptr};
    LLNode t3 = {3, new HeapPage(), nullptr, nullptr};
    ll->insertHead(&t1);
    ll->insertHead(&t2);
    ll->insertHead(&t3);
    ll->moveToHead(&t2);
    ll->dump();

    ll->removeTail();
    ll->removeTail();
    ll->dump();
}  

void testHT() {
    HT *ht = new HT();
    LLNode t1 = {1, new HeapPage(), nullptr, nullptr}; 
    LLNode t2 = {2, new HeapPage(), nullptr, nullptr};
    LLNode t3 = {102, new HeapPage(), nullptr, nullptr};

    ht->insert(&t1);
    ht->insert(&t2);
    ht->insert(&t3);

    ht->dump();

    ht->remove(2);
    ht->dump();
}

void testLRUCache() {
    LRUCache *cache = new LRUCache(100);

    cache->set(1, new HeapPage());
    cache->set(2, new HeapPage());
    cache->set(3, new HeapPage());
    cache->set(103, new HeapPage());
    cache->set(203, new HeapPage());

    cache->dump();

    cache->get(3);
    cache->dump();
}

int main() {
    
    // testLL();
    // testHT();
    testLRUCache();
}