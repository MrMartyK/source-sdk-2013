# Phase 0: Foundation - Week 1 Implementation Guide

**Status**: Ready to execute
**Duration**: 5-7 days
**Goal**: Establish TDD infrastructure, coding standards, and build foundation for Source 1.5

---

## Objectives

By end of Week 1, we will have:

1. **CI/CD pipeline** building Windows x64 successfully
2. **Test infrastructure** with Catch2 and passing unit tests
3. **Coding standards** enforced via clang-format
4. **Modular architecture** skeleton (framework/, engine_bridge/)
5. **Golden content** documented for parity testing
6. **CMake build system** coexisting with VPC

---

## Completed Artifacts

### 1. CI/CD Configuration ✓

**File**: `.github/workflows/build-windows.yml`

**What it does**:
- Builds Source SDK using VPC + MSVC 2022
- Runs basic smoke tests
- Uploads build artifacts (DLLs, PDBs)
- Placeholder for static analysis (clang-tidy, cppcheck)

**Next steps**:
- Push to GitHub, verify green build
- Add Linux x64 workflow (Steam Runtime)
- Wire up parity tests when maps ready

### 2. Coding Standards ✓

**Files**:
- `.clang-format` - Enforces Valve conventions (tabs, K&R braces, 120 char lines)
- `docs/CODING_STYLE.md` - Human-readable style guide

**What it enforces**:
- Naming: `CMyClass`, `m_MyMember`, `g_MyGlobal`
- Hungarian notation for clarity (`nCount`, `flValue`, `bFlag`)
- Include order: local → public → tier → system → memdbgon.h last
- C++17 allowed, but no exceptions/RTTI, prefer tier1 containers over STL

**Next steps**:
- Run `clang-format` on new code before commits
- Add pre-commit hook to auto-format

### 3. CMake Build System ✓

**Files**:
- Root `CMakeLists.txt` - Project config, options, compiler flags
- `src/framework/CMakeLists.txt` - Framework library
- `src/engine_bridge/CMakeLists.txt` - Engine adapters
- `tests/CMakeLists.txt` - Test suite with Catch2

**Options**:
- `SOURCE15_BUILD_TESTS=ON` - Enable unit tests (default ON)
- `SOURCE15_STEAMAUDIO=OFF` - Steam Audio integration (default OFF for now)
- `SOURCE15_BUILD_DX11=OFF` - DX11 renderer (defer to Week 6+)
- `SOURCE15_IWYU=OFF` - Include-what-you-use analysis

**Build commands**:
```bash
# Configure
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build --config Release

# Run tests
cd build && ctest --output-on-failure
```

**Next steps**:
- Test build on clean machine
- Add Linux CMake presets
- Integrate with GitHub Actions

### 4. Framework Library ✓

**Location**: `src/framework/`

**Purpose**: Engine-agnostic utilities that tools and game code can both use

**Modules created**:
- `string_utils.h/.cpp` - Safe string operations, path manipulation
- `math_extra.h/.cpp` - Math beyond mathlib (placeholders)
- `serialization.h/.cpp` - Binary/text serialization (placeholders)

**Design principles**:
- No game-specific code
- No dependencies on engine binaries
- Can build in tools, tests, game DLLs
- Namespace: `S15::`

**Next steps**:
- Add more utilities as needed during refactoring
- Keep it minimal - don't prematurely extract

### 5. Engine Bridge ✓

**Location**: `src/engine_bridge/`

**Purpose**: Adapters for CreateInterface pattern, allows tools/tests to talk to engine

**Modules created**:
- `filesystem_bridge.h/.cpp` - Wraps IFileSystem
- `materialsystem_bridge.h/.cpp` - Wraps IMaterialSystem
- `console_bridge.h/.cpp` - Wraps cvar/console commands

**Status**: Stubbed (Init() returns false for now)

**Next steps**:
- Implement CreateInterface calls when needed
- Add mock implementations for unit tests
- Use in Hub for IPC commands

### 6. Test Infrastructure ✓

**Location**: `tests/`

**Framework**: Catch2 v3.5.1 (fetched via CMake)

**Tests created**:
- `test_string_utils.cpp` - 20+ test cases for string utilities

**Test commands**:
```bash
# Build and run
cmake --build build --target run_tests

# Or via CTest
cd build && ctest -V
```

**Coverage**:
- All framework code must have unit tests
- Use `TEST_CASE` and `SECTION` for organization
- Run tests in CI on every commit

**Next steps**:
- Add tests for math_extra, serialization
- Add parity tests (headless map launches)
- Integrate Tracy profiler for perf tests

### 7. Golden Content Plan ✓

**File**: `docs/GOLDEN_CONTENT.md`

**Maps selected** (from your Steam install):
1. `background01.bsp` (HL2) - Minimal smoke test
2. `d1_canals_01.bsp` (HL2) - Outdoor scene, water, props
3. `cp_badlands.bsp` (TF2) - Complex MP map

**New test content to create**:
- `benchmark_pbr.vmf` - PBR shader validation
- `benchmark_audio.vmf` - Steam Audio occlusion test
- `benchmark_combined.vmf` - Full stress test

**Parity methodology**:
- Visual: Screenshot diff (SSIM/RMSE <2%)
- Performance: Frametime budgets (RTX 3060 @ 1080p: 144 FPS)
- Audio: FFT analysis for occlusion differences

**Next steps**:
- Set up Git LFS for golden screenshots
- Create benchmark maps in Hammer
- Write Python script for image diffing

---

## Week 1 Detailed Task Breakdown

### Day 1: CI and Build Verification

**Tasks**:
- [ ] Push `.github/workflows/build-windows.yml` to GitHub
- [ ] Verify build succeeds (SDK compiles via VPC)
- [ ] Fix any build errors from CI environment
- [ ] Add build status badge to README.md

**Success criteria**: Green build on GitHub Actions

### Day 2: CMake Integration

**Tasks**:
- [ ] Test CMake build locally (Windows)
- [ ] Build `lib_framework` successfully
- [ ] Build `lib_engine_bridge` successfully
- [ ] Build `source15_tests` and run

**Success criteria**: All CMake targets build, tests pass locally

### Day 3: Code Quality Tools

**Tasks**:
- [ ] Run `clang-format` on new code
- [ ] Set up clang-tidy baseline config
- [ ] Add IWYU pragmas to framework headers
- [ ] Document coding style violations to fix

**Success criteria**: No new formatting issues, baseline established

### Day 4: Test Coverage

**Tasks**:
- [ ] Review `test_string_utils.cpp` coverage
- [ ] Add edge case tests (null pointers, empty strings)
- [ ] Create placeholder tests for math_extra
- [ ] Document test patterns in `docs/TESTING.md`

**Success criteria**: >90% coverage of string_utils functions

### Day 5: Golden Content Setup

**Tasks**:
- [ ] Copy `background01.bsp` to `tests/golden_maps/`
- [ ] Launch map headless, verify no crashes
- [ ] Take reference screenshots (vanilla SDK)
- [ ] Document camera positions in `GOLDEN_CONTENT.md`

**Success criteria**: Can launch golden map from tests/

### Day 6: Parity Test Harness

**Tasks**:
- [ ] Write `tests/parity/run_visual_tests.py`
- [ ] Implement screenshot capture via console commands
- [ ] Implement image diff (PIL/OpenCV)
- [ ] Integrate with CTest

**Success criteria**: Can run parity test, get diff report

### Day 7: Documentation and Planning

**Tasks**:
- [ ] Update README.md with build instructions
- [ ] Write `docs/ARCHITECTURE.md` (module boundaries)
- [ ] Review Week 2 plan (bicubic + ACES)
- [ ] Create GitHub Project board for Source 1.5

**Success criteria**: Docs are clear, Week 2 ready to start

---

## File Tree (After Week 1)

```
source-sdk-2013/
├── .github/
│   └── workflows/
│       ├── build-windows.yml          # CI for Windows x64
│       └── build-linux.yml            # TODO: Add Linux
├── .clang-format                      # Code formatting rules
├── CMakeLists.txt                     # Root CMake config
├── README.md                          # Updated with build instructions
├── docs/
│   ├── CODING_STYLE.md                # Style guide
│   ├── GOLDEN_CONTENT.md              # Parity test plan
│   ├── PHASE_0_WEEK_1.md              # This file
│   └── ARCHITECTURE.md                # TODO: Module boundaries
├── src/
│   ├── framework/                     # New: Engine-agnostic utils
│   │   ├── CMakeLists.txt
│   │   ├── string_utils.h/.cpp
│   │   ├── math_extra.h/.cpp
│   │   └── serialization.h/.cpp
│   ├── engine_bridge/                 # New: CreateInterface adapters
│   │   ├── CMakeLists.txt
│   │   ├── filesystem_bridge.h/.cpp
│   │   ├── materialsystem_bridge.h/.cpp
│   │   └── console_bridge.h/.cpp
│   ├── tools/                         # New: Build utilities
│   │   └── CMakeLists.txt
│   ├── game/                          # Existing SDK code (unchanged)
│   ├── materialsystem/                # Existing (will add PBR shader Week 3)
│   ├── tier0/                         # Existing (precompiled .lib)
│   ├── tier1/                         # Existing (precompiled .lib)
│   └── ... (rest of SDK)
├── tests/
│   ├── CMakeLists.txt                 # Test suite config
│   ├── test_string_utils.cpp          # Framework unit tests
│   ├── golden_maps/                   # TODO: Copy BSP files
│   │   ├── background01.bsp
│   │   └── d1_canals_01.bsp
│   ├── golden_images/                 # TODO: Reference screenshots
│   │   ├── vanilla/
│   │   └── source15/
│   └── parity/
│       └── run_visual_tests.py        # TODO: Image diff script
└── content/                           # TODO: New test content
    ├── maps/benchmarks/
    │   ├── benchmark_pbr.vmf
    │   ├── benchmark_audio.vmf
    │   └── benchmark_combined.vmf
    ├── materials/pbr_test/
    └── sound/test/
```

---

## Success Metrics

At end of Week 1, we can answer "YES" to:

- [ ] Can we build Source SDK via GitHub Actions?
- [ ] Can we build new modules via CMake?
- [ ] Can we run unit tests automatically?
- [ ] Do we have coding standards enforced?
- [ ] Can we launch a golden map for testing?
- [ ] Is the architecture documented and clear?

---

## Risks and Mitigations

### Risk: VPC build fails in CI

**Mitigation**: Test locally first, match CI environment exactly (VS 2022, Python 3.13)

### Risk: CMake can't find tier0/tier1 libs

**Mitigation**: Use precompiled libs from `src/lib/public/x64`, don't try to rebuild tiers yet

### Risk: Golden maps don't load headless

**Mitigation**: Use `-novid -nosound -nojoy -noipx -sw` flags, simplest map (background01) first

### Risk: Tests depend on engine running

**Mitigation**: Start with pure logic tests (string_utils, math), defer engine-dependent tests to Week 2+

---

## Next Week Preview (Week 2: Bicubic + ACES)

**Goals**:
- Port HL2 20th Anniversary bicubic lightmap code
- Implement ACES tonemapping (post-process)
- Create before/after screenshot comparisons
- Start Hub shell (ImGui window, project chooser)

**Deliverables**:
- Bicubic lightmaps working with `r_lightmap_bicubic 1`
- ACES tonemap with `mat_tonemapping_aces 1`
- Docs with visual proof of improvements
- Hub.exe skeleton running

**Preparation needed**:
- Locate bicubic shader code from HL2 update
- Research ACES implementation (Narkowicz formula)
- Set up ImGui via CMake/vcpkg

---

## References

- Catch2 docs: https://github.com/catchorg/Catch2
- CMake best practices: https://cliutils.gitlab.io/modern-cmake/
- Valve Developer Wiki: https://developer.valvesoftware.com/
- TDD in game engines: https://www.gamedev.net/articles/programming/general-and-gameplay-programming/test-driven-design-in-game-development-r3714/

---

**This document is a living guide. Update as tasks complete or plans change.**

Last updated: 2025-11-03
