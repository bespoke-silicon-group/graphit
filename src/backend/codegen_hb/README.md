# Code generation backend for HammerBlade architecture

This backend generates code to target the HammerBlade manycore architecture.
It is not fully implemented at this time.

## To-Do

1. Implement other directions of scheduling language
2. Implement Graph type for manycore blocked runtime

## Design Decisions

1. Do we plan to keep the same next/current frontier data structures for entire run? GraphIt creates new structures for each iteration.
2. How do we want to implement the Graph data type? Can we create a macro for setting up blocked vertexsets?

