# PBR Integration Plan - Source 1.5

**Status:** Research Complete, Implementation Planned
**Date:** 2025-11-03 (Day 5)
**Reference:** Thexa4's source-pbr implementation
**Scope:** Phase 3 of Source 1.5 roadmap (weeks 3-5)

---

## Executive Summary

Physically-Based Rendering (PBR) integration will modernize Source 1.5's material system using Thexa4's proven VMT-compatible approach. The MRAO texture format (Metalness-Roughness-AO) enables industry-standard workflows with Substance Painter while maintaining backward compatibility with existing Source materials.

---

## Reference Implementation

**Source:** https://github.com/thexa4/source-pbr
- **Language:** C++ (95.6%), C (3.7%), GLSL (0.3%)
- **License:** Compatible with Source SDK (verify before integration)
- **Documentation:** https://wiki.empiresmod.com/PBR
- **Status:** Production-ready, actively maintained (182 commits)

---

## Technical Overview

### MRAO Texture Format

Single texture packing three PBR properties for memory efficiency:

- **Red Channel:** Metalness (0.0 = dielectric, 1.0 = metallic)
- **Green Channel:** Roughness (0.0 = smooth, 1.0 = rough)
- **Blue Channel:** Ambient Occlusion (contact shadows)

**Advantages:**
- Industry-standard (Unreal Engine, gametextures.com)
- Single texture read vs. three separate textures
- Direct export from Substance Painter

### VMT Parameters

```
"PBR"
{
    $basetexture "materials/mymat/albedo"
    $bumpmap "materials/mymat/normal"
    $mraotexture "materials/mymat/mrao"
    $envmap "env_cubemap"

    $model 1  // 1 = prop, 0 = brush
    $surfaceprop "metal"

    // Optional
    $emissiontexture "materials/mymat/emission"
    $speculartexture "materials/mymat/f0"  // Overrides metalness

    // Parallax mapping
    $parallax 1
    $parallaxdepth 0.03
    $parallaxcenter 0.5

    // Transform
    $basetexturetransform "center .5 .5 scale 2 2 rotate 0 translate 0 0"

    %keywords "empires"
}
```

### Shader Architecture

**Pixel Shader (pbr_ps20b.fxc):**
```hlsl
// Input structure
struct PS_INPUT
{
    float2 baseTexCoord : TEXCOORD0;
    float2 lightmapTexCoord : TEXCOORD1;
    float3 worldNormal : TEXCOORD2;
    float3 worldTangent : TEXCOORD3;
    float3 worldPos : TEXCOORD4;
    float4 lightAtten : TEXCOORD5;
    float4 projPos : TEXCOORD6;
};

// Sample MRAO
float3 mrao = tex2D(MRAOTextureSampler, texCoord).xyz;
float metalness = mrao.r;
float roughness = mrao.g;
float ao = mrao.b;

// PBR Lighting Model (Split-sum approximation)
// Ambient = diffuse IBL + specular IBL
// Direct = sum of dynamic lights with microfacet BRDF
```

**Key Functions:**
- `calculateLight()`: Microfacet BRDF per light
- `ambientLookup()`: Samples ambient cube + lightmap
- `fresnelSchlickRoughness()`: Fresnel with roughness
- `EnvBRDFApprox()`: Precomputed specular BRDF lookup

**Vertex Shader (pbr_vs20b.fxc):**
- Standard world-space transforms
- Tangent-space basis calculation
- Light attenuation prep

---

## Implementation Tasks

### Phase 3A: Shader Implementation (Week 3)

#### Task 1: Shader Files
- [ ] Create `pbr_ps20b.fxc` (pixel shader, SM 2.0b)
- [ ] Create `pbr_ps30.fxc` (pixel shader, SM 3.0)
- [ ] Create `pbr_vs20b.fxc` (vertex shader, SM 2.0b)
- [ ] Create `pbr_vs30.fxc` (vertex shader, SM 3.0)
- [ ] Create `pbr_helper.h` (shared BRDF functions)

**BRDF Functions to Implement:**
```hlsl
// Fresnel-Schlick with roughness
float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness);

// GGX normal distribution
float DistributionGGX(float3 N, float3 H, float roughness);

// Smith geometry shadowing
float GeometrySmith(float3 N, float3 V, float3 L, float roughness);

// Cook-Torrance specular BRDF
float3 calculateLight(float3 N, float3 V, float3 L, float3 albedo,
                      float metalness, float roughness, float3 F0);

// Environment BRDF approximation (Lazarov 2013)
float3 EnvBRDFApprox(float3 F0, float roughness, float ndotv);

// Ambient lookup (cubemap + lightmap)
float3 ambientLookup(float3 N, float3 worldPos, float2 lightmapUV);
```

#### Task 2: Shader Integration
- [ ] Add to `stdshaders/CMakeLists.txt` (if using CMake)
- [ ] Add to `stdshaders/buildshaders.bat` for Windows
- [ ] Add to shader compile scripts for Linux
- [ ] Register shader in `materialsystem/IShaderSystem.h`

#### Task 3: Material System Updates
- [ ] Update VMT parser to recognize `$mraotexture` parameter
- [ ] Add MRAO texture sampler binding
- [ ] Add `$emissiontexture` support
- [ ] Add `$speculartexture` (F0 override) support
- [ ] Update material proxy system for PBR parameters

**Files to Modify:**
- `src/materialsystem/imaterial.h`
- `src/materialsystem/imaterialinternal.h`
- `src/materialsystem/shadersystem.cpp`
- `src/materialsystem/cmaterial.cpp`

#### Task 4: Cubemap Requirements
- [ ] Ensure cubemap resolution ≥128x128
- [ ] Add `-forceallmips` launch parameter for rough materials
- [ ] Update env_cubemap entity for PBR (if needed)

**Testing:**
- [ ] Create test materials with known MRAO values
- [ ] Verify metalness scale (0.0 = plastic, 1.0 = chrome)
- [ ] Verify roughness scale (0.0 = mirror, 1.0 = matte)
- [ ] Verify AO darkening in crevices

---

### Phase 3B: Authoring Tools (Week 4)

#### Task 5: Substance Painter Export Preset
- [ ] Create export template for Source 1.5
- [ ] Map Substance channels to Source textures:
  - Base Color → $basetexture (RGB)
  - Normal → $bumpmap (RGB, OpenGL/DirectX format)
  - Metallic → MRAO R channel
  - Roughness → MRAO G channel
  - Ambient Occlusion → MRAO B channel
  - Emissive → $emissiontexture (RGB)
- [ ] Document texture resolution guidelines:
  - Props: 2048x2048 (hero), 1024x1024 (standard), 512x512 (background)
  - Brushes: 512x512 standard, tiled
- [ ] Add VMT template generator script

**Export Preset Location:**
`tools/substance_painter/source_1.5_export.spexp`

#### Task 6: VTF Conversion Pipeline
- [ ] Document VTFCmd usage for MRAO textures
- [ ] Create batch conversion scripts
- [ ] Add DXT5 compression guidelines
- [ ] Add mipmap generation settings

**Example Conversion:**
```bash
# Albedo (sRGB)
vtfcmd -file albedo.tga -format DXT1 -flag SRGB

# Normal (linear)
vtfcmd -file normal.tga -format DXT5 -flag NORMAL

# MRAO (linear)
vtfcmd -file mrao.tga -format DXT5

# Emission (sRGB)
vtfcmd -file emission.tga -format DXT1 -flag SRGB
```

---

### Phase 3C: Content Library (Week 4-5)

#### Task 7: CC0 Material Library

Create 5 starter materials from CC0 sources (ambientCG.com, polyhaven.com):

1. **Metal** (Brushed Aluminum)
   - Metalness: 1.0, Roughness: 0.4
   - Use case: Props, machinery

2. **Wood** (Oak Planks)
   - Metalness: 0.0, Roughness: 0.6
   - Use case: Furniture, floors

3. **Concrete** (Rough Concrete)
   - Metalness: 0.0, Roughness: 0.9
   - Use case: Brushwork, buildings

4. **Plastic** (Matte Plastic)
   - Metalness: 0.0, Roughness: 0.5
   - Use case: Props, trim

5. **Fabric** (Canvas)
   - Metalness: 0.0, Roughness: 0.8
   - Use case: Cloth, tarps

**Library Location:**
`content/materials/source15_pbr_library/`

**Structure:**
```
source15_pbr_library/
├── metal_brushed_aluminum/
│   ├── albedo.vtf
│   ├── normal.vtf
│   ├── mrao.vtf
│   └── material.vmt
├── wood_oak_planks/
│   └── ...
├── concrete_rough/
│   └── ...
├── plastic_matte/
│   └── ...
└── fabric_canvas/
    └── ...
```

---

### Phase 3D: Documentation (Week 5)

#### Task 8: Hello PBR Prop Tutorial

**Written Guide:** `docs/HELLO_PBR_PROP.md`

Steps:
1. Export textures from Substance Painter using Source 1.5 preset
2. Convert textures to VTF using VTFCmd
3. Create VMT file with PBR parameters
4. Compile prop in Hammer
5. Place prop in test map
6. Build cubemaps
7. Test under different lighting conditions

**Video Tutorial (2 minutes):**
- Silent screencapture
- Demonstrates: Export → Convert → VMT → Compile → Test
- Shows before/after (Phong vs PBR)

#### Task 9: Technical Documentation

**Files to Create:**
- `docs/PBR_SHADER_GUIDE.md` - Technical shader documentation
- `docs/PBR_AUTHORING_WORKFLOW.md` - Artist guide
- `docs/PBR_MATERIAL_REFERENCE.md` - VMT parameter reference

**Content:**
- BRDF theory and implementation
- Material property guidelines (what values to use)
- Common pitfalls and solutions
- Performance considerations
- Backward compatibility notes

---

## Testing Strategy

### Unit Tests
- [ ] BRDF function tests (Fresnel, GGX, Smith)
- [ ] MRAO texture unpacking
- [ ] Parameter validation

### Visual Tests
- [ ] Material validation scene (known reference materials)
- [ ] Side-by-side Phong vs PBR comparison
- [ ] Cubemap quality at various resolutions
- [ ] Metalness scale verification (0.0 to 1.0)
- [ ] Roughness scale verification (0.0 to 1.0)

### Performance Tests
- [ ] Draw call overhead vs Phong
- [ ] Texture memory usage (MRAO vs separate)
- [ ] Shader instruction count
- [ ] Frame time impact on RTX 3060 @1080p (target: <0.5ms)

---

## Deliverables

### Code
- [x] PBR shader files (.fxc)
- [ ] Material system updates (VMT parser)
- [ ] Shader integration (buildshaders, CMake)
- [ ] ConVars for PBR controls

### Content
- [ ] 5 CC0 PBR materials (ready-to-use)
- [ ] Substance Painter export preset
- [ ] VMT templates

### Documentation
- [ ] PBR shader guide (technical)
- [ ] Authoring workflow (artist)
- [ ] Hello PBR Prop tutorial (written + video)
- [ ] Material reference

### Tools
- [ ] Texture conversion scripts
- [ ] VMT generator
- [ ] Validation tools

---

## Integration Roadmap

### Week 3: Core Implementation
- Days 6-7: Shader files and BRDF functions
- Days 8-9: Material system integration
- Day 10: Testing and debugging

### Week 4: Authoring Pipeline
- Days 11-12: Substance Painter preset
- Days 13-14: Content library creation
- Day 15: Workflow documentation

### Week 5: Documentation and Polish
- Days 16-17: Technical documentation
- Days 18-19: Tutorial creation (written + video)
- Day 20: Final testing and validation

---

## Dependencies

### External Libraries
- Thexa4's source-pbr (reference implementation)
- Substance Painter (authoring tool)
- VTFCmd (texture conversion)

### Source SDK Components
- Shader compiler (DX9 fxc)
- Material system (VMT parser)
- Cubemap system (env_cubemap)

### Content Sources
- ambientCG.com (CC0 textures)
- polyhaven.com (CC0 HDRIs and textures)

---

## Performance Targets

- **RTX 3060 @1080p:** PBR materials <0.5ms per frame vs Phong
- **GTX 1660 @1080p:** PBR materials <1.0ms per frame vs Phong
- **Texture Memory:** MRAO = 33% savings vs separate textures
- **Shader Instructions:** <200 vs Phong's ~150 (acceptable overhead)

---

## Risk Mitigation

### Backward Compatibility
- PBR shader coexists with Phong (no replacement)
- Existing materials unaffected
- Gradual migration path for content creators

### Performance Impact
- Optional quality levels (Low/Medium/High)
- Disable via ConVar if needed
- GPU fallback to simpler lighting model

### Adoption Barriers
- Provide content library for quick start
- Comprehensive documentation and tutorials
- Community support via GitHub discussions

---

## References

- **Thexa4 source-pbr:** https://github.com/thexa4/source-pbr
- **Empires Wiki PBR:** https://wiki.empiresmod.com/PBR
- **Valve Dev Wiki:** https://developer.valvesoftware.com/wiki/Adding_PBR_to_Your_Mod
- **ambientCG:** https://ambientcg.com/
- **Poly Haven:** https://polyhaven.com/

---

## Next Steps

1. **Review Thexa4's implementation** in detail (fork and study)
2. **Set up PBR branch** in Source 1.5 repository
3. **Create shader stub files** (pbr_ps20b.fxc, pbr_vs20b.fxc)
4. **Implement BRDF functions** with unit tests
5. **Integrate with material system** (VMT parser)
6. **Test with simple props** (cube, sphere)
7. **Create authoring preset** for Substance Painter
8. **Build content library** (5 CC0 materials)
9. **Write documentation** and tutorials
10. **Community testing** and feedback

---

*Last Updated: 2025-11-03 (Day 5)*
*Status: Research complete, implementation planned for weeks 3-5*
