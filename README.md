# Life-Link
LifeLink is a C-based blood donation management system built using hash tables, linked lists, graphs, and a min-heap.

Donor records include the last donation date. A donor is considered active when
they have never donated (`00-00-0000`) or their last donation was at least 90
days ago. Compatible-donor search selects the closest active donor across the
blood groups compatible with the patient's group.

Build and run from the project directory:

```sh
gcc -std=c11 -Wall -Wextra -I. main.c graph.c hashtable.c heap.c -o life-link
./life-link
```
