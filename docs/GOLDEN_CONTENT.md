# Golden Content for Parity Testing

This document defines the "golden" maps and assets used to ensure Source 1.5 modifications don't break existing content or introduce regressions.

## Purpose

**Parity testing** ensures that:
1. Visual output matches expectations (rendering, lighting, materials)
2. Performance meets targets (frame time budgets)
3. Compatibility with existing SDK content is maintained
4. New features (PBR, bicubic, Steam Audio) can be A/B tested

## Golden Maps (from Steam installation)

### Located on your system

**Half-Life 2**: `C:\Program Files (x86)\Steam\steamapps\common\Half-Life 2\hl2\maps\`

**Team Fortress 2**: `C:\Program Files (x86)\Steam\steamapps\common\Team Fortress 2\tf\maps\`

### Selected Test Maps

#### 1. `background01.bsp` (HL2)
- **Purpose**: Minimal scene for smoke tests
- **Features**: Simple geometry, basic lighting
- **Load time**: <5 seconds
- **Use for**: Quick CI validation, shader compilation checks
- **Target FPS**: 300+ (trivial scene)

#### 2. `d1_canals_01.bsp` (HL2)
- **Purpose**: Outdoor environment with water, props, NPCs
- **Features**:
  - Water shader (reflects, refracts)
  - Static props (models with lightmaps)
  - Dynamic lights (flashlight)
  - Brush-based geometry
- **Use for**: Material system parity, lighting validation
- **Target FPS**: 144 @ 1080p (RTX 3060)

#### 3. `cp_badlands.bsp` (TF2)
- **Purpose**: Complex multiplayer map with varied lighting
- **Features**:
  - Indoor/outdoor transitions
  - Multiple material types
  - Env_cubemaps
  - Particles, physics props
- **Use for**: Full feature stress test, perf regression tracking
- **Target FPS**: 120 @ 1080p (RTX 3060)

## Test Methodology

### Visual Parity Tests

**Approach**: Capture reference screenshots from vanilla SDK, compare against Source 1.5 output

**Tools**:
- `screenshot` console command
- Image diff (SSIM/RMSE via Python/OpenCV)
- Acceptable threshold: <2% RMSE for exact parity, <5% for bicubic/tonemap differences

**Process**:
1. Launch map in vanilla SDK with fixed camera position
2. Take screenshot (save to `tests/golden_images/vanilla/`)
3. Launch same map in Source 1.5 at same position
4. Take screenshot (save to `tests/golden_images/source15/`)
5. Run image diff script
6. Pass if RMSE < threshold

**CI Integration**:
- Store golden screenshots in Git LFS
- Automated diff on every PR
- Fail build if new regressions introduced

### Performance Parity Tests

**Metrics**:
- Average frametime (ms)
- 99th percentile frametime (ms)
- CPU time (ms)
- GPU time (ms)

**Tools**:
- `timerefresh` console command (built-in benchmark)
- Tracy profiler for detailed breakdowns
- Custom frame capture tool (records 1000 frames)

**Budgets** (at 1080p, High settings):

| Map | Hardware | Avg FPS | 99p Frametime | Notes |
|-----|----------|---------|---------------|-------|
| background01 | RTX 3060 | 300+ | <5ms | Trivial scene |
| d1_canals_01 | RTX 3060 | 144 | <8ms | Outdoor |
| cp_badlands | RTX 3060 | 120 | <10ms | Complex MP |
| d1_canals_01 | GTX 1660 | 60 | <16ms | Minimum spec |

**Regression Policy**:
- <5% slowdown: Acceptable (within noise)
- 5-10% slowdown: Requires justification and profiling
- >10% slowdown: PR blocked until fixed

## New Test Content (Source 1.5 specific)

For features not in vanilla SDK (PBR, Steam Audio), create minimal test scenes:

### `benchmark_pbr.vmf`
- **Location**: `content/maps/benchmarks/`
- **Purpose**: PBR shader validation
- **Contents**:
  - 512x512 room
  - 5 prop_static with PBR materials (metal, wood, concrete, plastic, fabric)
  - 4 lights (point, spot, projected, env_cubemap)
  - 1 window (translucent test)
- **Compile**: VBSP + VRAD (bicubic ON/OFF variants)
- **Tests**: Material appearance, lightmap quality, reflection probes

### `benchmark_audio.vmf`
- **Location**: `content/maps/benchmarks/`
- **Purpose**: Steam Audio occlusion/HRTF validation
- **Contents**:
  - L-shaped corridor (128x512 units, 90° turn)
  - func_door at corner
  - 3 ambient_generic sources (outdoor, indoor, weapon fire)
  - Clear line-of-sight vs occluded positions
- **Tests**: Toggle occlusion ON/OFF, measure audio difference (FFT/level meters)

### `benchmark_combined.vmf`
- **Location**: `content/maps/benchmarks/`
- **Purpose**: Full stress test (PBR + Steam Audio + all features)
- **Contents**: Combination of above
- **Target**: 144 FPS @ 1080p with all features enabled

## Asset Library

### PBR Test Materials (to be created)

**Location**: `content/materials/pbr_test/`

1. `metal_brushed` - High metallic (1.0), medium roughness (0.4)
2. `concrete_rough` - No metallic (0.0), high roughness (0.8)
3. `wood_oak` - No metallic (0.0), medium roughness (0.6)
4. `plastic_glossy` - No metallic (0.0), low roughness (0.2)
5. `fabric_canvas` - No metallic (0.0), high roughness (0.9)

**Format**:
- Albedo: `_d.vtf` (sRGB)
- Normal: `_n.vtf` (linear, DX convention)
- MRAO: `_mrao.vtf` (linear, R=metallic, G=roughness, B=AO)

**Source**: CC0 from ambientCG.com or hand-authored in Substance Painter

### Audio Test Assets (CC0)

**Location**: `content/sound/test/`

1. `door_slam.wav` - Percussive, occlusion test
2. `speech_sample.wav` - "The quick brown fox..." HRTF clarity test
3. `ambient_room.wav` - Loopable tone, reflection test
4. `gunshot.wav` - Distance falloff test
5. `footsteps.wav` - Moving source test

**Source**: Freesound.org (CC0 license)

## CI Pipeline Integration

### GitHub Actions Workflow (added to `.github/workflows/build-windows.yml`)

```yaml
- name: Download golden content
  run: |
    # TODO: Download golden screenshots from Git LFS
    git lfs pull

- name: Run parity tests
  run: |
    # TODO: Launch maps headless, capture frames, diff images
    python tests/parity/run_visual_tests.py

- name: Upload diff images (on failure)
  if: failure()
  uses: actions/upload-artifact@v4
  with:
    name: parity-diff-images
    path: tests/golden_images/diffs/
```

## Manual Test Checklist

Before each release, manually verify:

- [ ] All golden maps load without errors
- [ ] Performance meets targets (run `timerefresh 1000`)
- [ ] PBR materials render correctly
- [ ] Bicubic lightmaps show visual improvement
- [ ] Steam Audio occlusion audible
- [ ] Hot-reload works for materials
- [ ] No shader compile errors in console

## Future Additions

- DX11 parity: Pixel-perfect comparison with DX9 output
- Linux parity: Same maps via Steam Runtime build
- VR parity: If VR support added
- BSP v25 validation: When upgraded

## References

- HL2 maps: [Valve Developer Wiki - Half-Life 2 Maps](https://developer.valvesoftware.com/wiki/Half-Life_2_Maps)
- TF2 maps: [TF2 Wiki - List of maps](https://wiki.teamfortress.com/wiki/List_of_maps)
- Parity testing best practices: https://www.alanzucconi.com/2015/07/01/automated-testing-in-unity/
