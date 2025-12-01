#include <cstring>
#include <unistd.h>

#include "page.hh"
#include "../util/global_constants.hh"

namespace page {
    /* Page()
     *
     * Need two constructors - one to create a new page with empty data,
     * and one to take in data and be treated as a page, possibly for
     * in-memory manipulation
     */
    Page::Page() {
        memset(this->data, 0, PAGE_SIZE);
    }
    Page::Page(void *data) {
        memcpy(this->data, data, PAGE_SIZE);
    }
    Page::~Page() {}

    // Returns the REFERENCE to the bytes
    unsigned char *Page::getData() {
        return this->data;
    }

    void Page::setData(void *data) {
        memcpy(this->data, data, PAGE_SIZE);
    }
}