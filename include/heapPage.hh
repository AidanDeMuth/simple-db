#pragma once

#include <typedefs.hh>

#include <page.hh> /* a heap page wraps a page, enabling read and write operations */

/* Special locations that are constant to EVERY page */
constexpr int32 LOC_PAGE_ID = 0;
constexpr int32 LOC_PREV_PAGE = 4;
constexpr int32 LOC_NEXT_PAGE = 8;
constexpr int16 LOC_PAGE_TYPE = 12;
constexpr int16 LOC_FREE_PTR = 14;
constexpr int16 LOC_FREE_SPACE = 16;

/* Page Types */
constexpr int16 P_NULL = -1;
constexpr int16 P_HEADER = 1;
constexpr int16 P_SPACEMAP = 2;

class HeapPage {
public:
    Page *page;

    HeapPage();
    HeapPage(Page *page);
    ~HeapPage();

    /* PAGE I/O */
    void writeByte(int16 loc, byte val);
    void writeInt16(int16 loc, int16 val);
    void writeInt32(int16 loc, int32 val);
    byte readByte(int16 loc);
    int16 readInt16(int16 loc);
    int32 readInt32(int16 loc);

    /* Table Metadata */
    void writePageID(int32 val);
    void writePrevID(int32 val);
    void writeNextID(int32 val);
    void writePageType(int16 val);
    void writeFreePtr(int16 val);
    void writeFreeSpace(int16 val);

    int32 readPageID();
    int32 readPrevID();
    int32 readNextID();
    int16 readPageType();
    int16 readFreePtr();
    int16 readFreeSpace();

    void formatGeneric();

    void dump();
};

