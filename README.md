TODO

1) Add bounds checks to heap page reads/writes
2) Pack the Frame struct for smarter padding
3) (done) right now set() overwrites the heappage, it probably shouldn't 
4) (done) Add error handling
5) Add negative pin count check in unpinpage
6) If a pread is less than page size is the file corrupted? 
7) pwrite check page number
8) More elegantly init first page and manage open db