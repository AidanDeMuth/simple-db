#pragma once

#include <typedefs.hh>
#include <page.hh>

constexpr int32 HT_SIZE = 100;
constexpr int32 HT_MODULO = 100;

/* Key
 *
 * The key struct is a grouping of unique identifiers for a frame. They
 * are seperated so that they can be easily compared and hashed.
 */
struct Key {
    int32 pageid;

    Key(int32 pid = -1) : pageid(pid) {}

    bool equals(Key other) {
        return pageid == other.pageid;
    }

    int32 getHash() {
        return pageid % HT_MODULO;
    }
    
    void print() {
        printf("pageid: %d\n", pageid);
    }
};

/* Frame
 *
 * The grouping of a heap page and its metadata to be stored in the buffer. Uniquely
 * identified by the key.
 */
struct Frame {
    Key key;
    Page *page;

    bool dirty; /* Dirty bit, described whether a page has been written to or not in the cache */
    int32 pinCount; /* The number of active transactions using the page- essentially > 0 means DO NOT EVICT ME */

    /* PREV and NEXT pointers for the LINKED LIST */
    Frame *prev;
    Frame *next;

    /* CHAIN POINTERS for the HASHTABLE */
    Frame *bucketNext;

    Frame() :
        key(Key(-1)),
        page(nullptr),
        dirty(0),
        pinCount(0),
        prev(nullptr),
        next(nullptr),
        bucketNext(nullptr) {}
    Frame(Key k, Page *p) :
        key(k),
        page(p),
        dirty(0),
        pinCount(0),
        prev(nullptr),
        next(nullptr),
        bucketNext(nullptr) {}

    // Heap page no longer needed
    ~Frame() {
        delete page;
    }

    void print() { // Debug
        printf("Key: "); key.print();
        printf("HP ptr: %p\t dirty: %d\t pin count: %d\n", page, dirty, pinCount);
    }
};

class LL {
public:
    Frame *head;
    Frame *tail;

    LL();
    ~LL();

    void insertHead(Frame *frame);
    void moveToHead(Frame *frame);
    Frame *removeTail();

    void dump();
};

class HT {
public:
    Frame *table[HT_SIZE];

    HT();
    ~HT();


    Frame *find(Key key);
    void insert(Frame *frame);
    void remove(Key key);
    

    void dump();
};

/* class LRUCache()
 *
 * 
 */
class LRUCache {
public:
    // I think these should be direct members so
    // they are less annoying
    LL ll;
    HT ht;

    int32 size;
    int32 capacity;

    LRUCache(int32 capacity);
    ~LRUCache();

    Frame *get(Key key);
    Frame *set(Frame *frame);

    void dump();
};

/* class Buffer()
 *
 * Wraps LRU cache and provides the API of the buffer manager. This class
 * MANAGES MEMORY, as it is responsible for the allocation of Frames and
 * heap pages, as well as their deletion.
 *
 * 
 *
 */
class Buffer {
public:
    LRUCache cache;

    Buffer(int32 capacity);
    ~Buffer();

    Page *pinPage(Key key);
    void markDirty(Key key);
    void unpinPage(Key key);

    void dump();
};