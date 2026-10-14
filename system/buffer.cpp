#include <buffer.hh>

LL::LL() {
    head = new Frame(); /* SENTINEL head */
    tail = new Frame(); /* SENTINEL tail */

    head->prev = nullptr;
    head->next = tail;

    tail->prev = head;
    tail->next = nullptr;
}

LL::~LL() {
    /* Linked list only owns the HEAD and TAIL sentinels */
    delete head;
    delete tail;
}

void LL::insertHead(Frame *frame) {
    frame->prev = head;
    frame->next = head->next;

    head->next->prev = frame;
    head->next = frame;
}

void LL::moveToHead(Frame *frame) {
    // Already at head
    if (frame->prev == head) { return; }

    // unlink
    frame->prev->next = frame->next;
    frame->next->prev = frame->prev;

    frame->prev = head;
    frame->next = head->next;

    head->next->prev = frame;
    head->next = frame;
}

void LL::remove(Frame *frame) {
    frame->prev->next = frame->next;
    frame->next->prev = frame->prev;

    frame->prev = nullptr;
    frame->next = nullptr;
}

/* findVictim()
 *
 * Normally in an LRU cache we take from the tail, but, we have pin counts to deal
 * with and don't want to evict a node with a > 0 pin count. Instead, crawl up the list and
 * see if there is one we can take.
 */
Frame *LL::findVictim() {
    if (tail->prev == head) { return nullptr; }

    Frame *ret = tail->prev; // save reference

    // Crawl backwards until we find a frame without pins
    while ((ret != head) && (ret->pinCount > 0)) { 
        ret = ret->prev;
    }

    // We might end up at the head, bail
    if (ret == head) { return nullptr; }

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


/* Hashtable Functions */
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

    // Set it to the new front of the list
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

/* Cache Functions */
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

/* victim()
 *
 * Returns an unpinned frame that could be evicted, or nullptr if every frame
 * is pinned without changing the cache
 */
Frame *LRUCache::victim() {
    return this->ll.findVictim();
}

/* insert()
 * frame: frame to add, must not already be in the cache
 *
 * Assumes there is room. Keeps the list and hashtable in step.
 */
void LRUCache::insert(Frame *frame) {
    this->ll.insertHead(frame);
    this->ht.insert(frame);
    this->size++;
}

/* remove()
 * frame: frame currently in the cache
 *
 * Unlinks the frame from both the list and hashtable. Does not free it.
 */
void LRUCache::remove(Frame *frame) {
    this->ll.remove(frame);
    this->ht.remove(frame->key);
    this->size--;
}

/* set()
 * frame: the frame intended to be added to the cache
 * evict: if a frame is evicted when set, this var is set to that frame
 *
 * If the given frame is already in the cache, it will be moved to the front of the
 * list. If it isn't, a node will be (or attempted to be) evicted from the linked
 * list and placed in the 'evict' param.
 */
bool LRUCache::set(Frame *frame, Frame **evict) {
    /* If in the cache already, move to the front */
    Frame *found = this->ht.find(frame->key);
    if (found) {
        this->ll.moveToHead(found);
        *evict = nullptr;
        return true;
    }

    /* If the cache is full, determine if we can evict something */
    *evict = nullptr;
    if (this->size >= this->capacity) {
        Frame *candidate = this->victim();

        if (!candidate) {
            return false; // All are pinned
        }

        /* Something will be evicted, set it in the param */
        this->remove(candidate);
        *evict = candidate;
    }

    /* Safe to insert */
    this->insert(frame);

    return true;
}

void LRUCache::dump() {
    printf("DUMPING LRUCache\n=============\n");
    this->ll.dump();
    this->ht.dump();
}

/* BUFFER Functions */
Buffer::Buffer(int32 capacity, Disk &disk) : cache(capacity), disk(disk) {}

/* Will not write dirty pages to disk, will forcefully clear the memory */
Buffer::~Buffer() {
    // Iterate and actually free memory and delete the frames
    Frame *itr = this->cache.ll.head->next;
    while (itr != nullptr && itr != this->cache.ll.tail) {
        Frame *next = itr->next;
        delete itr;
        itr = next;
    }
}

/* pinPage()
 * key: key struct uniquely identifying a page
 * outPage: Pointer to a page pointer, which is set to the desired
 *          page.
 *
 * Will either increment the pin count of a page in the buffer, or
 * pull the page from disk and set it in 'outPage.'
 */
BufferStatus Buffer::pinPage(Key key, Page **outPage) {
    // Check cache. If in cache, simply increment pin count
    Frame *search = this->cache.get(key);
    if (search) {
        search->pinCount++;
        *outPage = search->page;
        return BufferStatus::OK;
    }

    /* Validate before evicting anything for a page that doesn't exist */
    BufferStatus check = this->checkPageId(key.pageid);
    if (check != BufferStatus::OK) { return check; }

    // Removal should be the last thing we do, so we don't leak in the cache
    if (this->cache.size >= this->cache.capacity) {
        // Find a potential eviction
        Frame *victim = this->cache.victim();
        if (!victim) { return BufferStatus::AllPinned; }

        /* If the write fails the victim stays cached, so its changes aren't lost */
        if (victim->dirty) {
            DiskStatus write = this->disk.writePage(victim->key.pageid, victim->page->getData());
            if (write != DiskStatus::OK) { return BufferStatus::IOError; }
            victim->dirty = false;
        }

        this->cache.remove(victim);
        delete victim;
    }

    /* It must be in the disk. Fetch from disk */
    Frame *newFrame = new Frame(key);
    DiskStatus readStatus = this->disk.readPage(key.pageid, newFrame->page->getData());
    if (readStatus != DiskStatus::OK) { // Rare, but a disk read can fail so we don't need the frame
        delete newFrame;
        return BufferStatus::IOError;
    }

    /* Only a fully loaded frame goes in the cache, already pinned for the caller */
    newFrame->pinCount++;
    this->cache.insert(newFrame);

    *outPage = newFrame->page;
    return BufferStatus::OK;
}

void Buffer::markDirty(Key key) {
    // We only mark cache frames dirty, there's nothing in disk. Just
    // exit if we don't find anything
    Frame *search = this->cache.get(key);
    if (!search) { return; }

    search->dirty = true;
}

/* unpinPage()
 * key: unique identifier for a page in the buffer
 *
 * Will decrement the pin count of a page in the buffer. No disk opertions.
 */
BufferStatus Buffer::unpinPage(Key key) {
    Frame *search = this->cache.get(key);
    if (!search) { 
        return BufferStatus::NotFound; 
    }

    /* maybe this will save me a horrible debug later */
    if (search->pinCount <= 0) {
        return BufferStatus::NegativePins;
    }

    search->pinCount--;

    return BufferStatus::OK;
}

/* flush()
 *
 * Will iterate over the frames in the buffer, writing each to disk
 * with a dirty bit.
 */
BufferStatus Buffer::flush() {
    Frame *iterate = this->cache.ll.head->next;

    while (iterate != this->cache.ll.tail) {
        /* Try a page write if dirty, otherwise bail */
        if (iterate->dirty) {
            DiskStatus write = this->disk.writePage(iterate->key.pageid, iterate->page->getData());

            if (write != DiskStatus::OK) { return BufferStatus::IOError; }
            iterate->dirty = false;
        }

        iterate = iterate->next;
    }

    return BufferStatus::OK;
}

BufferStatus Buffer::checkPageId(int32 pageid) {
    DiskStatus check = disk.checkPageId(pageid);
    if (check == DiskStatus::InvalidPage) { return BufferStatus::InvalidPage; }
    if (check != DiskStatus::OK) { return BufferStatus::IOError; }

    return BufferStatus::OK;
}

void Buffer::dump() {
    printf("DUMPING BUFFER:\n===================\n");
    printf("Cache Size: %d\n\n", this->cache.size);
    this->cache.dump();
}
