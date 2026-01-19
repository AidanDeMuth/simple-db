#include <page.hh>
#include <buffer.hh>

/* In my implementation, a heap file will correspond to any pages linked together
 * through their 'prev' and 'next' ptrs. To keep it simple, it will only traverse
 * forward for now, and expected usage will be to give it the first page id
 */
class HeapScan {
public:
    int32 currid; /* Starting point of the scan (later, could be for restarts) */
    Buffer &buffer; /* Buffer reference to construct the class with */

    Page *currPage; /* Currently loaded frame */

    HeapScan(int32 currid, Buffer &buffer);
    ~HeapScan();

    BufferStatus startScan(); /* Pins the first page of the heap, if available */
    bool hasNext(); /* Checks if the next page ID is valid */
    BufferStatus next(); /* Actually fetches the next page */
    BufferStatus endScan(); /* Unpin loaded page and kill the scan */
};