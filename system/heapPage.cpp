#include <heapPage.hh>

HeapPage::HeapPage() {
    page = new Page();
}

HeapPage::HeapPage(Page *page) {
    this->page = page;
}

HeapPage::~HeapPage() {
    delete page;
}

void HeapPage::writeByte(int16 loc, byte val) {
    page->data[loc] = val;
}

void HeapPage::writeInt16(int16 loc, int16 val) {
    page->data[loc] = (val >> 8) & 0xFF;
    page->data[loc + 1] = val & 0xFF;
}

void HeapPage::writeInt32(int16 loc, int32 val) {
    page->data[loc] = (val >> 24) & 0xFF;
    page->data[loc + 1] = (val >> 16) & 0xFF;
    page->data[loc + 2] = (val >> 8) & 0xFF;
    page->data[loc + 3] = val & 0xFF;
}

byte HeapPage::readByte(int16 loc) {
    return page->data[loc];
}

int16 HeapPage::readInt16(int16 loc) {
    return (page->data[loc] << 8) | 
            (page->data[loc + 1]);
}

int32 HeapPage::readInt32(int16 loc) {
    return ((int32) page->data[loc] << 24) |
            ((int32) page->data[loc + 1] << 16) |
            ((int32) page->data[loc + 2] << 8) |
            ((int32) page->data[loc + 3]);
}

/* Dump a 32 byte wide map of the page for debugging */
void HeapPage::dump() {
    printf("\nDUMPING PAGE BYTES\n------------------\n");
    for (int i = 0; i < PAGE_SIZE; i++) {
        if (i % 32 == 0) { printf("\n"); } // Create a new line every 32 bytes
        // printf("%02X ", page->data[i]);
        printf("%d ", (int32) page->data[i]);
    }
    printf("\n");
}

/* TABLE METADATA WRITES */
void HeapPage::writePageID(int32 val) { writeInt32(LOC_PAGE_ID, val); }
void HeapPage::writePrevID(int32 val) { writeInt32(LOC_PREV_PAGE, val); }
void HeapPage::writeNextID(int32 val) { writeInt32(LOC_NEXT_PAGE, val); }
void HeapPage::writePageType(int16 val) { writeInt16(LOC_PAGE_TYPE, val); }
void HeapPage::writeFreePtr(int16 val) { writeInt16(LOC_FREE_PTR, val); }
void HeapPage::writeFreeSpace(int16 val) { writeInt16(LOC_FREE_SPACE, val); }

/* TABLE METADATA READS */
int32 HeapPage::readPageID() { return readInt32(LOC_PAGE_ID); };
int32 HeapPage::readPrevID() { return readInt32(LOC_PREV_PAGE); };
int32 HeapPage::readNextID() { return readInt32(LOC_NEXT_PAGE); };
int16 HeapPage::readPageType() { return readInt16(LOC_PAGE_TYPE); };
int16 HeapPage::readFreePtr() { return readInt16(LOC_FREE_PTR); };
int16 HeapPage::readFreeSpace() { return readInt16(LOC_FREE_SPACE); };

void HeapPage::formatGeneric() {
    writePageID(P_NULL);
    writePrevID(P_NULL);
    writeNextID(P_NULL);
    writePageType(P_NULL);
    writeFreePtr(P_NULL);
    writeFreeSpace(P_NULL);
}