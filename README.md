# cql2cpp
c++ library for OGC CQL2(Common Query Language)

## build
```bash
conan install . --output-folder build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=build -DCMAKE_MODULE_PATH=build
```
