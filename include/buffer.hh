#pragma once

#include <typedefs.hh>

#include <page.hh>
#include <disk.hh>

constexpr int32 HT_SIZE = 100;
constexpr int32 HT_MODULO = 100;

/* Error codes for the Buffer - Useful, given there are many reasons for failure */
enum class BufferStatus {
    OK,
    IOError,
    NegativePins,
    NotFound,
    AllPinned
};

/* Key
 *
 * The key struct is a grouping of unique identifiers for a frame. Provides functionality
 * for hashing and comparing.
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

    bool dirty;     /* Dirty bit, described whether a page has been written to or not in the cache */
    int32 pinCount; /* The number of active transactions using the page- essentially > 0 means DO NOT EVICT ME */

    /* PREV and NEXT pointers for the LINKED LIST */
    Frame *prev;
    Frame *next;

    /* BUCKET CHAIN pointers for the HASHTABLE */
    Frame *bucketNext;

    Frame() :
        key(Key(-1)),
        page(new Page()),
        dirty(0),
        pinCount(0),
        prev(nullptr),
        next(nullptr),
        bucketNext(nullptr) {}
    Frame(Key k) :
        key(k),
        page(new Page()),
        dirty(0),
        pinCount(0),
        prev(nullptr),
        next(nullptr),
        bucketNext(nullptr) {}

    // Heap page no longer needed
    ~Frame() {
        delete page;
    }

    void print() {
        printf("Key: "); key.print();
        printf("HP ptr: %p\t dirty: %d\t pin count: %d\n", page, dirty, pinCount);
    }
};

/* LL()
 *
 * Linked List class for the LRU Cache
 */
class LL {
public:
    Frame *head;
    Frame *tail;

    LL();
    ~LL();

    void insertHead(Frame *frame);
    void moveToHead(Frame *frame);
    Frame *evict();

    void dump();
};

/* HT()
 *
 * Hashtable class for the LRU Cache
 */
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
 * Implementation of an LRU cache, wrapping classes LL and HT
 */
class LRUCache {
public:
    LL ll;
    HT ht;

    int32 size;
    int32 capacity;

    LRUCache(int32 capacity);
    ~LRUCache();

    Frame *get(Key key);
    bool set(Frame *frame, Frame **evict);

    void dump();
};

/* class Buffer()
 *
 * Wraps LRU cache and provides the API of the buffer manager. This class
 * MANAGES MEMORY, as it is responsible for the allocation and deletion of 
 * Frames
 */
class Buffer {
public:
    LRUCache cache;
    Disk &disk;

    Buffer(int32 capacity, Disk &disk);
    ~Buffer();

    BufferStatus pinPage(Key key, Page **outPage);
    void markDirty(Key key);
    BufferStatus unpinPage(Key key);

    void dump();
};