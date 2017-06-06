#Usage
Usage:                    Purpose:
 Debug/cuba -h [--help]    show help message
 Debug/cuba source.pds     check given program


Problem instance:
 -f [--input-file] arg     an input pushdown system (default = X)
 -a [--target] arg         a target configuration (default = 0|1.2,3.4, interpreated as the first stack's content is 12, while second stack's) 
 -i [--initial] arg        an initial configuration (default = 0|0.0,1.1,2.2)
 -p [--pushdown]           show the pushdown system
 -m [--mode]
 -r [--reachability]       check the reachability of the target (default = false, aka compute all of reachable configurations)
 
Sequential Mode:
 -atm [--automaton]        show the pushdown store automaton

Concurrent Mode:
 -k [--ctx-bound] arg      the bound of contexts (default = 1)
 -n [--threads] arg        the number of threads (default = 1), this is only useful
 -s [--simulator]          run simulator, a forward search but probably non-terminate

Other Options:
 -h [--help]               help information
 -cmd [--cmd-line]         show the command line
 -all [--all]              show all of above messages
 -v [--version]            show version information and exit



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

# Pushdown Store Automaton
We implement the pushdown store automaton by means of post*. We *design* an algorithm as follows:

**INPUT:** The inputs are a PDS P = (Q, \Gamma, \Delta, q0, r0), and an initial configuration c0. 

**OUTPUT:** a finite automaton A represents all reachable configuration of P. 

  1. represent c0 as a finite automaton A

  2. a saturation procedure

We have saturation rules as follows:

  (1) for each push operation (q, r) +-> (q', r'), we add a new state to A;

  (2) Push operation: (q, r) +-> (q', r')

  (3) 
  
***Intermediate state*** Generating an intermediate state is implemented via a static variable, shown as follows

static fsa_state INTERM_STATE

and a static function, shown as follows

static fsa_state create_interm_state() {
	return ++INTERM_STATE;
}
  
# Anonymize
## Anonymize-by-split
This is 
