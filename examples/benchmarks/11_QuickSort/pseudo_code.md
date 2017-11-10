### 1. Pseudo code for multithreaded merge sort:

```c++
MergeSort(A, l, r) {
  if (l < r) {
    int m = (l + r)/2;
    spawn MergeSort(A, l, m);
    MergeSort(A, m+1, r);
    sync();
    Merge(A, l, r, m);
  }
}
```

### 2. Pseudo code for multithreaded quick sort:

##### Dynamic multithreading

```c++
QuickSort(A, l, r) {
  if (l < r) {
    int m = Partition(A, l, r);
    spawn QuickSort(A, l, m);
    QuickSort(A, m+1, r);
    sync();
  }
}
```

##### Thread pool


```c++
ParallelQuickSort(A, l, r) {
  if (l < r) {
    int m = Partition(A, l, r);
    spawn QuickSort(A, l, m);
    spawn QuickSort(A, m+1, r);
    sync();
  }
}
// Normal quicksort procedure
QuickSort(A, l, r) {
  if (l < r) {
    int m = Partition(A, l, r);
    QuickSort(A, l, m);
    QuickSort(A, m+1, r);
  }
} 
 
```