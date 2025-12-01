#include <heapPage.hh>

/* class Header()
 *
 * A header page is the FIRST (and possibly more, elsewhere linked) page of the
 * RDBMS. The header page will follow the same constant patterns as any other page,
 * but the containing data will need to be treated specially.
 */
class HeaderPage {
    HeapPage *heapPage;

    HeaderPage();
    HeaderPage(HeapPage *heapPage);
    ~HeaderPage();
};