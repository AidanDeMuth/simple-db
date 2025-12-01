#include <simpledb.hh>

int main() {
    Page p;
    p.dump();

    unsigned char myarray[4096] = {0};
    myarray[2041] = 0xFF;
    Page q(myarray);
    q.dump();

    unsigned char emptyarray[4096] = {0};
    q.setData(emptyarray);
    q.dump();
}