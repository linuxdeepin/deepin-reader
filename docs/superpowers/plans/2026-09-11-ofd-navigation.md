# OFD Navigation Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox syntax for tracking.

**Goal:** Expose OFD outlines and clickable page links without losing destination semantics or enabling automatic actions.

**Architecture:** Add optional typed navigation values to the existing model. A pure view calculator and URI validator are shared by outline and page-link activation; rofd remains behind the OFD adapter. Preserve legacy PDF/XPS navigation when the optional value is absent.

**Tech Stack:** C++17, Qt Widgets, rofd 0.4.0 C ABI, GoogleTest, CMake.

## Execution and build boundaries

Use the approved design at `docs/superpowers/specs/2026-09-11-ofd-navigation-design.md`.
The isolated worktree is `/tmp/deepin-reader-ofd-nav.bx98sU/tree`; the baseline is
`e50708ff`. The paired rofd header/library are in the sibling `rofd/` directory.
Upstream advanced to `35cb164` / 0.4.0 after design approval, so update the package
minimum instead of retaining the design's historical 0.3.0 limitation.

Run only focused tests and the reader target, with `-j1`. Never build the
`test-deepin-reader` aggregate. The clean baseline passes all 19 OFD tests.

```sh
cmake -S tests/ofd-model -B build/ofd-model \
  -DROFD_INCLUDE_DIR=/tmp/deepin-reader-ofd-nav.bx98sU/rofd/include \
  -DROFD_FFI_LIBRARY=/tmp/deepin-reader-ofd-nav.bx98sU/rofd/librofd_ffi.so
cmake --build build/ofd-model -j1
ctest --test-dir build/ofd-model --output-on-failure
```

### Task 1: Typed targets and pure navigation calculations

**Files:** Create `reader/document/Navigation.h`, `Navigation.cpp`,
`tests/document/ut_navigation.cpp`; modify `reader/document/Model.h` and
`tests/ofd-model/CMakeLists.txt`.

- [x] Add tests first for missing/zero fields, all five modes, invalid rectangles,
  scale limits, 90/180/270-degree rotation, double-page viewport allocation,
  allowed URI resolution, rejected file/relative/unknown URI, and legacy defaults.
- [x] Run the focused target and capture the missing-behavior failure before implementation.
- [x] Implement this shared contract (namespace `deepin_reader`):

```cpp
enum class DestinationMode { XYZ, Fit, FitH, FitV, FitR };
struct NavigationDestination {
    int pageIndex = -1;
    DestinationMode mode = DestinationMode::XYZ;
    std::optional<qreal> left, top, right, bottom, zoom;
    bool isValid() const;
};
struct NavigationTarget {
    std::optional<NavigationDestination> destination;
    QUrl uri;
    bool isValid() const;
};
struct NavigationView {
    qreal scale = 1;
    QRectF focusRect; // unscaled page coordinates; zero-size rectangle for a point
};
QUrl resolveNavigationUri(const QString &uri, const QString &base = QString());
std::optional<NavigationView> navigationView(
    const NavigationDestination &destination, const QSizeF &pageSize,
    const QSizeF &viewportSize, const QPointF &currentPosition,
    qreal currentScale, qreal maximumScale, int rotationDegrees, bool twoPages);
```

  `Section` and `Link` gain `std::optional<NavigationTarget> navigation`;
  `Section` additionally gains `std::optional<bool> expanded`. Register the
  target metatype. Absent navigation retains old `Link::isValid()` behavior;
  present navigation is checked on its own, never falling back to stale fields.
  Calculate fit scale using rotated bounds and half viewport width for two pages,
  clamp to `[0.1, maximumScale]`, reject non-finite inputs and invalid FitR.
  XYZ defaults use currentPosition; zoom absent/zero preserves currentScale.
  URI resolution accepts only absolute HTTP/HTTPS with host and nonempty mailto,
  uses QUrl strict parsing and explicit Base, and never performs I/O.
- [x] Verify all tests, including `Link legacy; legacy.page = 1; EXPECT_TRUE(legacy.isValid());`.
- [x] Review the exact diff for spec compliance, then code quality, before consuming the contract.

### Task 2: OFD outlines and destinations

**Files:** Modify `reader/document/OfdModel.h`, `OfdModel.cpp`,
`tests/document/ut_ofdmodel.cpp`, `CMakeLists.txt`, `debian/control`.

- [x] Add controlled two-page OFD fixtures with page origins `(7,11)` and `(3,5)`,
  nested titles four levels deep, a non-clickable parent, named and explicit
  destinations, zero/omitted fields, invalid targets and non-CLICK actions.
  First assertion against the current stub is `ASSERT_EQ(doc->outline().size(), 1);`.
- [x] Run and observe the empty-outline failure.
- [x] Implement `Outline outline() const override` with a mutex-protected cache,
  including empty/failed results. Convert preorder nodes in reverse index order
  into the tree after validating parent-before-child indices; retain invalid-target
  nodes without a page number. Copy all strings before releasing the RAII snapshot.
  A document helper maps rofd actions/destinations to Task 1's values and lazily
  caches target-page physical rectangles. Only CLICK Goto or allowed URI actions
  become targets; preserve optional coordinates and use target-page origins.
- [x] Probe outline/destination functions at configure time and require
  `librofd-ffi-dev (>= 0.4.0)`; refresh warnings after lazy navigation queries.
- [x] Verify geometry with `EXPECT_NEAR(*target.destination->left, (13-3)*dpi/25.4, 1e-6);`,
  unresolved page targets remain absent, and repeated outline calls are stable.
- [x] Run spec review then quality review; checkpoint the scoped backend change.

### Task 3: Complete catalog and view execution

**Files:** Create `reader/sidebar/CatalogOutlineModel.h/.cpp` and
`tests/document/ut_catalogoutlinemodel.cpp`; modify `CatalogTreeView.h/.cpp`,
`reader/uiframe/DocSheet.h/.cpp`, `reader/browser/SheetBrowser.h/.cpp`,
`tests/ofd-model/CMakeLists.txt`.

- [x] Test model construction first: a targetless root with a four-level child
  must remain present, have a blank page column, retain navigation/expansion roles,
  and expose the leaf. Preserve legacy page and offset roles.
- [x] Observe a failing model test, then implement reusable iterative tree-to-item
  population. Bind both columns to the same target. CatalogTreeView uses it,
  expands OFD defaults, and does not activate targets while populating or syncing.
- [x] Add `bool navigateTo(const deepin_reader::NavigationTarget &)` to SheetBrowser
  and a forwarding DocSheet method. Validate page count before changing state.
  Compute currentPosition by mapping viewport origin to the current page and
  dividing by current scale. Apply Task 1's view, call setScaleFactor, and map
  focusRect at the actual resulting scale through the target item's scene transform
  before setting scrollbars and notifying the current page.
- [x] For a typed URI, revalidate, display the final URL in SecurityDialog, and
  call QDesktopServices only after Accepted. Share this path with Task 4.
- [x] Enable PREVIEW_CATALOG for OFD. Typed outline actions execute on explicit
  click or keyboard activation exactly once, not currentChanged; legacy paths
  remain unchanged. Stored expansion state, including all-collapsed state, wins
  over defaults when the catalog is opened lazily.
- [ ] Verify model/calculator tests and reader compilation; exercise actual
  outline activation, rotation and cancellation in a controlled app window.
- [x] Spec review then quality review; checkpoint directory/navigation integration.

### Task 4: Page link adapter and shared activation

**Files:** Modify `reader/document/OfdModel.h/.cpp`,
`tests/document/ut_ofdmodel.cpp`, `reader/browser/SheetBrowser.cpp`, `CMakeLists.txt`.

- [x] Add links to controlled fixtures with separated regions, overlaps,
  transformed bounds, CLICK/PO/DO events, URI/Base and GotoA actions.
  First assertion against the stub is `EXPECT_TRUE(page->getLinkAtPoint(hit).isValid());`.
- [x] Run and observe that missing-link failure.
- [x] Implement a per-page immutable Qt-owned link cache guarded during init.
  Preserve source order, use the first supported hit, and union independent
  rectangles as paths without filling the gaps. Copy borrowed data before freeing
  the snapshot. Empty/failed lists are cached. Reuse Task 2 conversion and refresh
  warnings after rofd_page_get_links. No adapter action executes external code.
- [x] `SheetBrowser::jump2Link` calls navigateTo when `link.navigation` exists;
  otherwise preserves the old PDF/XPS path. Keep resolved URI text for hover tips.
- [x] Extend CMake symbol checks and tests for gaps, deterministic overlap,
  unsafe targets, failure isolation from outlines and repeated queries.
- [x] Run spec review then quality review; checkpoint page-link integration.

### Task 5: Integrated validation and delivery

**Files:** Update `tests/ofd-model/README.md` and this checklist with actual results.

- [x] Run `git diff --check`, all focused tests, and inspect linked rofd SONAME.
- [x] Build only `deepin-reader -j1`, reusing the existing PDFium build where possible.
- [ ] Verify real window catalog depth/activation, page link hover/click, rotation,
  scale changes and external-link cancellation. Do not visit test URLs.
- [x] Check legacy PDF/XPS targets keep their prior path and no new automatic actions
  occur on opening a document, restoring state or selecting a catalog item.
- [x] Review the entire diff after task reviews, report exact evidence and any
  unverified conditions. Keep outline/navigation and page links separable in history;
  do not push or merge to main without the user's request.

## Execution evidence (2026-09-11)

- Paired rofd snapshot: `35cb164`, version 0.4.0; the source repository was not
  modified. The original reader worktree remains on `ofd_support`.
- Task 1: 22 navigation/legacy tests passed after the missing-behavior RED run.
  Task 2: five new outline tests first failed on the empty inherited outline.
  Task 3: six catalog-model tests passed after their RED runs.
- Task 4: six link tests first failed on the inherited empty link. The additional
  missing-owner test reproduced a crash before the guard was added. The GUI
  regression also failed with the backend connected but the old browser route:
  a real mouse click changed pages but lost the destination's Fit scale.
- Fresh focused test run: **59 tests passed**, seven suites, no skips.
- Fresh `deepin-reader -j1` build succeeded with `BUILD_TESTS=OFF` and the existing
  PDFium shared library. No aggregate test build or OOM occurred.
- Reproducible real-widget checks: **247 assertions passed** through
  `python3 tests/ofd-model/run_navigation_smoke.py build/reader-local`.
  Includes deep catalogs, inert selection, explicit Enter/click activation,
  all five modes at four rotations and two layouts, omitted coordinates,
  page-link hover and actual mouse press/release, Fit semantics, and URI cancel.
  External test URLs were intercepted and never opened.
- `ldd` resolves `librofd_ffi.so.0` to the paired sibling `rofd/` snapshot and
  PDFium to the original reader's existing shared build. A negative configure
  check with the old rofd library correctly failed on missing navigation APIs.
- `git diff --check` passes. Spec and quality reviews passed for all four
  implementation tasks and the integrated change, with no critical or important
  findings. Direct fitting-ratio overflow/underflow coverage remains an optional
  test enhancement; the bounded implementation was reviewed.
- Scoped implementation commits: `f5be344a` (typed targets/calculations),
  `db340c99` (outline adapter), `d4336b10` (catalog/navigation UI and widget runner),
  `b2431562` (page links and shared activation).

### Validation limit

The real-widget checks use Qt's **offscreen** platform. Visible desktop-window
acceptance remains unchecked above: the native UI automation runtime was not
available. The aggregate test executable was deliberately not built, following
the user's reader-only build constraint. No main-branch merge or remote push
has been performed.
