# Rete Algorithm in C++

Rete is an algorithm for efficient pattern matching which is often used in business production rule systems to perform forward inference: The rules are translated into a graph structure, the rete (network), where every node implements some part of the condition-checks. By storing intermediate results in memory nodes, and reusing these nodes whenever possible, for multiple rules, the Rete algorithm gains improved evaluation speed (incremental updates are possible) on the cost of increased memory usage.

The algorithm is nothing new, but many implementations in c++ are rather small, unfinished hobby projects, are hidden in proprietary software or released under another license I'd rather not use (e.g. GPL). So, this is my attempt to implement the Rete algorithm, which also allows me to implement my own hooks and extensions.

If you are interested in how it works, please note the detailed description in [Robert B. Doorenbos' PhD thesis ('95)](reports-archive.adm.cs.cmu.edu/anon/1995/CMU-CS-95-113.pdf). I use it as a reference, but I won't implement it exactly as described there, neither will I include all the optimizations he describes. My main focus is on getting a working rule engine I can tweak to my demands.

My first use case will be symbolic reasoning on rdf data, so while the algorithm in principle is of very general nature, I include some code in here for that use case. Hopefully it will still be extensible for other use cases.