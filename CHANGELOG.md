# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- support arithmetic expression
- support LIKE
- support MULTI* geometry
- support IS NULL and IS NOT NULL
- support BETWEEN
- support CASEI and ACCENTI
- support some lower case operators
- add test function to test all unsupported examples

### Changed
- reject 3d bbox

### Deprecated
- 

### Removed
- 

### Fixed
- 

### Security
- 

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
