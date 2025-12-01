#include <LRUCache.hh>

// LINKED LIST FUNCTIONS

LL::LL() {
    head = nullptr;
    tail = nullptr;
}

LL::~LL() {
    LLNode *iterate = head;

    while (iterate) {
        LLNode *remove = iterate;
        iterate = iterate->next;

        delete remove;
    }
}

void LL::insertHead(LLNode *llnode) {
    llnode->prev = nullptr;
    llnode->next = head;

    if (head) { head->prev = llnode; }
    head = llnode;

    // If first entry, need to adjust the tail
    if (!tail) { tail = llnode; }
}

void LL::moveToHead(LLNode *llnode) {
    // Can be from arbitrary location
    if (llnode == head) { return; }

    // Now, node is in [second element, tail], so detach
    if (llnode->prev) { llnode->prev->next = llnode->next; }
    if (llnode->next) { llnode->next->prev = llnode->prev; }
    if (llnode == tail) { tail = llnode->prev; }

    insertHead(llnode);
}

LLNode *LL::removeTail() {
    if (!tail) { return nullptr; }

    LLNode *ret = tail; // save reference

    // we do have a tail, unlink it and set head/tail correctly
    // otherwise
    if (tail->prev) { tail->prev->next = nullptr; }
    else { head = nullptr; }

    tail = tail->prev;
    return ret;
}

void LL::dump() {
    LLNode *iterate = head;
    
    printf("Dumping linked list: PID, HeapPage\n");
    while (iterate) {
        printf("LLNODE: %d %p\n", iterate->pageid, iterate);
        iterate = iterate->next;
    }
}

// HASHTABLE FUNCTIONS

HT::HT() {
    for (int i = 0; i < HT_SIZE; i++) {
        table[i] = nullptr;
    }
}

HT::~HT() { }

LLNode *HT::find(int32 pageid) {
    int32 index = HT::getHash(pageid);

    LLNode *iterate = table[index];
    while (iterate) {
        if (iterate->pageid == pageid) { return iterate; }

        iterate = iterate->bucketNext;
    }

    return nullptr;
}

void HT::insert(LLNode *llnode) {
    int32 index = HT::getHash(llnode->pageid);

    // Set new front of list
    llnode->bucketNext = table[index];
    table[index] = llnode;
}

void HT::remove(int32 pageid) {
    int32 index = HT::getHash(pageid);

    // Need to consider case of unlinking head
    LLNode *prev = nullptr;
    LLNode *iterate = table[index];
    while (iterate) {
        // Unlink if found
        if (iterate->pageid == pageid) {
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
    for (int i = 0; i < HT_SIZE; i++) {
        LLNode *iterate = table[i];

        if (iterate) {
            printf("Bucket %d:\n", i);

            while (iterate) {
                printf("  pageid is %d, LLNode ptr is %p\n", iterate->pageid, iterate);
                iterate = iterate->bucketNext;
            }
        }
    }
}

// LRU CACHE FUNCTIONS

LRUCache::LRUCache(int32 capacity) {
    this->capacity = capacity;
    this->size = 0;
}

LRUCache::~LRUCache() {}

HeapPage *LRUCache::get(int32 pageid) {
    LLNode *found = this->ht.find(pageid);
    if (!found) { return nullptr; } // Cache miss

    this->ll.moveToHead(found);
    return found->heapPage; // hit
}

/* MEMORY
 *
 *
 */
void LRUCache::set(int32 pageid, HeapPage *heapPage) {
    LLNode *found = this->ht.find(pageid);

    // If exists already, update it and bail
    if (found) {
        found->heapPage = heapPage;
        this->ll.moveToHead(found);
        return;
    }

    // Allocate and add new node
    LLNode *newNode = new LLNode;
    newNode->pageid = pageid;
    newNode->heapPage = heapPage;
    this->ll.insertHead(newNode);
    this->ht.insert(newNode);
    this->size++;

    // SIZE CHECK
    if (this->size > this->capacity) {
        LLNode *evict = this->ll.removeTail();
        this->ht.remove(evict->pageid);

        delete evict;
        this->size--;        
    }
}

void LRUCache::dump() {
    printf("DUMPING LRUCache\n=============\n");
    this->ll.dump();
    this->ht.dump();
}