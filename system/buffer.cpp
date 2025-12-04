#include <buffer.hh>

// LINKED LIST FUNCTIONS

LL::LL() {
    head = nullptr;
    tail = nullptr;
}

LL::~LL() {}

void LL::insertHead(Frame *frame) {
    frame->prev = nullptr;
    frame->next = head;

    if (head) { head->prev = frame; }
    head = frame;

    // If first entry, need to adjust the tail
    if (!tail) { tail = frame; }
}

void LL::moveToHead(Frame *frame) {
    // Can be from arbitrary location
    if (frame == head) { return; }

    // Now, node is in [second element, tail], so detach
    if (frame->prev) { frame->prev->next = frame->next; }
    if (frame->next) { frame->next->prev = frame->prev; }
    if (frame == tail) { tail = frame->prev; }

    frame->prev = nullptr;
    frame->next = head;
    if (head) head->prev = frame;
    head = frame; 
}

Frame *LL::removeTail() {
    if (!tail) { return nullptr; }

    Frame *ret = tail; // save reference

    // we do have a tail, unlink it and set head/tail correctly
    // otherwise
    if (tail->prev) { tail->prev->next = nullptr; }
    else { head = nullptr; }

    tail = tail->prev;
    return ret;
}

void LL::dump() {
    Frame *iterate = head;
    
    printf("Dumping linked list: PID, HeapPage\n");
    while (iterate) {
        iterate->print();
        
        iterate = iterate->next;
    }
    printf("=================\n\n");
}

// HASHTABLE FUNCTIONS

HT::HT() {
    for (int i = 0; i < HT_SIZE; i++) {
        table[i] = nullptr;
    }
}

HT::~HT() {}

Frame *HT::find(Key key) {
    int32 index = key.getHash();

    Frame *iterate = table[index];
    while (iterate) {
        if (iterate->key.equals(key)) { return iterate; }

        iterate = iterate->bucketNext;
    }

    return nullptr;
}

void HT::insert(Frame *frame) {
    int32 index = frame->key.getHash();

    // Set new front of list
    frame->bucketNext = table[index];
    table[index] = frame;
}

void HT::remove(Key key) {
    int32 index = key.getHash();

    // Need to consider case of unlinking head
    Frame *prev = nullptr;
    Frame *iterate = table[index];
    while (iterate) {
        // Unlink if found
        if (iterate->key.equals(key)) {
            // If we have a previous we are NOT at head
            // Otherwise, set table index start to next ptr
            if (prev) { prev->bucketNext = iterate->bucketNext; }
            else { table[index] = iterate->bucketNext; }

            return;
        }

        prev = iterate;
        iterate = iterate->bucketNext;
    }
}

void HT::dump() {
    printf("DUMPING HASH TABLE:\n");
    for (int i = 0; i < HT_SIZE; i++) {
        Frame *iterate = table[i];

        if (iterate) {
            printf("Bucket %d:\n", i);

            while (iterate) {
                iterate->print();
                
                iterate = iterate->bucketNext;
            }
        }
    }
    printf("=================\n\n");
}

// LRU CACHE FUNCTIONS

LRUCache::LRUCache(int32 capacity) {
    this->capacity = capacity;
    this->size = 0;
}

LRUCache::~LRUCache() {}

Frame *LRUCache::get(Key key) {
    Frame *found = this->ht.find(key);
    if (!found) { return nullptr; } // Cache miss

    this->ll.moveToHead(found);
    return found; // hit
}

/* 
 * Returns the evicted frame
 */
Frame *LRUCache::set(Frame *frame) {
    Frame *found = this->ht.find(frame->key);

    if (found) {
        // Already in the cache, just move to head
        this->ll.moveToHead(found);
        return nullptr; // No eviction
    }

    // Not in cache, so insert the frame
    this->ll.insertHead(frame);
    this->ht.insert(frame);
    this->size++;

    // SIZE CHECK- might need to evict
    if (this->size > this->capacity) {
        Frame *evict = this->ll.removeTail();
        this->ht.remove(evict->key);
        this->size--;  
        return evict; // let the buffer deal with it
    }

    return nullptr;
}

void LRUCache::dump() {
    printf("DUMPING LRUCache\n=============\n");
    this->ll.dump();
    this->ht.dump();
}

/* BUFFER WRAPPER */

Buffer::Buffer(int32 capacity) : cache(capacity) {}

Buffer::~Buffer() {}

/* pinPage()
 * key: key struct uniquely identifying a page
 *
 * We need to roughly follow "does the page exist in the cache? If it does, get it and pin it, 
 * otherwise pull it from disk and add it to the cache."
 */
HeapPage *Buffer::pinPage(Key key) {
    Frame *search = this->cache.get(key);
    if (!search) { // Not in cache, need to check disk
        // TEMPORARY START
        HeapPage *diskPage = new HeapPage(); // pretend this is the find from the disk

        if (!diskPage) { return nullptr; } // If the disk returns invalid, bail

        search = new Frame(key, diskPage); // Completely temporary
        // TEMPORARY END

        // By now we have a valid frame- try to set into the cache.
        // If the cache returns a frame, it was evicted, so free the memory
        Frame *cached = this->cache.set(search);
        if (cached) { 
            if (cached->dirty) {
                // TODO: WRITE THE PAGE
            }

            delete cached;
        }
    }

    search->pinCount++;
    return search->heapPage;
}

void Buffer::markDirty(Key key) {
    // We only mark cache frames dirty, there's nothing in disk. Just
    // exit if we don't find anything
    Frame *search = this->cache.get(key);
    if (!search) { return; }

    search->dirty = true;
}

void Buffer::unpinPage(Key key) {
    Frame *search = this->cache.get(key);
    if (!search) { return; }

    search->pinCount--;
}


void Buffer::dump() {
    printf("DUMPING BUFFER:\n===================\n");
    printf("Cache Size: %d\n\n", this->cache.size);
    this->cache.dump();
}
