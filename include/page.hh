#pragma once

#include <typedefs.hh>

#include <cstdio>

/* Globally defined page size in bytes */
constexpr int16 PAGE_SIZE = 4096;

/* Container for all information found in a struct
 *
 * We don't care about the size here, just need this accessible in memory
 */
struct Header {
    int32 pageid;
    int32 prevPage;
    int32 nextPage;
    int16 pageType;
    int16 freePtr;
    int16 freeSpace;
};

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

class Page {
public:
    byte data[PAGE_SIZE];

    Page();
    ~Page();

    /* Access the buffer */
    byte *getData();

    /* Header Metadata, not super useful, just cleaner than raw accesses in the buffer */
    Header readHeader();
    void writeHeader(Header h);

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
    
    void dump();
};

