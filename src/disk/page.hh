#ifndef PAGE
#define PAGE

#include <cstring>
#include <unistd.h>

#include "../util/project_config.hh"

class Page {
private:
    unsigned char data[config::PAGE_SIZE];

public:
    /* Page()
     *
     * Need two constructors - one to create a new page with empty data,
     * and one to take in data and be treated as a page, possibly for
     * in-memory manipulation
     */
    Page() {
        memset(this->data, 0, config::PAGE_SIZE);
    }
    Page(void *data) {
        memcpy(this->data, data, config::PAGE_SIZE);
    }

    // Returns the REFERENCE to the bytes
    unsigned char *getData() {
        return this->data;
    }

    void setData(void *data) {
        memcpy(this->data, data, config::PAGE_SIZE);
    }
};

#endif