# CUBA

CUBA (Context-UnBounded Analysis) contains algorithms for 
_context-unbounded reachability analysis for concurrent programs_. 
The program could contain recursion in which case reachability 
analysis is undecidable even for two threads. 

# Installation
1. git clone https://github.com/lpzun/cuba.git
2. make

# Usage
./cuba -h [--help]


# Documentation 

We generate the CUBA documentation with doxygen. The configuration file is `cuba.doxyfile` under folder `doc`. 


### Generate the documentation

- Make sure you have [doxygen](http://www.doxygen.org/) installed on your machine. Then, run

>     make

- To clean up the old documentation, you can run the following command

>     make clean

### Remark

For now, we only generate the documentation in __html__ format.   