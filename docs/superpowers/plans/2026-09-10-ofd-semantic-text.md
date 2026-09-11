# OFD Semantic Text Integration Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Make OFD documents participate in the reader's existing text extraction, search, selection, and copy workflows using the current `rofd` semantic C ABI.

**Architecture:** Keep semantic adaptation inside `OfdPage`. Convert between physical-page millimetres and reader logical pixels at the boundary, use owned result handles per query, and expose character layout through the existing `Word` list.

**Tech Stack:** C++17, Qt Core/Gui, GoogleTest, `rofd` stable C ABI

---

### Task 1: Fixture-backed semantic behavior

**Files:**
- Modify: `tests/document/ut_ofdmodel.cpp`

- [ ] **Step 1: Add failing tests**

Add tests that load `normal.ofd`, require non-empty full-page text, extract text
from the first returned word rectangle, verify case-sensitive and whole-word
search filtering, and require non-empty `words()` entries with valid geometry.

- [ ] **Step 2: Run the focused tests and verify RED**

Run:

```bash
QT_QPA_PLATFORM=offscreen build-semantic/tests/test-deepin-reader \
  --gtest_filter='TestOfdModel.semantic*'
```

Expected: the text, search, and words assertions fail because `OfdPage` still
returns empty values.

### Task 2: Coordinate-safe semantic adapter

**Files:**
- Modify: `reader/document/OfdModel.h`
- Modify: `reader/document/OfdModel.cpp`
- Test: `tests/document/ut_ofdmodel.cpp`

- [ ] **Step 1: Store physical page geometry and declare `words()`**

Add `QList<Word> words() override`, store the page's complete `rofd_rect_t`, and
declare private `toMillimetres` and `toPixels` rectangle helpers.

- [ ] **Step 2: Implement owned-string extraction**

Use `rofd_page_get_text` for a null rectangle and
`rofd_page_get_text_for_area` otherwise. Convert the borrowed UTF-8 bytes with
an explicit length before freeing `rofd_string_t`.

- [ ] **Step 3: Implement semantic search**

Initialize `rofd_find_options_t`, set `ROFD_FIND_CASE_SENSITIVE` and
`ROFD_FIND_WHOLE_WORDS` as requested, enumerate matches, convert each match
rectangle, and append `PageSection{PageLine{QString(), rect}}`.

- [ ] **Step 4: Implement selectable character layout**

Acquire canonical text and a layout snapshot, skip synthesized separators and
zero-area geometry, validate every UTF-8 span, and append one `Word` per scalar
using the converted rectangle.

- [ ] **Step 5: Run focused tests and verify GREEN**

Run the semantic test filter and expect every new test to pass.

### Task 3: Build contract and regression verification

**Files:**
- Modify only if required by configure checks: `CMakeLists.txt`
- Modify: `debian/control` only after the semantic package version is known

- [ ] **Step 1: Configure against local current `rofd main`**

Build `rofd-ffi` into an isolated `/tmp` target, then configure a fresh reader
build using `-DROFD_ROOT=/home/hualet/projects/hualet/rofd` and the matching
library directory.

- [ ] **Step 2: Build the reader and tests**

Run the normal CMake build for `test-deepin-reader` and `deepin-reader`.

- [ ] **Step 3: Run all OFD model tests**

Run:

```bash
QT_QPA_PLATFORM=offscreen build-semantic/tests/test-deepin-reader \
  --gtest_filter='TestOfdModel.*'
```

Expected: all OFD model tests pass.

- [ ] **Step 4: Run the complete document-model test subset**

Run the repository's document model test filters and report any unrelated
pre-existing failures separately.

- [ ] **Step 5: Inspect the final diff**

Confirm only the OFD adapter, OFD tests, and approved build-contract changes are
present. Leave implementation changes uncommitted until the user requests a
commit.
