# PBR Materials Guide

Complete guide to creating and using Physically-Based Rendering (PBR) materials in Source 1.5.

---

## Overview

Source 1.5 implements a Cook-Torrance microfacet BRDF model for physically accurate material rendering. This guide covers the complete workflow from texture creation to in-engine integration.

**Status:** Phase 3 (PBR Materials) - 55% Complete

**Prerequisites:**
- PBR shaders compiled (pbr_vs20.vcs, pbr_ps20b.vcs)
- Understanding of PBR workflow (albedo, metalness, roughness, normal, AO)
- Texture authoring tool (Substance Painter recommended)

---

## PBR Theory (Brief)

### What is PBR?

Physically-Based Rendering uses real-world material properties to simulate light interaction:

**Energy Conservation:** Reflected light never exceeds incident light
**Fresnel Effect:** Surfaces more reflective at grazing angles
**Microsurface Detail:** Small-scale surface imperfections affect reflection

### Material Properties

1. **Albedo (Base Color):** Diffuse color without lighting
2. **Metalness:** 0 = dielectric (non-metal), 1 = metal
3. **Roughness:** 0 = smooth/glossy, 1 = rough/matte
4. **Normal Map:** Surface detail (tangent-space)
5. **Ambient Occlusion:** Cavity shadowing

### MRAO Texture Format

Source 1.5 uses MRAO (Metalness-Roughness-AO) packed texture:
- **R channel:** Metalness (0-1)
- **G channel:** Roughness (0-1)
- **B channel:** Ambient Occlusion (0-1)
- **A channel:** Unused (can be used for emission mask)

**Why MRAO?**
- Efficient: 3 textures in 1
- Standard format used by Substance Painter
- Reduces texture memory and draw calls

---

## VMT (Valve Material Type) Format

### Basic PBR Material

```
"PBR"
{
    // Required textures
    "$basetexture" "materials/models/props/metal_01_albedo"
    "$bumpmap" "materials/models/props/metal_01_normal"
    "$mraotexture" "materials/models/props/metal_01_mrao"

    // Optional parameters
    "$envmap" "env_cubemap"              // Environment map (use map's cubemaps)
    "$emissiontexture" ""                // Emission texture (optional)
    "$envmapcontrast" "1.0"              // IBL intensity (default 1.0)

    // Surface properties
    "$surfaceprop" "metal"               // Surface type for physics/sound

    // Rendering flags
    "$translucent" "0"                   // Opaque by default
    "$alphatest" "0"                     // No alpha testing
    "$nocull" "0"                        // Backface culling enabled
}
```

### Metal Material Example

```
"PBR"
{
    "$basetexture" "materials/models/metal/steel_plate_albedo"
    "$bumpmap" "materials/models/metal/steel_plate_normal"
    "$mraotexture" "materials/models/metal/steel_plate_mrao"
    "$envmap" "env_cubemap"
    "$surfaceprop" "metal"

    // Metalness should be 1.0 in R channel of MRAO
    // Roughness controls glossiness in G channel
    // AO in B channel adds depth to grooves
}
```

### Wood Material Example

```
"PBR"
{
    "$basetexture" "materials/models/wood/oak_planks_albedo"
    "$bumpmap" "materials/models/wood/oak_planks_normal"
    "$mraotexture" "materials/models/wood/oak_planks_mrao"
    "$envmap" "env_cubemap"
    "$surfaceprop" "wood"

    // Metalness should be 0.0 (non-metal) in R channel
    // Roughness typically 0.6-0.8 for wood
    // AO emphasizes grain and knots
}
```

### Plastic Material Example

```
"PBR"
{
    "$basetexture" "materials/models/plastic/abs_black_albedo"
    "$bumpmap" "materials/models/plastic/abs_black_normal"
    "$mraotexture" "materials/models/plastic/abs_black_mrao"
    "$envmap" "env_cubemap"
    "$surfaceprop" "plastic"

    // Metalness: 0.0 (dielectric)
    // Roughness: 0.2-0.4 for glossy plastic
    // AO: Subtle, mostly in corners
}
```

---

## Texture Creation Workflow

### Option 1: Substance Painter (Recommended)

**Advantages:**
- Industry-standard PBR tool
- Real-time preview
- Extensive material library
- Smart materials and generators
- Direct MRAO export

**Workflow:**
1. Import base mesh (.fbx, .obj)
2. Apply materials and textures
3. Bake ambient occlusion
4. Export textures using Source 1.5 preset

**Export Settings:**
- Albedo: RGB, sRGB, 2048x2048
- Normal: RGB, Linear, DirectX format
- MRAO: RGB (Metalness R, Roughness G, AO B), Linear, 2048x2048

### Option 2: Photoshop/GIMP Manual

**Workflow:**
1. Create albedo map (base color, no lighting)
2. Create normal map (from height map or sculpt)
3. Create MRAO composite:
   - Red channel: Metalness (black = non-metal, white = metal)
   - Green channel: Roughness (black = smooth, white = rough)
   - Blue channel: AO (black = occluded, white = no occlusion)

**Tips:**
- Albedo should be flat-lit (no shadows or highlights)
- Avoid pure white or pure black in albedo (physically impossible)
- Metals should have colored albedo (e.g., gold = RGB(255, 215, 0))
- Non-metals should have low albedo values (30-240 RGB range)

### Option 3: AI Generation (Experimental)

**Tools:**
- Poly.pizza (CC0 materials)
- Materialize (texture generation)
- Stable Diffusion + ControlNet

**Caution:** Always verify licensing and attribution requirements

---

## CC0 Material Sources

### Recommended Sources (Public Domain)

1. **ambientCG** (https://ambientcg.com/)
   - 1000+ high-quality PBR materials
   - CC0 license (public domain)
   - Resolution up to 8K
   - Pre-made MRAO maps

2. **Poly Haven** (https://polyhaven.com/textures)
   - 200+ PBR texture sets
   - CC0 license
   - Includes HDRIs for testing
   - High resolution (2K-8K)

3. **3D Textures** (https://3dtextures.me/)
   - 500+ seamless textures
   - CC0 license
   - Good variety of materials
   - 2K resolution standard

4. **Texture Ninja** (https://texture.ninja/)
   - CC0 PBR textures
   - Seamless tileable
   - 4K resolution
   - Material categories

### Converting Downloaded Materials

Most CC0 sources provide separate maps:
- Albedo/Diffuse/Color
- Normal
- Metallic/Metalness
- Roughness
- AO/Ambient Occlusion

**Combine into MRAO:**
1. Open Photoshop/GIMP
2. Create new RGB image (same resolution as source)
3. Paste Metallic into Red channel
4. Paste Roughness into Green channel
5. Paste AO into Blue channel
6. Save as PNG or TGA (no compression)

---

## Material Categories

### 1. Metals

**Characteristics:**
- Metalness = 1.0 (full metal)
- Colored albedo (not gray)
- High reflectivity (Fresnel)
- Roughness varies by finish

**Examples:**
- Gold: Albedo RGB(255, 215, 0), Roughness 0.2
- Steel: Albedo RGB(180, 180, 190), Roughness 0.3
- Copper: Albedo RGB(200, 120, 80), Roughness 0.4
- Aluminum: Albedo RGB(220, 220, 230), Roughness 0.25

### 2. Dielectrics (Non-Metals)

**Characteristics:**
- Metalness = 0.0
- Lower albedo values
- Specular reflectance ~4% (F0 = 0.04)
- Roughness varies widely

**Examples:**
- Wood: Albedo RGB(120, 80, 50), Roughness 0.7
- Plastic: Albedo RGB(100, 100, 100), Roughness 0.3
- Concrete: Albedo RGB(140, 140, 130), Roughness 0.85
- Rubber: Albedo RGB(40, 40, 40), Roughness 0.95

### 3. Hybrid Materials

**Characteristics:**
- Metalness 0.0-1.0 (use with caution)
- For painted metal, use metalness mask
- Generally avoid in-between values

**Example (Painted Metal):**
- Base: Metalness 1.0 (metal)
- Paint areas: Metalness 0.0 (dielectric)
- Use texture mask to blend

---

## Substance Painter Export Preset

### Export Configuration (JSON)

Save as `source15_pbr.spexp` in Substance Painter export presets folder:

```json
{
  "name": "Source 1.5 PBR",
  "maps": [
    {
      "fileName": "$mesh_$textureSet_albedo",
      "channels": [
        {
          "destChannel": "R",
          "srcChannel": "BaseColor_R",
          "srcMapType": "documentMap",
          "srcMapName": "BaseColor"
        },
        {
          "destChannel": "G",
          "srcChannel": "BaseColor_G",
          "srcMapType": "documentMap",
          "srcMapName": "BaseColor"
        },
        {
          "destChannel": "B",
          "srcChannel": "BaseColor_B",
          "srcMapType": "documentMap",
          "srcMapName": "BaseColor"
        }
      ],
      "parameters": {
        "fileFormat": "png",
        "bitDepth": "8",
        "sizeLog2": 11,
        "dithering": true,
        "paddingAlgorithm": "infinite",
        "dilationDistance": 16,
        "colorspace": "sRGB"
      }
    },
    {
      "fileName": "$mesh_$textureSet_normal",
      "channels": [
        {
          "destChannel": "R",
          "srcChannel": "Normal_R",
          "srcMapType": "documentMap",
          "srcMapName": "Normal_DirectX"
        },
        {
          "destChannel": "G",
          "srcChannel": "Normal_G",
          "srcMapType": "documentMap",
          "srcMapName": "Normal_DirectX"
        },
        {
          "destChannel": "B",
          "srcChannel": "Normal_B",
          "srcMapType": "documentMap",
          "srcMapName": "Normal_DirectX"
        }
      ],
      "parameters": {
        "fileFormat": "png",
        "bitDepth": "8",
        "sizeLog2": 11,
        "dithering": false,
        "paddingAlgorithm": "infinite",
        "dilationDistance": 16,
        "colorspace": "linear"
      }
    },
    {
      "fileName": "$mesh_$textureSet_mrao",
      "channels": [
        {
          "destChannel": "R",
          "srcChannel": "Metallic_R",
          "srcMapType": "documentMap",
          "srcMapName": "Metallic"
        },
        {
          "destChannel": "G",
          "srcChannel": "Roughness_R",
          "srcMapType": "documentMap",
          "srcMapName": "Roughness"
        },
        {
          "destChannel": "B",
          "srcChannel": "AmbientOcclusion_R",
          "srcMapType": "documentMap",
          "srcMapName": "AmbientOcclusion"
        }
      ],
      "parameters": {
        "fileFormat": "png",
        "bitDepth": "8",
        "sizeLog2": 11,
        "dithering": false,
        "paddingAlgorithm": "infinite",
        "dilationDistance": 16,
        "colorspace": "linear"
      }
    }
  ]
}
```

**Installation:**
1. Copy `source15_pbr.spexp` to:
   - Windows: `Documents/Adobe/Adobe Substance 3D Painter/assets/export-presets/`
   - macOS: `~/Documents/Adobe/Adobe Substance 3D Painter/assets/export-presets/`
2. Restart Substance Painter
3. Select "Source 1.5 PBR" in export dialog

---

## Integration Checklist

### Per-Material Checklist

- [ ] Albedo texture created (no lighting, sRGB)
- [ ] Normal map created (DirectX format, tangent-space)
- [ ] MRAO texture created (Metalness R, Roughness G, AO B)
- [ ] Textures converted to VTF (Valve Texture Format)
- [ ] VMT file created with correct paths
- [ ] `$envmap "env_cubemap"` set for reflections
- [ ] `$surfaceprop` matches material type
- [ ] Tested in-game on test prop

### Texture Conversion (VTF)

**Using VTFCmd (command-line):**
```bat
vtfcmd -file albedo.png -format DXT1
vtfcmd -file normal.png -format DXT5 -nolod
vtfcmd -file mrao.png -format DXT5
```

**Using VTFEdit (GUI):**
1. Open texture in VTFEdit
2. Tools → Convert Folder
3. Format: DXT1 (albedo), DXT5 (normal/MRAO)
4. Generate mipmaps: Yes (except normal)
5. Save to `game/materials/` directory

---

## Common Issues

### 1. Materials Too Dark

**Cause:** Albedo values too low, or AO too strong

**Fix:**
- Raise albedo brightness (30-240 range)
- Reduce AO intensity (0.7-1.0 range)
- Check environment map is present

### 2. Materials Too Shiny

**Cause:** Roughness too low

**Fix:**
- Increase roughness value in MRAO green channel
- Most real-world materials have roughness > 0.2

### 3. No Reflections

**Cause:** Missing environment map

**Fix:**
- Add `"$envmap" "env_cubemap"` to VMT
- Ensure map has cubemaps built (`buildcubemaps` in console)

### 4. Incorrect Normal Mapping

**Cause:** Wrong normal map format (OpenGL vs DirectX)

**Fix:**
- Source uses DirectX format (green channel down = inward)
- Flip green channel if using OpenGL normals

### 5. Black Pixels in MRAO

**Cause:** Pure black in any channel

**Fix:**
- Metalness: 0.0 for non-metals (not black)
- Roughness: Minimum 0.04 (avoid pure black)
- AO: Use 0.1-1.0 range (never 0.0)

---

## Performance Considerations

### Texture Resolution

| Prop Size | Recommended Resolution | Notes |
|-----------|------------------------|-------|
| Small (< 1m) | 512x512 | Bullets, small props |
| Medium (1-3m) | 1024x1024 | Weapons, furniture |
| Large (3-10m) | 2048x2048 | Vehicles, large props |
| Huge (> 10m) | 4096x4096 | Buildings, terrain |

### Texture Compression

**Formats:**
- **DXT1:** Albedo (no alpha), 4:1 compression
- **DXT5:** Normal, MRAO (with alpha), 4:1 compression
- **RGBA8888:** Uncompressed (avoid for runtime)

**Memory Usage (1024x1024):**
- DXT1: 0.5 MB
- DXT5: 1.0 MB
- RGBA8888: 4.0 MB

**Tip:** Use DXT1 for albedo, DXT5 for everything else

---

## Next Steps

1. **Create Test Materials:** Start with 5 basic materials
2. **Test in Engine:** Apply to simple cube prop
3. **Iterate:** Adjust roughness/metalness based on results
4. **Expand Library:** Add more material types as needed

---

## References

- **PBR Theory:** https://learnopengl.com/PBR/Theory
- **Material Authoring:** https://marmoset.co/posts/basic-theory-of-physically-based-rendering/
- **Substance Painter:** https://substance3d.adobe.com/documentation/spdoc/
- **ambientCG:** https://ambientcg.com/
- **Poly Haven:** https://polyhaven.com/

---

*Last Updated: 2025-11-04*
*Source 1.5 - Phase 3 PBR Materials Guide*
