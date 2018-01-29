# A schematic overview of tool RUBA
We implement the observation sequences for resource-unbounded analysis (RUBA) idea in this tool. It contains two parts for now (upcomming more):

```
                             RUBA
                   ___________|___________
                  |                       |
                 CUBA                    WUBA
                   
```

* CUBA: context-unbounded analysis
* WUBA: write-unbouded analysis

# Code architecture
### namespace
* ruba
* cuba
* wuba
* cmd

# Data structure

##### State representation

###### (1) Thread-wise data structures
* thread_config:
* thread_state:

###### (2) Global data structures
* explicit_config:
* symbolic_config:





