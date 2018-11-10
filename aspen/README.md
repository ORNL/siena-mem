Aspen Performance Modeling Suite
================================

Aspen is a domain specific language (DSL) for performance modeling.
This repository contains the official parsers for the Aspen
application model language and the Aspen abstract machine 
model (AMM) language, built as a library with query support.
It also contains a suite of examples and tools to help understand
application and machine models, both separately and when convolved
together (for example, to return a runtime prediction).

Repository Structure
--------------------

#### Core Library

+ `aspen/`
  - contains the core language, parser, and query methods
+ `lib/`
  - generated Aspen libraries are put here
+ `config-site/`
  - contains build settings for common operating systems and dev machines
+ `config/`
  - contains internal files used by the library and configure machinery

#### Aspen Based Tools

+ `examples/`
  - contains simple C++ tools for parsing and querying models
+ `tools/`
  - contains more complex and robust C++ Aspen query tools
+ `test/`
  - contains the regression test suite (including specialized tools)

#### Interface Modules 

The native interface to Aspen is C++, but there are modules which
support interfaces in other languages.

+ `python/`
  - contains the Aspen Python module (note: depends on Boost::Python)
+ `java/`
  - contains the Aspen Java package (note: uses JNI)
+ `c/`
  - contains the Aspen flat C interface

#### Models

+ `models/`
  - contains a set of example application models
+ `models/machine/`
  - contains a set of example machine models

#### Documentation

Most documentation is not in the repository.  (See the
wiki instead.)  Specialized documentation generated from
the source tree is still in the repository for now.

+ `docs/`
  - contains special documentation, e.g., the Aspen grammar

Contact Information
-------------------

Email aspen-dev (at) elist (dot) ornl (dot) gov to reach the
Aspen developers.