#pragma once

#include <typedefs.hh>
#include <heapPage.hh>

constexpr int32 HT_SIZE = 100;
constexpr int32 HT_MODULO = 100;

struct LLNode {
    int32 pageid;
    HeapPage *heapPage;

    /* PREV and NEXT pointers for the LINKED LIST */
    LLNode *prev;
    LLNode *next;

    /* CHAIN POINTERS for the HASHTABLE */
    LLNode *bucketNext;
};

class LL {
public:
    LLNode *head;
    LLNode *tail;

    LL();
    ~LL();

    void insertHead(LLNode *llnode);
    void moveToHead(LLNode *llnode);
    LLNode *removeTail();

    void dump();
};

class HT {
public:
    LLNode *table[HT_SIZE];

    HT();
    ~HT();

    /* Static method to get the table index hashed by the page id */
    static int32 getHash(int32 pageid) {
        return pageid % HT_MODULO;
    }

    LLNode *find(int32 pageid);
    void insert(LLNode *llnode);
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