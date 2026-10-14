#include <simpledb.hh>
#include <assert.h>

void testReadWrite() {
    Page p;
    p.writeByte(0, 'A');
    p.writeInt16(10, -300);
    p.writeInt32(20, -70000);

    assert(p.readByte(0) == 'A');
    assert(p.readInt16(10) == -300);
    assert(p.readInt32(20) == -70000);
}

void testHeader() {
    Page p;
    p.writePageID(1);
    p.writeNextID(2);
    p.writePageType(P_HEADER);

    assert(p.readPageID() == 1);
    assert(p.readNextID() == 2);
    assert(p.readPageType() == P_HEADER);
}

int main() {
    testReadWrite();
    testHeader();
}