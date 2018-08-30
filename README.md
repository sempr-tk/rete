# Rete Algorithm in C++

Rete is an algorithm for efficient pattern matching which is often used in business production rule systems to perform forward inference: The rules are translated into a graph structure, the rete (network), where every node implements some part of the condition-checks. By storing intermediate results in memory nodes, and reusing these nodes whenever possible, for multiple rules, the Rete algorithm gains improved evaluation speed (incremental updates are possible) on the cost of increased memory usage.

The algorithm is nothing new, but many implementations in c++ are rather small, unfinished hobby projects, are hidden in proprietary software or released under another license I'd rather not use (e.g. GPL). So, this is my attempt to implement the Rete algorithm, which also allows me to implement my own hooks and extensions.

If you are interested in how it works, please note the detailed description in [Robert B. Doorenbos' PhD thesis ('95)](reports-archive.adm.cs.cmu.edu/anon/1995/CMU-CS-95-113.pdf). I use it as a reference, but I won't implement it exactly as described there, neither will I include all the optimizations he describes. My main focus is on getting a working rule engine I can tweak to my demands.

My first use case will be symbolic reasoning on rdf data, so while the algorithm in principle is of very general nature, I include some code in here for that use case. Hopefully it will still be extensible for other use cases.



## Network

The core of this library is the support structure for rete networks, consisting of the working memory elements (WME), the different node types and other things only exist for the core structure, without any concrete reasoning functionality, without being pre-defined to any type of knowledge etc. The main parts are the following:

### rete::Network

While the rete network is constructed implicitly by connecting nodes, the Network-class contains a single node which should be used as the top-level entry-point into the network. It also provides a method to create a description of the current state of the network in a dot-file format.

### rete::WME

The working memory element (WME) class is the base for all types of knowledge that is to be processed in the rete network. An example for a concrete type is the rete::Triple class which represents simple string triples, but you may implement whatever you need here. Just make sure that a WME must be constant within the network, and every change must be stated explicitly by retracting and asserting WMEs.

### rete::AlphaNode

Alpha nodes perform simple checks on single WMEs. E.g., one node may check if the given WME is a Triple and if the triples predicate is "foo". Next you may append a node to check if the triples object if "bar". Again, implement your own checks. Examples are the classes TripleConsistency and TripleAlpha.

### rete::AlphaMemory

A storage for WMEs that match a the conditions checked by the chain of alpha nodes in front of it. The alpha memories are the barrier between the alpha- and the beta-network.

### rete::BetaNode

Beta nodes generally (with maybe some exceptions) perform checks on multiple WMEs. They can be connected to a BetaMemory and an AlphaMemory and are used to create joins between partial matches (BetaMemory) and the WMEs satisfying the next condition in the clause (AlphaMemory). The results are (extended) Tokens which are stored in the next BetaMemory. See e.g. the TripleJoin class. Think of inter-WME-conditions, while AlphaNodes implement intra-WME-conditions: AlphaNodes are used to find triples of the form "(?a foo bar)" and "(something else ?a)", and a BetaNode makes sure that the subject of the first triple in the results matches the object of the second triple (?a == ?a).

### rete::BetaMemory

Hold partial matching results (tokens).

### rete::Token

A token is a list of <N\> WMEs which fulfill the first <N\> conditions. Tokens are stored in beta memories and are the result of joins done by the beta nodes

### rete::AlphaBetaAdapter

The first beta node in a chain wants a beta memory and an alpha memory, but only alpha memories exist so far. The AlphaBetaAdapter is a BetaNode that is connected to an alpha memory only and simply forwards every entry in the alpha memory as a token to its beta memory, thus converting an alpha- into a beta memory as a starting point for the regular beta network.

### rete::ProductionNode, rete::AgendaNode

At the end of the matching network you may want to react to the found matches. A production node can be connected to a beta memory to do so. In particular, the AgendaNode is used to schedule productions to the agenda whenever a match is found.

### rete::Agenda

The agenda is a list of triples consisting of a token, a production and a propagation flag: The token is the match that was previously found, and the production is what should be executed on the match. The flag however states if the match was just found or is currently being retracted, so that the production can react differently in both cases. By scheduling the productions instead of executing them directly while processing the activations in the network we make sure that no production is executed prematurely, and can also implement a priority system.



## Example Network

The following network represents the pattern `(?a self ?b) (?x color red) (?y color red)`. For more complex patterns like `(?a self ?a) (?a color red)` where you need to intra-consistency checks and conditional joins, see the test files.

You can convert the dot-file to a png using
```
dot -Tpng network.dot > network.png
```
which will give you something like this:
![Alt text](example.png)
