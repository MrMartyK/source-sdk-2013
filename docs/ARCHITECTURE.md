# Source 1.5 Architecture

**Last updated**: 2025-11-03
**Version**: 0.1.0-alpha

---

## Vision

**Source 1.5** transforms the Source SDK 2013 from a developer SDK into a modern, usable engine + toolchain with:

- **Godot-like iteration**: Drag-drop assets, hot-reload in <10s
- **Modern visuals**: PBR materials, bicubic lightmaps, ACES tonemap
- **Spatial audio**: Steam Audio HRTF, occlusion, reflections
- **Apple-grade tools**: Clean UI, low friction, zero visual noise
- **TDD foundation**: Every module tested, parity validated, CI enforced

**Non-goals**:
- Replace Source Engine completely (we extend, don't rewrite)
- Break existing mods (compatibility is non-negotiable)
- Compete with Source 2 (we elevate Source 1 to "1.5")

---

## System Overview

```
┌─────────────────────────────────────────────────────────────┐
│                        Source 1.5                           │
│                                                             │
│  ┌─────────────┐         ┌───────────────────────────┐    │
│  │ Editor Hub  │         │  Source Engine Runtime    │    │
│  │  (ImGui)    │◄───IPC──┤  (Valve binaries + mods)  │    │
│  │  MIT License│         │  SDK2013 License          │    │
│  └─────────────┘         └───────────────────────────┘    │
│        │                           │                       │
│        │  Compiles                 │  Loads                │
│        ▼                           ▼                       │
│  ┌──────────────────────────────────────────────────┐     │
│  │            Asset Pipeline                        │     │
│  │  FBX→MDL  PNG→VTF  VMT→MAT  WAV→soundscript    │     │
│  └──────────────────────────────────────────────────┘     │
│                                                             │
│  ┌───────────────────────────────────────────────────┐    │
│  │       New Modules (Source 1.5)                    │    │
│  │  • framework/ (engine-agnostic utils)             │    │
│  │  • engine_bridge/ (CreateInterface adapters)      │    │
│  │  • shaderapidx11/ (DX11 renderer, optional)       │    │
│  │  • steamaudio/ (spatial audio, optional)          │    │
│  │  • stdshaders/pbr_dx9.cpp (PBR shader)            │    │
│  └───────────────────────────────────────────────────┘    │
│                                                             │
│  ┌───────────────────────────────────────────────────┐    │
│  │       Existing SDK (Preserved)                    │    │
│  │  • tier0/tier1/mathlib (foundation)               │    │
│  │  • materialsystem/ (DX9 default)                  │    │
│  │  • engine/ (closed binary)                        │    │
│  │  • game/client, game/server (split codebase)      │    │
│  └───────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────┘
```

---

## Module Boundaries

### Tier System (Valve's Foundation)

**Preserved unchanged**, use as-is:

- **tier0**: Platform abstractions (memory, threading, debug)
- **tier1**: Core utilities (containers, KeyValues, strings, convars)
- **mathlib**: Vector/matrix math
- **vstdlib**: Standard library replacements

**Dependency rule**: Higher tiers can depend on lower, never reverse.

**Why keep**: Battle-tested, stable, all SDK code depends on these.

### Framework (New in Source 1.5)

**Location**: `src/framework/`

**Purpose**: Engine-agnostic utilities for tools and game code

**Modules**:
- `string_utils` - Safe string ops, path manipulation
- `math_extra` - Helpers beyond mathlib
- `serialization` - Simple binary/text formats

**Dependencies**: tier0, tier1, mathlib ONLY (no engine, no game)

**Why**: Tools (Hub) need utilities without pulling in 50MB of engine binaries.

**Rules**:
- Must have unit tests
- No game-specific code
- Namespace: `S15::`

### Engine Bridge (New in Source 1.5)

**Location**: `src/engine_bridge/`

**Purpose**: Adapters for CreateInterface pattern, thin wrappers for tools/tests

**Modules**:
- `filesystem_bridge` - Wraps IFileSystem
- `materialsystem_bridge` - Wraps IMaterialSystem
- `console_bridge` - Wraps cvar/console

**Dependencies**: tier0, tier1, framework, engine public headers

**Why**: Decouple tools from engine implementation, enable mocking for tests.

**Pattern**:
```cpp
class CFileSystemBridge {
public:
    bool Init(); // Calls CreateInterface("VFileSystem017")
    bool FileExists(const char *path);
private:
    IFileSystem *m_pFS; // Got via CreateInterface
};
```

### Material System (Extended)

**Location**: `src/materialsystem/`

**Changes in Source 1.5**:
- **Add**: `stdshaders/pbr_dx9.cpp` - New PBR shader
- **Modify**: `stdshaders/lightmappedgeneric_ps20b.fxc` - Add bicubic filtering
- **Add**: `shaderapidx11/` - Parallel DX11 implementation (optional, flag-gated)

**Preserve**:
- DX9 ShaderAPI (default renderer)
- All existing shaders (Phong, LightmappedGeneric, etc.)
- VMT format (extend with new keys like `$MRAOTexture`)

**Interface boundary**: `IMaterialSystem` (CreateInterface)

**Testing**: Parity scenes ensure DX9 output unchanged, DX11 pixel-perfect when enabled.

### Steam Audio (New, Optional)

**Location**: `src/soundemittersystem/steamaudio/`

**Integration**: Wet bus (send/return path), not mixer replacement

**Dependencies**: Steam Audio C API (Apache 2.0), tier0/tier1

**Cvars**:
- `s1_steamaudio 0/1` - Master enable
- `s1_sa_occlusion 0/1` - Ray-traced occlusion
- `s1_sa_hrtf 0/1` - Binaural spatialization
- `s1_sa_reflections 0/1` - Early reflections

**CMake flag**: `SOURCE15_STEAMAUDIO=ON/OFF` (default OFF)

**Why optional**: Not all mods need spatial audio, keeps base build lean.

### Game Code (Refactored)

**Location**: `src/game/`

**Current problem**: Mixed HL2/TF2/HL2MP code via `#ifdef` soup in `shared/`

**Source 1.5 strategy**:
1. **Short term (Weeks 1-4)**: Leave as-is, don't break anything
2. **Medium term (Weeks 5-8)**: Extract mini-libs from `shared/`
   - `shared/animation/` → `lib_animation`
   - `shared/network/` → `lib_network`
   - `shared/prediction/` → `lib_prediction`
3. **Long term (Post-1.0)**: Game templates use only framework + mini-libs

**Dependency rule**: `game/` can depend on framework, engine_bridge, tier*, but framework CANNOT depend on `game/`

---

## Editor Hub Architecture

### Design Philosophy: "Apple-Grade" UX

**Pillars**:
1. **Calm, bright, purposeful** - Generous spacing, zero noise
2. **Frictionless affordances** - Obvious next actions, drag-drop everywhere
3. **Latency as UX** - Show progress, never block

### Color System

```
Background:   #F8FAFC (near-white)
Separator:    #E8EDF2 (subtle gray)
Accent:       #0077FF (saturated blue, primary actions only)
Text:         #101418 (body), #606B78 (secondary)
```

### Typography

**Font stack**: Inter (Windows/Linux), SF Pro (if macOS)

**Scale**:
- Display: 28pt (headers)
- Title: 20pt (pane titles)
- Subhead: 16pt (section headers)
- Body: 14pt (default text)
- Caption: 12pt (metadata)

### Layout

```
┌────────────────────────────────────────────────────────────┐
│ [Project ▼]  [▶ Play] [Dev ▼]         [🔍 Search...]      │ Header
├────────────┬────────────────────────────────┬──────────────┤
│  Assets    │      Viewport / Preview        │  Inspector   │
│            │                                │              │
│ 🗀 Models   │   [Material preview sphere]    │ Name:        │
│ 🗀 Materials│                                │ metal_01     │
│ 🗀 Audio    │                                │              │
│ 🗀 Scripts  │                                │ Metallic: ●─ │
│ 🗀 Maps     │                                │ Roughness:●─ │
│            │                                │              │
│            │                                │              │
├────────────┴────────────────────────────────┴──────────────┤
│ Console & Tasks                                            │
│ [✓] Compiled metal_01.vmt (142ms)                         │
│ [→] Hot-reloading materials...                            │
└────────────────────────────────────────────────────────────┘
```

### Critical Flows

**Material iteration loop** (target: <10s):
1. User edits PNG in Photoshop, saves
2. Hub detects file change (file watcher)
3. Auto-runs `vtex` (shows progress pill in header)
4. Writes VMT (or updates existing)
5. Sends IPC: `mat_reloadmaterial metal_01`
6. Game updates in-engine
7. Console shows: "[✓] Reloaded metal_01 (87ms)"

**Audio toggle flow** (instant):
1. User clicks checkbox: "Occlusion ON"
2. Hub sends IPC: `s1_sa_occlusion 1`
3. Game updates immediately (no reload)
4. Listener gizmo shows raycast debug lines

### Tech Stack

**UI**: Dear ImGui 1.92.0 (immediate mode, MIT license)

**File watching**: `std::filesystem` + platform APIs (Windows: ReadDirectoryChangesW, Linux: inotify)

**IPC**: Named pipes (Windows) / Unix domain sockets (Linux)

**Asset preview**:
- Models: Assimp → OpenGL/DX11 viewer
- Materials: PBR sphere renderer (same shader as game)
- Audio: Waveform + playback via OpenAL/XAudio2

**Build**: CMake, separate repo (`source-hub`)

**License**: MIT (separate from SDK2013 non-commercial)

---

## Data Flow

### Asset Import (FBX → MDL)

```
User drags FBX
    ↓
Hub detects drop
    ↓
Parse FBX metadata (scale, materials)
    ↓
Generate QC file (auto-rules or template)
    ↓
Run studiomdl.exe (show progress)
    ↓
Output: model.mdl + .vvd + .vtx
    ↓
Copy to game/mod/models/
    ↓
Update asset browser
    ↓
Preview in viewport
```

### Material Hot-Reload

```
User saves metal_01_d.png
    ↓
File watcher fires
    ↓
Hub: vtex metal_01_d.png → metal_01_d.vtf
    ↓
Update metal_01.vmt ($baseTexture path)
    ↓
IPC → Game: mat_reloadmaterial metal_01
    ↓
Game: Flush cached material, reload VMT, rebind textures
    ↓
Scene updates (next frame)
```

### Audio Occlusion Toggle

```
User clicks "Occlusion ON" in Hub
    ↓
Hub: s1_sa_occlusion 1 via IPC
    ↓
Game: g_SA_Occlusion.SetValue(1)
    ↓
Steam Audio: Enable raycast occlusion
    ↓
Audio mixer: Apply wet bus with occlusion factor
    ↓
User hears muffled sound behind walls
```

---

## Build System

### Dual System Strategy

**VPC (Valve Project Creator)**: Primary for SDK builds (compatibility)

**CMake**: New modules, tests, tools

**Why both**:
- VPC generates .sln for game DLLs (familiar workflow)
- CMake builds framework, engine_bridge, tests (modern tooling)
- Gradual migration, no big-bang rewrite

### CMake Targets

```bash
# Core libraries
lib_framework       # String utils, math, serialization
lib_engine_bridge   # CreateInterface adapters

# Renderers
shaderapidx9        # Existing DX9 (via VPC for now)
shaderapidx11       # New DX11 (CMake, flag-gated)

# Shaders
stdshaders          # Includes new PBR shader

# Optional features
lib_steamaudio      # Steam Audio integration (flag-gated)

# Tests
source15_tests      # Unit tests (Catch2)
parity_tests        # Visual/perf regression tests

# Tools
source15_hub        # Editor Hub (separate repo)
```

### Build Flags

```cmake
SOURCE15_BUILD_TESTS=ON      # Unit tests (default: ON)
SOURCE15_STEAMAUDIO=OFF      # Spatial audio (default: OFF)
SOURCE15_BUILD_DX11=OFF      # DX11 renderer (default: OFF, alpha only)
SOURCE15_IWYU=OFF            # Include analysis (default: OFF)
```

---

## Testing Strategy

### Unit Tests (Catch2)

**Coverage**: Framework, engine_bridge, new modules

**Example**:
```cpp
TEST_CASE("StringCopy handles truncation", "[framework]") {
    char buf[10];
    int n = S15::StringCopy(buf, "TooLongString", sizeof(buf));
    REQUIRE(n == 9);
    REQUIRE(buf[9] == '\0');
}
```

**Run**: `ctest --output-on-failure`

### Parity Tests (Visual/Performance)

**Purpose**: Ensure changes don't break existing content

**Approach**:
1. Capture golden screenshots from vanilla SDK
2. Launch same scene in Source 1.5
3. Compare images (SSIM/RMSE)
4. Pass if <2% diff (exact parity) or <5% (bicubic allowed)

**Performance**:
- Record 1000 frames with `timerefresh`
- Compare avg/99p frametime
- Fail if >10% regression

**CI Integration**: Run on every PR, block merge if regressions

### Integration Tests (Hub ↔ Game)

**Scenarios**:
- Material hot-reload (<10s end-to-end)
- Audio toggle (instant IPC response)
- Asset import (FBX → in-game in <60s)

**Method**: Automated Selenium-style (pyautogui for Hub, rcon for game)

---

## Dependency Graph

```
┌─────────────────────────────────────────────────────┐
│                 Application Layer                   │
│  (game/client, game/server, source15_hub)          │
└────────────────┬────────────────────────────────────┘
                 │
        ┌────────┴────────┐
        ▼                 ▼
┌──────────────┐  ┌────────────────┐
│ engine_bridge│  │   framework    │
└──────┬───────┘  └────────┬───────┘
       │                   │
       ▼                   ▼
┌─────────────────────────────────────┐
│  tier0, tier1, mathlib, vstdlib     │
│  (Valve's foundation, unchanged)    │
└─────────────────────────────────────┘
```

**Rules**:
- Lower tiers never depend on higher
- `framework` is peer to `engine_bridge`, both depend only on tiers
- Game code can use everything, but nothing depends on game code
- Circular dependencies are forbidden (enforced by CMake)

---

## Release Pipeline

### Version Scheme

**Format**: `MAJOR.MINOR.PATCH-TAG`

**Examples**:
- `0.1.0-alpha` - Week 1 foundation
- `0.2.0-alpha` - Week 3 PBR + bicubic
- `0.5.0-beta` - Week 10 all features, needs polish
- `1.0.0` - Public release (Week 12+)

### CI/CD Flow

```
Push to GitHub
    ↓
GitHub Actions: Build Win x64
    ↓
Run unit tests (Catch2)
    ↓
Run parity tests (golden maps)
    ↓
Static analysis (clang-tidy, cppcheck)
    ↓
[All green?]
    ├─ YES → Upload artifacts (DLLs, PDBs, symbols)
    └─ NO  → Block PR, notify developer
    ↓
[Tagged release?]
    ├─ YES → Build Hub installer (MSI/AppImage)
    │        Create GitHub Release
    │        Upload binaries + docs
    └─ NO  → Done
```

### Artifacts

**Per-commit** (7 day retention):
- `windows-x64-binaries.zip` (client.dll, server.dll, shaders)
- `windows-x64-symbols.zip` (.pdb files)

**Tagged release**:
- `Source15-v0.1.0-Windows-x64.zip` (full SDK)
- `Source15-Hub-v0.1.0-Windows.msi` (Hub installer)
- `Source15-Hub-v0.1.0-Linux.AppImage` (Linux Hub)
- `Source15-Content-v0.1.0.zip` (templates, test maps)

---

## Security Model

### Sandbox Constraints

**Hub safe mode** (default for untrusted projects):
- Read-only access to `<mod>/` directories
- No external process execution
- No network access

**Full mode** (user confirms):
- Can run studiomdl, vtex, vbsp, vrad, vvis
- Whitelisted paths: `<steam>/steamapps/common/Source SDK Base 2013 Multiplayer/bin/`
- Forbidden: System directories, Steam install root

**Rationale**: Prevent malicious VMF/QC from running arbitrary code

### Asset Validation

**Before import**:
- FBX: Size <100MB, poly count <500k (configurable)
- PNG: Dimensions power-of-2 or allow NPOT, size <16MB
- WAV: Sample rate 44.1/48kHz, <10 seconds (ambient) or <60s (music)

**Malformed input handling**:
- Catch parser exceptions (Assimp, libpng, libsndfile)
- Show user-friendly error: "Invalid FBX: corrupted mesh data"
- Never crash Hub

---

## Migration Path (SDK → 1.5)

**Existing mods**: Zero-effort compatibility

**Steps for modder**:
1. Clone `source-sdk-2013` (unchanged)
2. Add `source-1.5` as git submodule or separate clone
3. Copy `game/mymod/` to `source-1.5/game/mymod/`
4. Run VPC as usual: `createallprojects.bat`
5. Optionally enable PBR: Add `$MRAOTexture` to materials
6. Optionally enable Steam Audio: Set `SOURCE15_STEAMAUDIO=ON`

**No breaking changes**:
- VMT format extended, not replaced
- Entity I/O unchanged
- Network protocol unchanged
- BSP format unchanged (v25 is post-1.0)

---

## Performance Budgets

### Rendering (1080p, High settings)

| Feature | GPU Time | Notes |
|---------|----------|-------|
| DX9 baseline | 8ms | Vanilla SDK |
| + Bicubic | +0.5ms | Shader math |
| + PBR | +1ms | MRAO texture reads |
| + ACES tonemap | +0.2ms | Post-process pass |
| **Total (Source 1.5)** | **9.7ms** | 103 FPS |

**Target**: 144 FPS (6.9ms) → Need optimizations or Quality presets

### Audio (Steam Audio)

| Feature | CPU Time | Notes |
|---------|----------|-------|
| Vanilla audio | 1ms | Miles Sound |
| + HRTF (24 sources) | +1.5ms | Convolution |
| + Occlusion (24 sources) | +0.8ms | Raycasts |
| + Early reflections | +0.5ms | Image-source |
| **Total** | **3.8ms** | With budget caps |

**Target**: 2-3ms → Limit to 16 sources, disable distant ambience

---

## Future Roadmap (Post-1.0)

### 1.1 Features (Q1 2026)

- Model hot-reload (entity refresh on .mdl change)
- Visual scripting (nodes → VScript compiler)
- Steam Audio baking (offline reverb)
- DX11 parity complete (production-ready)

### 1.2 Features (Q2 2026)

- Prefab authoring (func_instance wrapper)
- BSP v25 (expanded limits, via Strata reference)
- Linux parity testing (Steam Runtime CI)

### 2.0 Vision (2027+)

- Integrated map editor (basic gizmos in Hub)
- VR support (OpenVR/OpenXR)
- Ray-traced lighting (DXR/VK_KHR_ray_tracing)

---

## Appendix: Key Decisions

### Why not rewrite everything in modern C++20?

**Answer**: Compatibility. Existing mods compile against tier0/tier1 headers. Changing ABI breaks everything. New code uses C++17, old code stays C++03-era.

### Why keep VPC instead of full CMake migration?

**Answer**: Risk. VPC works, generates correct projects. CMake migration is multi-month effort with high breakage risk. Hybrid approach is pragmatic.

### Why not use STL containers?

**Answer**: Source SDK disables exceptions/RTTI, tier1 containers (`CUtlVector`) are optimized for that environment. STL would require enabling exceptions (ABI break).

### Why is Hub a separate repo/license?

**Answer**: Legal clarity. SDK is non-commercial only. Hub is MIT to allow commercial use of tools (e.g., studios building proprietary games can use Hub without license contamination).

### Why not replace Miles Sound with modern audio?

**Answer**: Miles is closed-source but functional. Steam Audio augments (sends), doesn't replace. Full audio system rewrite is multi-year project beyond 1.0 scope.

---

**Document status**: Living architecture guide, update as implementation evolves.
