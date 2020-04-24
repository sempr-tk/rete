# Changelog

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
