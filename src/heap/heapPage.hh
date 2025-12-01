#pragma once

#include <string>

#include "./page.hh"

namespace heapPage {
    class HeapPage {
    public:
        page::Page *page;
        bool dirty;

        // Pairing of a heapfile string and the page number-
        // the two unique identifiers of a name (on a per-project)
        // basis
        std::string heapFile;
        int pageNum;
        struct HeapKey {
            std::string heapFile;
            int pageNum;
        };

        HeapPage();
        HeapPage(page::Page *page, std::string heapFile, int pageNum);
        ~HeapPage();
        HeapKey getHeapKey();
    };
}