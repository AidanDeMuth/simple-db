#include <page.hh>

Page::Page() {
    memset(this->data, 0, PAGE_SIZE);
}
Page::~Page() {}

/* Get the data ptr of the page */
byte *Page::getData() {
    return this->data;
}

/* Dump a 32 byte wide map of the page for debugging */
void Page::dump() {
    printf("\nDUMPING PAGE BYTES\n------------------\n");
    for (int i = 0; i < PAGE_SIZE; i++) {
        if (i % 32 == 0) { printf("\n"); } // Create a new line every 32 bytes
        printf("%02X ", this->data[i]);
    }
    printf("\n");
}
