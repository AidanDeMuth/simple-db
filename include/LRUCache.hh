#pragma once

#include <typedefs.hh>
#include <heapPage.hh>

constexpr int32 HT_SIZE = 100;
constexpr int32 HT_MODULO = 100;

struct Frame {
    int32 pageid;
    HeapPage *heapPage;

    /* PREV and NEXT pointers for the LINKED LIST */
    Frame *prev;
    Frame *next;

    /* CHAIN POINTERS for the HASHTABLE */
    Frame *bucketNext;
};

class LL {
public:
    Frame *head;
    Frame *tail;

    LL();
    ~LL();

    void insertHead(Frame *Frame);
    void moveToHead(Frame *Frame);
    Frame *removeTail();

    void dump();
};

class HT {
public:
    Frame *table[HT_SIZE];

    HT();
    ~HT();

    /* Static method to get the table index hashed by the page id */
    static int32 getHash(int32 pageid) {
        return pageid % HT_MODULO;
    }

    Frame *find(int32 pageid);
    void insert(Frame *Frame);
    void remove(int32 pageid);
    

    void dump();
};

class LRUCache {
public:
    LL ll;
    HT ht;

    int32 size;
    int32 capacity;

    LRUCache(int32 capacity);
    ~LRUCache();

    HeapPage *get(int32 pageid);
    void set(int32 pageid, HeapPage *heapPage);

    void dump();
};