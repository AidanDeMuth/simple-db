#include <heapScan.hh>

/* An invalid starting page is reported by startScan(), since pinPage
 * validates the page id
 */
HeapScan::HeapScan(int32 currid, Buffer &buffer) : currid(currid), buffer(buffer), currPage(nullptr) {}

HeapScan::~HeapScan() { endScan(); }

BufferStatus HeapScan::startScan() {
    /* Pin the first page */
    return this->buffer.pinPage(Key(this->currid), &this->currPage);
}

bool HeapScan::hasNext() {
    if (!currPage) { return false; } /* to be safe */
    return (this->currPage->readNextID() != INVALID_PAGE);
}

BufferStatus HeapScan::next() {
    if (!currPage) { return BufferStatus::InvalidState; }

    /* Unpin current page, and store next id */
    int32 nextId = this->currPage->readNextID();
    BufferStatus unpin = buffer.unpinPage(Key(this->currid));

    /* Report back error... if any at all */
    if (unpin != BufferStatus::OK) {
        return unpin;
    }

    /* Pin the next page */
    this->currPage = nullptr;
    this->currid = nextId;
    return buffer.pinPage(Key(nextId), &this->currPage);
}

BufferStatus HeapScan::endScan() {
    if (!currPage) { return BufferStatus::OK; }

    /* Set the ptr to null to prevent use- page may be dirty now */
    this->currPage = nullptr;
    return buffer.unpinPage(Key(this->currid));
}