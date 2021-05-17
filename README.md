[![pipeline status](https://git.ni.dfki.de/sempr/rete/badges/master/pipeline.svg)](https://git.ni.dfki.de/sempr/rete/commits/master)

# Rete Algorithm & Reasoner in C++

Rete is an algorithm for efficient pattern matching. Patterns are represented by a graph structure in which every node implements a small check on the incoming data. By storing intermediate results in memory nodes the algorithm is able to work with incremental updates. Also, nodes can be reused for multiple patterns, which is one reason for its efficiency.

This project provides an implementation of the rete algorithm in C++. The algorithm is not new, but many existing implementations are rather small, unfinished hobby projects, are hidden inside proprietary software or released under a licence I'd rather not use (e.g. GPL).

Furthermore this project contains a rule based forward reasoner that uses the mentioned rete implementation to check the preconditions of its rules.

For a detailed documentation please **[visit the WIKI](https://git.ni.dfki.de/sempr/rete/wikis/home)**.



## Install

This project is _mostly_ independent. The only dependencies are openssl (for a
hash function -- shouldn't be too critical as it is probably installed on most
systems anyway), and the `pegmatite` parser library:

```
git clone git@git.ni.dfki.de:sempr/Pegmatite.git
cd Pegmatite
mkdir build && cd build
cmake ..
make -j8 && make install
```

> Note: Make sure that your `$PKG_CONFIG_PATH` is configured correctly so that pegmatite can be found.

Afterwards you can install rete:

```
git clone git@git.ni.dfki.de:sempr/rete.git
cd rete
mkdir build && cd build
cmake ..
make -j8 && make install
```



## Features

- C++ implementation of the rete algorithm
  - With support for **adding, removing** and **updating** data
  - Fully extensible: Feel free to implement your own nodes and datatypes
- A RuleParser which creates rete networks from a textual representation of rules
  - No need to manually setup your network, let the parser do the work
  - Fully extensible: It is designed to allow (more or less) easily adding support for custom datatypes, conditions, operations and effects
- A Reasoner that adds on top of this:
  - Tracking of (possibly multiple) origins of a fact/bit of knowledge/data blob
  - Support for non-monotonic inference
    - Removing data only if none of the rules to infer it can be applied anymore
    - Checks for cyclic inferences which do no longer ground in asserted facts
    - Incremental (or decremental?) updates, even when updating or removing data
  - Explaining inferred data: Since it tracks which facts were used in which rules to create which data, this information can be traversed to generate e.g. a graphical view on the paths of inference
- Support for RDF triples as the first implemented datatype to get you started
- Special builtins / syntax:
  - Math operators (so far: `add`, `multiply`, `divide`)
  - Comparisons (`<`, `<=`, `==`, `!=`, `>=`, `>`)
  - `noValue` to check for non-existence patterns
  - `makeSkolem` creates an identifier from the given arguments which can be
     used e.g. as the id of a semantic entity in triples.
  - `GROUP BY` to aggregate partial matches and perform special operators on the result, e.g. to compute the sum of all values or a union of all geometries with the specified properties.
  - Simple way to add custom callbacks in the rules effects, using `makeCallbackBuilder` and `CallbackEffect`.



## Limitations

- Not (yet?) optimized for speed, but focused on features so far
- No notion of negations (despite the `noValue` keyword)
- Work in progress. Expect bugs.
