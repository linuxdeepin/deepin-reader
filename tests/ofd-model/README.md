# Focused OFD adapter checks

This independent CMake project runs the OFD adapter, navigation calculator and
catalog-model tests against a shared rofd library. It does not link PDFium or the
monolithic `test-deepin-reader` executable. The `.cc` entry point is intentionally
outside the main test project's `.cpp`/`.h` source glob.

```sh
cmake -S tests/ofd-model -B build/ofd-model \
  -DROFD_INCLUDE_DIR=/path/to/rofd/crates/rofd-ffi/include \
  -DROFD_FFI_LIBRARY=/path/to/lib/librofd_ffi.so
cmake --build build/ofd-model -j1
ctest --test-dir build/ofd-model --output-on-failure
```

Use matching rofd **0.4.0 or newer** headers and library, including region,
metadata, warnings, outline/destination and page-link APIs.
Its runtime SONAME (`librofd_ffi.so.0`) must resolve to the same library.
Qt Widgets, DTK Core, Cairo, Google Test and CMake are required. Fixtures are
created in temporary directories using `cmake -E tar --format=zip`.

Coverage includes full and tiled rendering, nonzero physical page origins,
small tiles on canvases larger than 4 GiB, pre-allocation raster limits,
real-invoice pixel comparisons, search/selection, metadata/DocID, missing or
invalid dates, and fresh warning snapshots after lazy page and annotation loads.
Navigation checks cover target-page physical origins, optional coordinates,
five destination modes, rotation/two-page fitting, safe URI resolution, deep
catalogs, expansion restoration, independent link regions and cached failures.

## Real-widget smoke checks

After building **only the reader application**, the following optional check
reuses its objects and link libraries with a small test entry point:

```sh
cmake --build build/reader-local --target deepin-reader -j1
python3 tests/ofd-model/run_navigation_smoke.py build/reader-local
```

The runner requires a completed, up-to-date **Unix Makefiles** CMake build with
OFD enabled. Use the shared PDFium/rofd configuration to keep linking small; this
does not build the aggregate `test-deepin-reader` target. It compiles only
`navigation_smoke.cc`, discovers moc from CMake, and isolates temporary files and
XDG application state. The source uses `.cc` to stay out of the aggregate test
source glob.

It exercises real DocSheet, CatalogTreeView, SheetBrowser and SecurityDialog
objects with the Qt **offscreen** platform: deep catalog/explicit activation,
selection without navigation, all-collapsed restoration, five modes at four
rotations and both layouts, omitted coordinates, page-link hover/activation,
and cancellation of external links. Test URLs are intercepted and never opened.
It also covers navigation during the mainline reading-state restore guard:
catalog and page-link activation, timer expiry, tab return, already-visible
destinations, and preservation of normal restoration for invalid/external targets.
Actual thumbnail painting is checked in light/dark themes and all four eye modes,
including rotated, high-DPI source pixmaps. These checks compare content pixels;
they do not verify border styling or desktop repaint timing.
These are in-process widget checks, not a visible desktop-window acceptance test.

## Focused thumbnail filter checks

The smaller sidebar project checks the production thumbnail filter/cache without
linking the reader, PDFium or rofd:

```sh
cmake -S tests/sidebar-appearance -B build/sidebar-appearance
cmake --build build/sidebar-appearance -j1
ctest --test-dir build/sidebar-appearance --output-on-failure
```

It compares pixels with the mainline `NightFilter`, including transparent input,
device-pixel ratio, cache invalidation and filter-before-rotation behavior.
Thumbnail image-object masks are not yet supplied: thumbnails use the mainline
filter's empty-mask fallback, not the main page's photo-preservation path.

## Rebuilding after the master rebase

Master `9acd6e5a` includes the new `DPdfPage::imageObjectRects(int, int)` API.
When reusing a locally built shared PDFium wrapper, update that wrapper as well
as its header; an older shared library will fail to link despite a clean rebase.
The existing underlying PDFium static library can be reused if its sources and
build configuration have not changed. Keep `BUILD_TESTS=OFF` and build only the
`deepin-reader` target with `-j1`; the focused checks above remain independent.

Validation after this rebase on Qt 6/rofd 0.4.0: reader-only build, 59 model tests,
41 filter/cache checks and 294 real-widget checks passed. The qmake path and
visible desktop-window acceptance were not part of this validation.
