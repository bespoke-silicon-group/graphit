# Code generation backend for HammerBlade architecture

This backend generates code to target the HammerBlade manycore architecture.
It is not fully implemented at this time.

## To-Do

1. Implement other directions of scheduling language. How will these work with memory blocking?
2. Implement Graph type for manycore blocked runtime
3. Look at the GraphIt NUMA optimizations and see if we can leverage them for our system.

## Design Decisions

1. Do we plan to keep the same next/current frontier data structures for entire run? GraphIt creates new structures for each iteration.
2. How do we want to implement the Graph data type? Can we create a macro for setting up blocked vertexsets?
    - Will we always index by long type or should we use graph_t?
    - Will we support the out_neigh() call on each node?
    - etc. (reference Graph class for other design decisions to be made)
3. Do we want to support templates? i.e. passing in the apply/filter funcs as params to the main kernel code
4. Do we want to allow users to switch between sparse and dense storage of vertex sets? (this is I believe in teh Graph class)
5. Do we want to implement deduplication in the edgeset aply expr?
6. Do we want the current and next frontiers to be persistent data structures across iterations? currently the next frontier is built each iteration and returned from the edgeset apply func.
7. How are we going to partition code between the Xeon and manycore? How will this affect our code generation?
