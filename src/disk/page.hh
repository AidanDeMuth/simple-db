#ifndef PAGE
#define PAGE

#include <cstring>

#define PAGE_SIZE 4096
#define INIT_PAGES 500

class Page {
public:
    unsigned char bytes[PAGE_SIZE] = {0};

    Page() {}

    Page(unsigned char *data) {
        memcpy(bytes, data, PAGE_SIZE);
    }

    ~Page() {

    }
};

#endif