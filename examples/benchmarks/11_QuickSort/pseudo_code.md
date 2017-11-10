Pseudo code for multithreaded merge sort:

```java
Merge-Sort(A, l, r)
  if (l < r) {
    int m = (l + r)/2;
    spawn Merge-Sort(A, l, m);
    merge
  }
```