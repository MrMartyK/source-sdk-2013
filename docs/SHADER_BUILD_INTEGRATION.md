# Shader Build Integration

Integration of PBR and SSAO shaders into Source SDK 2013 shader build system.

---

## Overview

Added Source 1.5 shaders to the SDK shader compilation pipeline. This enables automatic compilation of PBR and SSAO shaders during the build process.

**Status:** Phase 3 (PBR Materials) - Build Integration Complete

---

## Shaders Added

### PBR (Physically-Based Rendering)

**Files:**
- `pbr_vs20.fxc` - Vertex shader (70 lines)
- `pbr_ps20b.fxc` - Pixel shader (160 lines)
- `pbr_dx9.cpp` - Material system integration (180 lines)

**Compiled Outputs:**
- `pbr_vs20.vcs` - Vertex shader bytecode
- `pbr_ps20.vcs` - Pixel shader SM 2.0
- `pbr_ps20b.vcs` - Pixel shader SM 2.0b

### SSAO (Screen Space Ambient Occlusion)

**Files:**
- `ssao_ps20b.fxc` - SSAO compute shader (115 lines)
- `ssao_blur_ps20b.fxc` - Bilateral blur shader (80 lines)
- `ssao_dx9.cpp` - Material system integration (135 lines)
- `ssao_blur_dx9.cpp` - Blur material integration (105 lines)

**Compiled Outputs:**
- `ssao_ps20.vcs` - SSAO compute SM 2.0
- `ssao_ps20b.vcs` - SSAO compute SM 2.0b
- `ssao_blur_ps20.vcs` - Blur SM 2.0
- `ssao_blur_ps20b.vcs` - Blur SM 2.0b

---

## Build System Architecture

### Shader Compilation Pipeline

```
1. buildshaders.bat [projectname]
   ↓
2. process_shaders.ps1 -Version 20b 'projectname.txt'
   ↓
3. Reads shader list from projectname.txt
   ↓
4. For each shader:
   - Compile .fxc → .vcs (bytecode)
   - Generate .inc files (C++ headers)
   ↓
5. Copy compiled shaders to game/shaders/fxc/
```

### Shader List Files

**Location:** `src/materialsystem/stdshaders/`

**Files:**
- `stdshader_dx9_20b.txt` - Standard Valve shaders
- `sdkshaders_dx9_20b.txt` - SDK/custom shaders (Source 1.5 shaders here)
- `stdshader_dx9_30.txt` - Shader Model 3.0 shaders

**Format:**
```
// Comments start with //
shader_vs20.fxc
shader_ps20b.fxc
shader_ps2x.fxc  // Compiles to both ps20 and ps20b
```

### Shader Naming Conventions

| Extension | Meaning | Outputs |
|-----------|---------|---------|
| `_vs20.fxc` | Vertex Shader SM 2.0 | `_vs20.vcs` |
| `_ps20.fxc` | Pixel Shader SM 2.0 | `_ps20.vcs` |
| `_ps20b.fxc` | Pixel Shader SM 2.0b | `_ps20b.vcs` |
| `_ps2x.fxc` | Pixel Shader both | `_ps20.vcs`, `_ps20b.vcs` |

**Source 1.5 uses:** `_ps20b` for pixel shaders (SM 2.0b features required)

---

## Building Shaders

### Windows

**Prerequisites:**
- Visual Studio 2022 with C++ tools
- DirectX SDK (legacy, for fxc.exe)
- PowerShell execution policy set

**Build Command:**
```bat
cd src\materialsystem\stdshaders
buildshaders sdkshaders_dx9_20b
```

**Build with Mod:**
```bat
buildshaders sdkshaders_dx9_20b -game "C:\path\to\mod" -source "C:\path\to\src"
```

**Output Location:**
- Default: `src/materialsystem/stdshaders/shaders/fxc/`
- With `-game`: `[game]/shaders/fxc/`

### Linux

Shader compilation typically done on Windows, but Linux can use pre-compiled `.vcs` files.

**Copy compiled shaders to Linux:**
```bash
cp -r game/shaders/ ~/Steam/steamapps/sourcemods/mymod/shaders/
```

---

## Shader Include Files

Compiled shaders generate C++ header files for use in material system:

**Generated Files (in `include/` directory):**
- `pbr_vs20.inc` - PBR vertex shader
- `pbr_ps20b.inc` - PBR pixel shader
- `ssao_ps20b.inc` - SSAO compute shader
- `ssao_blur_ps20b.inc` - SSAO blur shader

**Usage in C++:**
```cpp
#include "pbr_vs20.inc"
#include "pbr_ps20b.inc"

// In shader code:
DECLARE_STATIC_VERTEX_SHADER( pbr_vs20 );
SET_STATIC_VERTEX_SHADER( pbr_vs20 );

DECLARE_STATIC_PIXEL_SHADER( pbr_ps20b );
SET_STATIC_PIXEL_SHADER( pbr_ps20b );
```

---

## Modified Files

### sdkshaders_dx9_20b.txt

**Added (lines 18-24):**
```
// Source 1.5 - PBR (Physically-Based Rendering)
pbr_vs20.fxc
pbr_ps20b.fxc

// Source 1.5 - SSAO (Screen Space Ambient Occlusion)
ssao_ps20b.fxc
ssao_blur_ps20b.fxc
```

**Rationale:**
- Grouped Source 1.5 shaders separately
- Clear comments for maintainability
- Added to SDK shader list (not standard shaders)

---

## Verification

### Check Compiled Shaders

After building, verify output files exist:

**Windows:**
```bat
dir src\materialsystem\stdshaders\shaders\fxc\pbr_*.vcs
dir src\materialsystem\stdshaders\shaders\fxc\ssao_*.vcs
```

**Expected Files:**
```
pbr_vs20.vcs
pbr_ps20.vcs
pbr_ps20b.vcs
ssao_ps20.vcs
ssao_ps20b.vcs
ssao_blur_ps20.vcs
ssao_blur_ps20b.vcs
```

### Check Include Files

**Windows:**
```bat
dir src\materialsystem\stdshaders\include\pbr_*.inc
dir src\materialsystem\stdshaders\include\ssao_*.inc
```

**Expected Files:**
```
pbr_vs20.inc
pbr_ps20b.inc
ssao_ps20b.inc
ssao_blur_ps20b.inc
```

### Verify in-game

1. Build game DLLs
2. Copy shaders to game directory
3. Launch game
4. Test materials using new shaders

**Test PBR:**
```
// materials/test_pbr.vmt
"PBR"
{
    "$basetexture" "test/albedo"
    "$bumpmap" "test/normal"
    "$mraotexture" "test/mrao"
}
```

**Test SSAO:**
```
mat_ssao 1
mat_ssao_radius 0.5
mat_ssao_samples 16
```

---

## Troubleshooting

### Error: "ShaderCompile2.exe not found"

**Cause:** Missing DirectX shader compiler

**Fix:**
1. Install DirectX SDK (June 2010)
2. Ensure `src/devtools/bin/ShaderCompile2.exe` exists
3. If missing, copy from SDK installation

### Error: "fxc.exe not found"

**Cause:** DirectX SDK not in PATH

**Fix:**
```bat
set PATH=%PATH%;C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Utilities\bin\x64
```

### Error: "Cannot open include file"

**Cause:** Missing common shader headers

**Fix:**
- Ensure `common_ps_fxc.h` and `common_vs_fxc.h` exist in shader directory
- Verify `#include` paths in shader files

### Shaders not loading in-game

**Cause:** Shaders not copied to game directory

**Fix:**
```bat
xcopy /s /y src\materialsystem\stdshaders\shaders\fxc\*.vcs game\mod_tf\shaders\fxc\
```

### Shader compilation errors

**Check log files:**
```
src\materialsystem\stdshaders\shaders\fxc\*.log
```

**Common issues:**
- Syntax errors in HLSL
- Missing shader model features
- Undefined macros or constants

---

## Shader Model Support

### Shader Model 2.0b Features Used

**PBR Shader:**
- Dynamic branching (`[loop]`)
- Texture LOD bias
- 16 texture samplers
- Complex arithmetic (BRDF calculations)

**SSAO Shader:**
- Dynamic loop (configurable samples)
- Multiple texture samples
- Complex depth reconstruction

**Requirement:** Graphics cards from 2004+ (GeForce 6, Radeon X800)

---

## Performance Considerations

### Shader Compilation Time

**Per-shader compilation:** ~2-5 seconds
**Full SDK shader build:** ~5-10 minutes

**Optimization:** Use `-Dynamic` flag for incremental builds (only recompile changed shaders)

### Runtime Performance

**Shader caching:** Compiled `.vcs` files are cached by engine
**First load:** Slight delay loading new shaders (~100-200ms)
**Subsequent loads:** Instant (loaded from cache)

---

## Future Work

### Shader Model 3.0 Support

Add shaders to `sdkshaders_dx9_30.txt` for SM 3.0 features:
- More texture samplers
- Longer instruction counts
- Better branching performance

**Files to create:**
- `pbr_ps30.fxc` (SM 3.0 version)
- `ssao_ps30.fxc` (SM 3.0 version)

### DirectX 11 Shaders

When DX11 backend is implemented (Phase 5):
- Create `pbr_ps50.hlsl` (SM 5.0)
- Create `ssao_cs50.hlsl` (Compute shader)
- Add to `stdshader_dx11.txt`

---

## Integration Checklist

- [x] Add shaders to `sdkshaders_dx9_20b.txt`
- [x] Verify shader files exist (.fxc)
- [x] Verify material integrations exist (.cpp)
- [ ] Build shaders using `buildshaders.bat`
- [ ] Verify compiled bytecode (.vcs files)
- [ ] Verify include files generated (.inc files)
- [ ] Copy shaders to game directory
- [ ] Test in-game
- [ ] Document build process

---

## References

- **VDC Shader Compilation:** https://developer.valvesoftware.com/wiki/Shader_Authoring
- **HLSL Reference:** Microsoft DirectX SDK Documentation
- **Source Shader System:** `src/materialsystem/stdshaders/README.txt`

---

*Last Updated: 2025-11-04*
*Source 1.5 - Phase 3 Shader Build Integration*
