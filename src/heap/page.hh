#pragma once

#include <cstring>
#include <unistd.h>

#include "../util/global_constants.hh"

namespace page {
    class Page {
    private:
        unsigned char data[PAGE_SIZE];

    public: 
        /* Page()
        *
        * Need two constructors - one to create a new page with empty data,
        * and one to take in data and be treated as a page, possibly for
        * in-memory manipulation
        */
        Page();
        Page(void *data);
        ~Page();
        unsigned char *getData();
        void setData(void *data);
    };
}