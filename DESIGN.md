#Source Code Organization
## util
util contains all code that uses in this project. 

_state.hh state.cc_: 
the data structures that designs for this project. 

# Design and Implementation
1. **Empty** When a stack of a PDS is empty, then the following action would be blocked

	(1) pop()
	(2) overwrite();

2. **Context Switch** A context is an uninterrupted sequence of actions executed by a single thread.

    (1) Example 1: A trace (q1|r1,r1,r2,r1,r4) -> (q2|r2,r1,r2,r1,r4) -> (q3|r4,r1,r2,r1,r4) occurs in the same context.

    (2) Example 2: A trace (q1|r1,r1,r2,r1,r4) -> (q2|r2,r1,r2,r1,r4) -> (q4|r2,r2,r2,r1,r4) contains one context switch. 

3. In my implementation, data structure global configuration contains four different parts (already done!): 

    (tid, k, s|w1,...,wn): 

    tid     := the last thread's identifier to current configuration;
    k       := the number of context switches used to reach current configuration;
    s       := control state, aka shared state in a program-like model;
    w1,…,wn := the stack content for n different threads.
    Examples: 
              c1 := (2,5,s|w1,...,wn), 
              c2 := (3,5,s|w1,...,wn), 
              c3 := (2,6,s|w1,...,wn)

**Theorem 1** _c1 and c2 can never be merged, while c1 and c3 can be merged._

**Lemma 2:** _successors(c3) \subset successors(c1)._

4. Top Configuration: Applying symmetry reduction to top configurations (already done!).