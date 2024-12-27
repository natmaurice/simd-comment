# SIMD Comment 

Prune comment sections of a given text ([response to reddit thread](https://www.reddit.com/r/simd/comments/1hmwukl/mask_calculation_for_single_line_comments/))


# Installation


```
git clone --recursive 
```

The only dependency is [catch2](https://github.com/catchorg/Catch2/) for tests.
Catch2 should be downloaded through `git clone --recursive`, if that's not the case then `git submodule update --init` should fix the issue.

## Building 

For Debug version
```
mkdir build
cd build
cmake .. -B Debug -DCMAKE_BUILD_TYPE=Debug -DSIMDCOMMENT_ENABLE_TESTS=ON -DSIMDCOMMENT_ENABLE_AVX512=ON -DCMAKE_CXX_FLAGS="-fsanitize=address -march=native"
make -C Debug -j 4
```

For Release version
```
mkdir build
cd build
cmake .. -B Release -DCMAKE_BUILD_TYPE=Release -DSIMDCOMMENT_ENABLE_TESTS=ON -DSIMDCOMMENT_ENABLE_AVX512=ON -DCMAKE_CXX_FLAGS="-march=native -O3"
make -C Release -j 4
```

### CMake flags

| Name                      | Description            |
|---------------------------|------------------------|
| SIMDCOMMENT_ENABLE_TESTS  | Enable Catch2 tests    |
| SIMDCOMMENT_ENABLE_AVX512 | Enable AVX512 codepath |


## Execution 

To run tests (debug mode)
```
./Debug/tests/tests
```
