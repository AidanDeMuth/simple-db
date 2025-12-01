#include <headerPage.hh>

HeaderPage::HeaderPage() {
    heapPage = new HeapPage();
}

HeaderPage::HeaderPage(HeapPage *heapPage) {
    this->heapPage = heapPage;
}

HeaderPage::~HeaderPage() {}