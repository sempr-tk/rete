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
which will give you something like this (old picture, see sections RuleParser/Reasoner for newer images):
![Alt text](img/example.png)



## Reasoning

Our main use case for the rete network is not only pattern matching, but _reasoning_. The rete algorithm is used to find matches for the preconditions of the rules, which are then executed to infer new WMEs that are added to the network, etc.

To manage the known facts and multiple sources (assertions or inference chains) for them, a few more classes are implemented in a more or less generic way:

### rete::Evidence

Evidences, or rather **proof**s  (*Todo: Change the name?*) are the reasons why a WME is still kept in the knowledge base. They can be **AssertedEvidence** which is given externally, or **InferredEvidence** which describes which production created the WME from which match. Keeping track of evidences has two major advantages:

1. It allows us to inspect where some knowledge comes from. We can now follow the chain of inference back to its origin, and display which rules and matches lead to the final result.
2. When multiple rules and/or matches result in the same inferred WME and one source/evidence is retracted, we can still keep it.

Of course, this bookkeeping results in some overhead, but is IMHO necessary to correctly handle the removal of formerly valid knowledge.

### rete::BackedWME

A wrapper for WMEs that adds a list of evidences to it.

### rete::Reasoner

The reasoner contains two things: A rete network and a set of BackedWMEs. It allows us to add and remove evidences, and adds/retracts WMEs to/from the network when necessary. It also provides methods to perform the inference. To do so, it simply takes the first item on the networks agenda, executes it and, if a token was asserted, takes inferred WMEs from the production and adds them to the knowledge base, with the token and the production as evidence.

"Executing an AgendaItem" means to call the `execute` method of its production with the provided token and the propagation flag. In case the flag is "ASSERTED" we are performing normal forward inference, and the production is allowed to infer new WMEs. When the flag is "RETRACTED" this is not possible, as the only token we have as a "reason" is currently being removed from the knowledge base. If you want to infer knowledge from the absence of WMEs / tokens, consider implementing negative nodes in the rete network.



## Reasoning with RDF triples

Up to this point everything has been rather generic, without actual, sensible representation of knowledge or consequences etc. My main focus with this algorithm is to implement some reasoning on RDF triples, so I present some classes for this here. But you can implement your own stuff, completely unrelated to RDF triples, of course.

### WMEs and nodes

The knowledge representation and some of the nodes used to check conditions have already been described before: TripleAlpha, TripleConsitency and TripleJoin are used to construct the network and find patterns in the set of Triples. To infer a new triple as a consequence of a match, you can use the `InferTriple`-Node.

The following code example implements the rule:
`[(?a rdfs:subClassOf ?b), (?b rdfs:subClassOf ?c) -> (?a rdfs:subClassOf ?c)]`

It then adds the facts: `(A rdfs:subClassOf B)` , `(B rdfs:subClassOf C)` and `(C rdfs:subClassOf D)`. This results in the inferred triples:

```
(A rdfs:subClassOf C)
(A rdfs:subClassOf D)
(B rdfs:subClassOf D)
```

And when the triple `(B rdfs:subClassOf C)` is removed, all three inferred triples get removed, too.

Code (see [SubClassExample.cpp](test/SubClassExample.cpp)):

```c++
#include <iostream>
#include <fstream>

// your include paths will obviously differ.
#include "../include/Network.hpp"
#include "../include/Triple.hpp"
#include "../include/TripleAlpha.hpp"
#include "../include/AlphaMemory.hpp"
#include "../include/JoinNode.hpp"
#include "../include/AlphaBetaAdapter.hpp"
#include "../include/TripleConsistency.hpp"
#include "../include/TripleJoin.hpp"
#include "../include/AgendaNode.hpp"
#include "../include/InferTriple.hpp"

#include "../include/Reasoner.hpp"
#include "../include/AssertedEvidence.hpp"

using namespace rete;


void save(Network& net, const std::string& filename)
{
    std::ofstream out(filename);
    out << net.toDot();
    out.close();
}

int main()
{
    Reasoner reasoner;
    Network& net = reasoner.net();

    // setup network
    //        C1                     C2
    // (?a rdfs:subClassOf ?b) (?b rdfs:subClassOf ?c)

    // predicate check
    auto foo = std::make_shared<TripleAlpha>(Triple::PREDICATE, "rdfs:subClassOf");
    net.getRoot()->addChild(foo);
    foo->initAlphaMemory();

    // adapter for first (and only) join
    auto adapter = std::make_shared<AlphaBetaAdapter>();
    BetaNode::connect(adapter, nullptr, foo->getAlphaMemory());

    // join where object of the most recent wme in the token matches the subject of the wme in the
    //bin                                    |--- C1.?b ------|----- C2.?b -----|
    auto join = std::make_shared<TripleJoin>(0, Triple::OBJECT, Triple::SUBJECT);
    BetaNode::connect(join, adapter->getBetaMemory(), foo->getAlphaMemory());


    // the consequence: construct (C1.?a  rdfs:subClassOf  C2.?c)
    InferTriple::Ptr infer(new InferTriple(
        {1, Triple::SUBJECT},
        "rdfs:subClassOf",
        {0, Triple::OBJECT}
    ));

    // create an AgendaNode for the production
    auto inferNode = std::make_shared<AgendaNode>(infer, net.getAgenda());
    join->getBetaMemory()->addProduction(inferNode);

    // put in some data
    auto t1 = std::make_shared<Triple>("A", "rdfs:subClassOf", "B");
    auto t2 = std::make_shared<Triple>("B", "rdfs:subClassOf", "C");
    auto t3 = std::make_shared<Triple>("C", "rdfs:subClassOf", "D");

    AssertedEvidence::Ptr source(new AssertedEvidence("some named graph or just anything"));
    reasoner.addEvidence(t1, source);
    reasoner.addEvidence(t2, source);
    reasoner.addEvidence(t3, source);

    save(net, "0.dot");  // before inference
    reasoner.performInference();
    save(net, "1.dot");  // after inference
    reasoner.removeEvidence(t2, source); // remove "B foo C"
    reasoner.performInference();
    save(net, "2.dot"); // after removing t2

    return 0;
}
```



The exported dot-files document the state of the rete network.

After inserting the facts, but before performing inference:

![img](img/0.dot.png)

You can see all known triples in the AlphaMemory: Only the ones we added manually are present. But notice that the network already found two matches for our rule and added them to the last BetaMemory (read tokens from right to left). Those matches instantiate/trigger/execute the rule as follows:

```
(A rdfs:subClassOf B), (B rdfs:subClassOf C) -> (A rdfs:subClassOf C)
(B rdfs:subClassOf C), (C rdfs:subClassOf D) -> (B rdfs:subClassOf D)
```

This is only the first step. With those new facts more inference can be done, namely:

```
(A rdfs:subClassOf C), (C rdfs:subClassOf D) -> (A rdfs:subClassOf D)
(A rdfs:subClassOf B), (B rdfs:subClassOf D) -> (A rdfs:subClassOf D)
```

As you can see, we have created a situation where one fact `(A rdfs:subClassOf D)` is inferred on two different ways. This is the state of the network after performing inference:

![img](img/1.dot.png)



If we now remove our asserted triple `(B rdfs:subClassOf C)` the whole construct collapses and no inferences can be made. The evidences are removed one after the other, until the network is reset to this:

![img](img/2.dot.png)



> **TODO**: The reasoner does not provide an interface to access the inferred triples yet, an does not have a way to export changes (added and retracted inferences)



### RuleParser

Of course, constructing the network manually in the is cumbersome. It would be way more convenient to write rules in a simple, text based structure, and let the network be constructed automatically from those rules. This is where the  `rete::RuleParser` comes in: It takes a string as an input, parses is according to the grammar described below, and constructs a rete network from the input, reusing nodes when possible (with limitations).

The grammar is defined in EBNF, for the complete definition see `RuleParser.cpp`. The main parts of it are as follows:

```
subject   ::= variable | iriref | blank_node_label
predicate ::= variable | iriref
object    ::= variable | iriref | blank_node_label | literal

triple    ::= '(' subject predicate object ')'
triples   ::= triple (',' triple)*
rule      ::= '[' triples '->' triples ']'
rules     ::= rule+
```



As an example, some non-sense rules and the created network are shown below:

```c++
int main()
{
  RuleParser p;
  Reasoner reasoner;
  Network& net = reasoner.net();

  p.parseRules(
   "[(?a <subClassOf> ?b), (?b <subClassOf> ?c) -> (?a <subClassOf> ?c)]" \
   \
   "[(?x <subClassOf> ?y), (?y <subClassOf> ?z),(?z <knows> ?p), (?p <type> <person>)" \
   "  -> (?x <knowsPerson> ?p)]" \
   \
   "[(?foo <type> <person>), (<person> <subClassOf> ?class) -> (?foo <type> ?class)]",
   reasoner.net()
  );
  save(net, "complexExample.dot");
  return 0;
}
```

The three rules, stating that

1. If a is subclass of b, and b subclass of c then a is subclass of c, too.
2. If the conditions in 1. hold, and c (here: z) 'knows' a person p, then a (here: x) 'knows' p, too.
3. if foo is a person, and person is a subclass of some class, then foo is of that class, too.

result in the following network:

![img](img/complexExample.png)



