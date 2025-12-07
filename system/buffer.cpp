#include <buffer.hh>

// LINKED LIST FUNCTIONS

LL::LL() {
    // SENTINEL nodes
    head = new Frame();
    tail = new Frame();

    head->prev = nullptr;
    head->next = tail;

    tail->prev = head;
    tail->next = nullptr;
}

LL::~LL() {
    // Come up with a destructor
}

void LL::insertHead(Frame *frame) {
    frame->prev = head;
    frame->next = head->next;

    head->next->prev = frame;
    head->next = frame;
}

void LL::moveToHead(Frame *frame) {
    // bail
    if (frame->prev == head) { return; }

    // unlink
    frame->prev->next = frame->next;
    frame->next->prev = frame->prev;

    frame->prev = head;
    frame->next = head->next;

    head->next->prev = frame;
    head->next = frame;
}

Frame *LL::removeTail() {
    if (tail->prev == head) { return nullptr; }

    Frame *ret = tail->prev; // save reference

    // unlink
    ret->prev->next = tail;
    tail->prev = ret->prev;

    return ret;
}

void LL::dump() {
    Frame *iterate = head->next;
    
    printf("Dumping linked list: PID, page\n");
    while (iterate != tail) {
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

/* BUFFER Functions */

Buffer::Buffer(int32 capacity, Disk &disk) : cache(capacity), disk(disk) {}

Buffer::~Buffer() {}

/* pinPage()
 * key: key struct uniquely identifying a page
 * outPage: Pointer to a page pointer, which is set to the desired
 *          page.
 *
 * We need to roughly follow "does the page exist in the cache? If it does, get it and pin it, 
 * otherwise pull it from disk and add it to the cache."
 */
BufferStatus Buffer::pinPage(Key key, Page **outPage) {
    Frame *search = this->cache.get(key);

    if (!search) { 
        // Not in cache, need to check disk
        Frame *newFrame = new Frame();
        DiskStatus readStatus = this->disk.readPage(key.pageid, newFrame->page->getData());
        if (readStatus != DiskStatus::OK) {
            delete newFrame;
            *outPage = nullptr;

            return BufferStatus::IOError;
        }
        newFrame->key = key;

        // Insert into cache
        Frame *evicted = this->cache.set(newFrame);
        if (evicted) { 
            // Write evicted page to disk if dirty
            if (evicted->dirty) {
                DiskStatus writeStatus = this->disk.writePage(evicted->key.pageid, evicted->page->getData());

                if (writeStatus != DiskStatus::OK) {
                    delete evicted;
                    *outPage = nullptr;

                    return BufferStatus::IOError;
                }
            }

            delete evicted;
        }

        search = newFrame;
    }

    search->pinCount++;
    *outPage = search->page;
    return BufferStatus::OK;
}

void Buffer::markDirty(Key key) {
    // We only mark cache frames dirty, there's nothing in disk. Just
    // exit if we don't find anything
    Frame *search = this->cache.get(key);
    if (!search) { return; }

    search->dirty = true;
}

BufferStatus Buffer::unpinPage(Key key) {
    Frame *search = this->cache.get(key);
    if (!search) { 
        return BufferStatus::NotFound; 
    }

    search->pinCount--;

    if (search->pinCount < 0) {
        return BufferStatus::NegativePins;
    }

    return BufferStatus::OK;
}

void Buffer::dump() {
    printf("DUMPING BUFFER:\n===================\n");
    printf("Cache Size: %d\n\n", this->cache.size);
    this->cache.dump();
}
