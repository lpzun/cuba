# cuba
Context-Unbounded Analysis for Concurrent Software

# Installation
1. git clone https://github.com/lpzun/cuba.git
2. make

# Usage
./cuba -h [--help]

# Code Details

## cuba
cuba contains the algorithms for _Context-Unbounded Analysis for Concurrent Software_. 

## util
util contains all code that uses in this project. 

_state.hh state.cc_: 
the data structures that designs for this project. 

# Design and Implementation
**Context Switch**: 
  A _context_ is an uninterrupted sequence of actions executed by a _single_ thread.  
  
  Example 1: A trace (q1|**r1**,r1,r2,r1,r4) -> (q2|**r2**,r1,r2,r1,r4) -> (q3|**r4**,r1,r2,r1,r4) 
  in the same context.
  
  Example 2: A trace (q1|**r1**,r1,r2,r1,r4) -> (q2|**r2**,r1,r2,r1,r4) -> (q4|r2,**r2**,r2,r1,r4) 
  has a context switch. 
  
  
Data structure **global configuration** contains four different parts: (tid, k, s|w1,...,wn)
c1 := (2,5,s|w1,...,wn),
c2 := (3,5,s|w1,...,wn),
c3 := (2,6,s|w1,...,wk)

**Theorem 1**: c1 and c2 can never be merged, while c1 and c3 can be merged.

**Lemma 2**: successors(c1) \subset successors(c3)

  