# Multithreaded Recrusive programs

### 11. QuickSort
##### Description
Concurrent QuickSort shows one important concurrency paradigm: dynamic multithreading. 

Dynamic multithreading does not fit our requirements very well because of the presence 
of dynamic thread creation. 

But, we 

For us, **thread pool** is a better option. 

Another example is mergesort: a multithreaded recursive mergesort. Please see here for an implementation:
[https://github.com/zakrywilson/multithreaded-recursive-merge-sort](https://github.com/zakrywilson/multithreaded-recursive-merge-sort).

### 12. Concurrent BST
##### Description
This repository contains SnapTree, a concurrent AVL tree with fast
cloning, snapshots, and consistent iteration.  It is described in
the paper "A Practical Concurrent Binary Search Tree", by N. Bronson,
J. Casper, H. Chafi, and K. Olukotun, published in PPoPP'10.

SnapTreeMap is a drop-in replacement for ConcurrentSkipListMap,
with the additional guarantee that clone() is atomic and
iteration has snapshot isolation.  For more details see

##### URL
[http://ppl.stanford.edu/papers/ppopp207-bronson.pdf](http://ppl.stanford.edu/papers/ppopp207-bronson.pdf)

### 13. Files and Folder Crawler
##### Description
This is a multi-threaded java program that takes a filepath and recursively 
goes through each folder and displays the contents of each folder. 


##### URL

**Paper**: [http://amac4.blogspot.com/2013/07/java-multi-threaded-hierarchical-file.html](http://amac4.blogspot.com/2013/07/java-multi-threaded-hierarchical-file.html)

**Source code**: [https://github.com/nbronson/snaptree](https://github.com/nbronson/snaptree)



### 14. JavaXT Directory Search
##### Description
**Project**: 
JavaXT is a collection of Java libraries and utilities that provide a number of functions 
not available in the standard JDK. It is an extension of the Java core, written to simplify 
Java development. 

**Class Directory**: Used to represent a directory on a file system. In many ways, this 
class is an extension of the java.io.File class. However, unlike the java.io.File class,
this object provides functions that are relevant and specific to directories. For example, 
this class provides a mechanism to move and copy directories - something not offered by 
the java.io.File class. In addition, this class provides a mechanism to retrieve files 
and folders found in a directory AND any subdirectories. This is accomplished via a 
multi-threaded recursive search. Finally, this class provides a powerful tool to monitor 
changes made to the directory (e.g. getEvents).


##### URL
[http://www.javaxt.com/documentation/?jar=javaxt-core.jar&class=Directory&package=javaxt.io](http://www.javaxt.com/documentation/?jar=javaxt-core.jar&class=Directory&package=javaxt.io)

### 15. Multhreaded Fibonacci Function
##### URL
[https://groups.google.com/forum/#!topic/golang-nuts/shZUZ9ZeVqs](https://groups.google.com/forum/#!topic/golang-nuts/shZUZ9ZeVqs)

### 16. SPADE
##### URL
[https://www.liafa.jussieu.fr/~sighirea/spade/](https://www.liafa.jussieu.fr/~sighirea/spade/)


# Resources for Dynamic Multhreaded Recursive Programs
### 1. Slides from UTA


### 2. Minicourse
[http://supertech.csail.mit.edu/papers/minicourse.pdf](http://supertech.csail.mit.edu/papers/minicourse.pdf)

# Other Pushdown benchmarks

1. 
##### URL
[http://www.cs.ox.ac.uk/people/matthew.hague/pdsolver.html](http://www.cs.ox.ac.uk/people/matthew.hague/pdsolver.html).
