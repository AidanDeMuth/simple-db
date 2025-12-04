#include <page.hh>

Page::Page() {  }

Page::~Page() {  }

void Page::writeByte(int16 loc, byte val) {
    buf[loc] = val;
}

void Page::writeInt16(int16 loc, int16 val) {
    buf[loc] = (val >> 8) & 0xFF;
    buf[loc + 1] = val & 0xFF;
}

void Page::writeInt32(int16 loc, int32 val) {
    buf[loc] = (val >> 24) & 0xFF;
    buf[loc + 1] = (val >> 16) & 0xFF;
    buf[loc + 2] = (val >> 8) & 0xFF;
    buf[loc + 3] = val & 0xFF;
}

byte Page::readByte(int16 loc) {
    return buf[loc];
}

int16 Page::readInt16(int16 loc) {
    return (buf[loc] << 8) | 
            (buf[loc + 1]);
}

int32 Page::readInt32(int16 loc) {
    return ((int32) buf[loc] << 24) |
            ((int32) buf[loc + 1] << 16) |
            ((int32) buf[loc + 2] << 8) |
            ((int32) buf[loc + 3]);
}

/* Dump a 32 byte wide map of the page for debugging */
void Page::dump() {
    printf("\nDUMPING PAGE BYTES\n------------------\n");
    for (int i = 0; i < PAGE_SIZE; i++) {
        if (i % 32 == 0) { printf("\n"); } // Create a new line every 32 bytes
        // printf("%02X ", page->data[i]);
        printf("%d ", (int32) buf[i]);
    }
    printf("\n");
}

/* TABLE METADATA WRITES */
void Page::writePageID(int32 val) { writeInt32(LOC_PAGE_ID, val); }
void Page::writePrevID(int32 val) { writeInt32(LOC_PREV_PAGE, val); }
void Page::writeNextID(int32 val) { writeInt32(LOC_NEXT_PAGE, val); }
void Page::writePageType(int16 val) { writeInt16(LOC_PAGE_TYPE, val); }
void Page::writeFreePtr(int16 val) { writeInt16(LOC_FREE_PTR, val); }
void Page::writeFreeSpace(int16 val) { writeInt16(LOC_FREE_SPACE, val); }

/* TABLE METADATA READS */
int32 Page::readPageID() { return readInt32(LOC_PAGE_ID); };
int32 Page::readPrevID() { return readInt32(LOC_PREV_PAGE); };
int32 Page::readNextID() { return readInt32(LOC_NEXT_PAGE); };
int16 Page::readPageType() { return readInt16(LOC_PAGE_TYPE); };
int16 Page::readFreePtr() { return readInt16(LOC_FREE_PTR); };
int16 Page::readFreeSpace() { return readInt16(LOC_FREE_SPACE); };

void Page::formatGeneric() {
    writePageID(P_NULL);
    writePrevID(P_NULL);
    writeNextID(P_NULL);
    writePageType(P_NULL);
    writeFreePtr(P_NULL);
    writeFreeSpace(P_NULL);
}