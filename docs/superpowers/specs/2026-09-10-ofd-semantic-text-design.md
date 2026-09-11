# OFD Semantic Text Integration Design

## Scope

Connect the semantic text API already available in the current `rofd` C ABI to
the existing `deepin_reader::Page` abstraction. The change implements OFD page
text extraction, text search, and selectable character geometry. It does not
add OFD-specific UI or attempt metadata, outline, links, annotations, native
writing, or tiled rendering, because those still require new `rofd` APIs.

## Architecture

`OfdPage` remains the only adapter between `deepin-reader` page semantics and
`rofd`. It stores the physical page rectangle returned by
`rofd_page_get_size_mm`, then uses small private conversion helpers for physical
millimetres and the logical page-pixel coordinate system used by the reader.

Each public query creates and frees its own immutable `rofd` result handle. No
mutable semantic cache is shared between render and search worker threads.
This follows the existing adapter structure and the C ABI's concurrent
read-only handle contract.

## Data Flow

- `text(rect)` converts the requested logical-pixel rectangle to physical-page
  millimetres and calls `rofd_page_get_text_for_area`. A null rectangle requests
  the full canonical page text through `rofd_page_get_text`.
- `search(query, matchCase, wholeWords)` initializes
  `rofd_find_options_t`, maps the two reader flags to `ROFD_FIND_*`, and turns
  every match rectangle into one `PageSection` containing one `PageLine`.
- `words()` gets canonical UTF-8 text plus its layout snapshot. Every
  non-synthesized scalar with valid geometry becomes one `Word`; its text is
  decoded from the canonical string using the layout's byte offset and length.
  One scalar per `Word` matches the existing selection overlay contract.

## Coordinates

The reader expresses page text geometry in logical pixels at the document's X
and Y logical DPI. `rofd` expresses geometry in physical-page millimetres.
Conversions include the physical page rectangle's X/Y origin:

```
mm.x = physical.x + px.x * 25.4 / xDpi
px.x = (mm.x - physical.x) * xDpi / 25.4
```

The same formula applies independently to Y, width, and height.

## Error Handling

Invalid handles, empty search input, invalid rectangles, invalid UTF-8 spans,
and failed C ABI queries return the empty result expected by `Page`. Owned
`rofd` handles are always freed. ABI failures are logged with page index,
status, and the optional error message.

## Verification

Fixture-backed tests prove full and area text extraction, case-sensitive and
whole-word search behavior, search rectangles, and selectable character text
and geometry. The tests are first run against the current stubs to demonstrate
the intended failures, then rerun against the implementation using a local
build of the latest `rofd main` semantic ABI.
