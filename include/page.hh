#pragma once

#include <typedefs.hh>

#include <unistd.h>

#include <cstdio>
#include <cstring>
#include <cstdint>

/* Page Constants */
constexpr int16 PAGE_SIZE = 4096;

class Page {
public:
    byte data[PAGE_SIZE]; 

    Page();
    Page(const byte *data);
    ~Page();

    byte *getData();
    void setData(const byte *data);

    void dump();
};
