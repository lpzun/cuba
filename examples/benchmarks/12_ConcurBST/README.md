#Description
This repository contains SnapTree, a concurrent AVL tree with fast
cloning, snapshots, and consistent iteration.  It is described in
the paper "A Practical Concurrent Binary Search Tree", by N. Bronson,
J. Casper, H. Chafi, and K. Olukotun, published in PPoPP'10.

SnapTreeMap is a drop-in replacement for ConcurrentSkipListMap,
with the additional guarantee that clone() is atomic and
iteration has snapshot isolation.  For more details see
http://ppl.stanford.edu/papers/ppopp207-bronson.pdf