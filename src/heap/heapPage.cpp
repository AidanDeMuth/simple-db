#include "./heapPage.hh"
#include "./page.hh"

namespace heapPage {
    /* If we want to do something in memory, nice to have a default
     * constructor. Maybe for something like sorting in the buffer
     *
     */
    HeapPage::HeapPage() {
        this->page = new page::Page();

        this->dirty = false;
        this->pageNum = -1;
    }

    HeapPage::HeapPage(page::Page *page, std::string heapFile, int pageNum) {
        this->page = page;

        this->dirty = false;
        this->pageNum = pageNum;
        this->heapFile = heapFile;
    }

    HeapPage::~HeapPage() {
        delete this->page;
    }

    /* getHeapKey()
     * returns a pairing of the heapFile, and pageID to be used
     * as a map key.
     */
    HeapPage::HeapKey HeapPage::getHeapKey() {
        return HeapPage::HeapKey{heapFile, pageNum};
    }
}