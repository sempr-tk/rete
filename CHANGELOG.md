# Changelog

## [0.8.0] - 2020-11-19

- Implemented global constants which can be defined at the top of your rules,
  but after @PREFIX definitions:
  ```
    $varA : "stringvalue"
    $varB : 42
    $varC : rdf:type
  ```
- Fix TokenGroupAccessors to allow access to variables which are used in
  successive GROUP BY statements.

## [0.7.1] - 2020-11-06

- Allow a single colon in effect names, e.g. foo:bar(...), for namespacing
  puposes. This was already allowed in builtin names.

## [0.7.0] - 2020-10-09

- Improved error messages for illformed rules
- Added `GROUP BY (?var1 ?var2 ...)`: Gathers all sub-matches so far, putting
  those with equal values on the given variables in the same group. This allows
  you to perform operations on collections of facts.
  See `test/GroupByBuiltins.cpp` for examples.
- Improved math builtins to support multiple number types; added NumberToNumber
  conversion utility for that sake.
- Added `makeSkolem(?skolem ?val1 ?val2 ...)`, which creates unique identifiers
  from a combination of values (strings and numbers supported).


## [0.6.1] - 2020-08-02

- Workaround: Compiler bug during template method overload resolution at
  static initialization. There were conflicts between type traits in cereal
  (used in sempr, not here) and rete::util::is\_std\_to\_string\_valid.

## [0.6.0] - 2020-08-01

- Fixed incorrect handling of strings/triples in the reasoner/rule parser.
  String constants in rules are now correctly parsed without the quotation
  marks, while the contents of triples are unquoted when interpreted as string.
  Added a "TriplePart" wrapper for strings to identify strings that can be used
  as part of a triple as they are.
  NOTE: Obviously, this is a breaking change.
- Introduced concept of AccessorConversion: Helper classes that convert any of
  a given set of interpretations into a single data type. Can be used by nodes
  to provide compatibility to a range of types while still respecting the
  preferred interpretation of the source data.
- Require: C++14.
- util::to\_string now defaults to an empty string if std::to\_string cannot be
  used.

## [0.5.0] - 2020-08-31

- Reworked the accessor system. How to access a value in a WME and what type of
  value can be accessed are now properly decoupled into Accessor and
  Interpretation.
  NOTE: This is a breaking change in the fundamental structure of this project!


## [0.4.1] - 2020-05-20

- Fixed a segfault that would occur after removing and adding rules again,
  due to the BetaComparator trying to access expired weak pointers.
- Added a bit of cleanup-code: Nodes now remove themselves from their parents
  children-lists upon destruction.

## [0.4.0] - 2020-05-12

- Added a visitor interface for nodes (NodeVisitor)
- Added a toString() method to nodes as an alternative to the getDOTAttr,
  to return a string representation without the DOT formatting stuff.
  NOTE: getDOTAttr and getDOTId should be removed, and creating a DOT
        representation be moved into a special visitor. Maybe later.

## [0.3.0] - 2020-04-30

- Removed vector to store ProductionNodes in from the rete::Network class
- RuleParser now returns vector of ParsedRules, which contain the
  ProductionNodes and additional information.
  NOTE: This is a breaking change! If you ignore the return value of
        rete::RuleParser::parseRules the constructed ProductionNodes run out
        of scope and the rules will be immediately deconstructed again.

## [0.2.7] - 2020-04-24

- Added "print" as a rule effect

## [0.2.6] - 2020-03-27

- Fixed a bug in the AgendaItem-scheduling that prevented optimizations like
  "no UPDATE if there is an unprocessed ASSERT" to take effect. As updates are
  also scheduled before asserts, this could result in processing an update of
  a WME before its actual assertion.

## [0.2.5] - 2020-03-23

- Added VERSION (and implicitely SOVERSION) to the generated libraries. This
  allows dpkg-shlibdeps in other projects to properly find this library as an
  dependency. Note that VERSION/SOVERSION is set to MAJOR.MINOR as I will not
  make any promises for a stable ABI between even minor versions.

## [0.2.4] - 2020-03-20

- Changed rule grammar to allow "<>" in rule effects
- Added changelog

## [0.2.3] - 2020-03-16

- Allow builtins to set the isComputed flag of WMEs. With this, builtins may
  override the leftActivate(...) method instead of process(...) for more
  flexibility

## [0.2.2] - 2020-03-12

- Added perfect argument forwarding to RuleParser::registerNodeBuilder.
  No need to explicitely create unique pointers to builders that have no default
  constructor anymore

## [0.2.1] - 2020-03-03

- Added dependencies to the created debian package

## [0.2.0] - 2020-03-02

- Note: v0.1.0 was kept for a very long time during heavy development.
  Some features added between the first v0.1.0 and 0.2.0 are:
  - Accessors, GenericJoin
  - Builtins
  - noValue
  - Mutable WMEs (UPDATE flag)
  - Runtime modifications of the network
  - Complex noValue constructs
- v0.2.0 explicitely adds creation of debian packages

## [0.1.0] - 2018-09-10 (creation of pkgconfig file)

- Initial, unreleased version
