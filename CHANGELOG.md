# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- 

### Changed
- 

### Deprecated
- 

### Removed
- 

### Fixed
- 

### Security
- 

## [0.3.0] -2025-05-14
### Added
- sqlite3 and spatialite as dependencies to verify SQL
- Add iterator for ast node to loop over all property name
- Add ConvertToSQL() in top level Cql2CPP class
- Add unit tests for SQL
- Use argparse to parse command line arguments instead of gflags
- Convert arithmetic, is null, like, between to SQL
- Convert array predicate to SQL
### Fixed
- Fix bug about ST_MakeEnvelope
- Fix bug about using "-" as prefix of number

## [0.2.1] - 2025-05-08
### Fixed
- Fix error in demo.png

## [0.2.0] - 2025-05-08
### Added
- parse arithmetic expression
- parse LIKE
- parse MULTI* geometry
- parse IS NULL and IS NOT NULL
- parse BETWEEN
- parse CASEI and ACCENTI
- parse some lower case operators
- add test function to test all unsupported examples
### Changed
- reject 3d bbox

## [0.1.0] - 2025-01-04
### Added
- Initial release of the project
- boolean expression
- isInList predicate
- array predicate and all array functions
- binary comparison predicate and all comparison operator
- geom expression (including BBOX)
- spatial predicate (only S_INTERSECTS)
- property name
- function
