#pragma once

#include <typedefs.hh>

#include <cstdio>
#include <string.h>

/* Page Constants */
constexpr int16 PAGE_SIZE = 4096;

class Page {
public:
    byte data[PAGE_SIZE]; 

    Page();
    ~Page();

    byte *getData();

    void dump();
};
