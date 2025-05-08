# Cql2Cpp
Cql2Cpp is a C++ library that implements the parsing and evaluation of OGC CQL2(Common Query Language).
https://www.ogc.org/publications/standard/cql2/

![demo image](demo.png)

# History
Syrius Robotics has been focused on developing applications for indoor robots, which led us to create an indoor geographic information system. At the end of 2024, while working on this system, we recognized the need for a filter language to help robots query indoor features more efficiently. Around that time, OGC CQL2 had just been released, but there were no existing C++ libraries available for it. As a result, Syrius Robotics decided to develop their own solution.

# Progress and Plan
As of the end of 2024, only a limited number of syntax features have been developed, including the most basic field operations and a small set of spatial operations. This library is not recommended for use in any production environment or product.

We plan to complete all the requirements specified in Requirement 1 "/req/basic-cql2/cql2-filter"([Clause 6.2](https://docs.ogc.org/is/21-065r2/21-065r2.html#basic-cql2_filter-expression)) of CQL2, along with a subset of spatialPredicate, by early 2025.

In the second quarter of 2025, we completed the parsing of almost all features except those related to time and Unicode encoding. Additionally, we plan to translate the parsing results into the WHERE clause of the SQL language.

| feature | parsing | evaluation | sql |
| ---- | ---- | ---- | ---- |
| bool expression | &check; | &check; | &#10008; |
| isInList predicate | &check; | &check; | &#10008; |
| array predicate and all array functions | &check; | &check; | &#10008; |
| comparison operator | &check; | &check; | &#10008; |
| geom expression (including BBOX) | &check; | &check; | &#10008; |
| spatial predicate | &check; | &check; | &#10008; |
| property name | &check; | &check; | &#10008; |
| function | &check; | &check; | &#10008; |
| isLike predicate | &check; | &#10008; | &#10008; |
| isBetween predicate | &check; | &#10008; | &#10008; |
| numeric expression | &check; | &#10008; | &#10008; |
| isNull predicate | &check; | &#10008; | &#10008; |
| pattern expression | &check; | &#10008; | &#10008; |
| non-ascii charactor literal | &#10008; | &#10008; | &#10008; |

you can find a full list of features supported here:
https://github.com/orgs/IndoorSpatial/projects/1/views/1

# Dependencies
1. flex: lexer
2. bison: parser
3. gflags: for cli tool
4. glog: for logging
5. geos: for geometry and spatial predicate and geojson
6. gtest: for unit test

# Build

## build locally
```bash
sudo apt-get install -y build-essential g++-aarch64-linux-gnu
sudo apt-get install -y flex bison
sudo apt-get install -y libgflags-dev libgoogle-glog-dev libgtest-dev libgeos++-dev
mkdir build && cd build
cmake ..
make -j16

```

## build by conan
```bash
sudo apt-get install -y build-essential g++-aarch64-linux-gnu
sudo apt-get install -y flex bison
pip install conan
cd cql2cpp
conan install . --build=missing --output-folder build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=build -DCMAKE_MODULE_PATH=build
make -j16
```

## build in docker
```bash
docker run --rm -it -v path/to/cql2cpp:/home/ubuntu/cql2cpp/ kunlinyu/cql2cpp:latest bash
docker$ cd cql2cpp
docker$ mkdir build && cd build
docker$ cmake ..
docker$ make -j16
```
