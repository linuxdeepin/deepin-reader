# Focused OFD adapter checks

This independent CMake project runs `tests/document/ut_ofdmodel.cpp` against the
real reader adapter and a shared rofd library. It does not link PDFium or the
monolithic `test-deepin-reader` executable. The `.cc` entry point is intentionally
outside the main test project's `.cpp`/`.h` source glob.

```sh
cmake -S tests/ofd-model -B build/ofd-model \
  -DROFD_INCLUDE_DIR=/path/to/rofd/crates/rofd-ffi/include \
  -DROFD_FFI_LIBRARY=/path/to/lib/librofd_ffi.so
cmake --build build/ofd-model -j1
ctest --test-dir build/ofd-model --output-on-failure
```

The library must provide the region and metadata APIs added in rofd
commits `56da925` and `6db9bac`; the initial 0.3.0 release does not include them.
Its runtime SONAME (`librofd_ffi.so.0`) must resolve to the same library.
Qt Widgets, DTK Core, Cairo, Google Test and CMake are required. Fixtures are
created in temporary directories using `cmake -E tar --format=zip`.

Coverage includes full and tiled rendering, nonzero physical page origins,
small tiles on canvases larger than 4 GiB, pre-allocation raster limits,
real-invoice pixel comparisons, search/selection, metadata/DocID, missing or
invalid dates.
