#include <simpledb.hh>

int main() {
    unsigned char myarray[4096] = {0};
    myarray[2041] = 0xFF;

    /*
    Page *q = new Page(myarray);
    HeapPage hp(&q);
    hp.dump();

    HeapPage t(q);
    t.writeByte(2, 'a');
    byte test1 = t.readByte(2);

    t.writeInt16(0, 1000);
    int16 test2 = t.readInt16(0);

    t.writeInt32(55, 256);
    int32 test3 = t.readInt32(55);

    t.dump();
    printf("Tests: %d %d %d", test1, test2, test3);
    */

    Page *s = new Page(myarray);
    HeapPage r(s);

    r.formatGeneric();
    r.dump();
    printf("Tests: %d %d %d", r.readPageID(), r.readNextID(), r.readPrevID());
}