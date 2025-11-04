# Source 1.5 Coding Style Guide

## Philosophy

Follow existing Valve conventions in the codebase. When in doubt, match the style of surrounding code.

## Formatting

Run `clang-format` before committing. The `.clang-format` file enforces Valve's conventions.

## Naming Conventions

### Classes and Interfaces

- **Classes**: `CMyClass` (C prefix + PascalCase)
- **Interfaces**: `IMyInterface` (I prefix + PascalCase)
- **Structs**: `MyStruct_t` (PascalCase + _t suffix for POD types)

```cpp
class CBaseEntity;
interface IShaderAPI;
struct MaterialVideoMode_t;
```

### Variables

- **Members**: `m_MyMember` (m_ prefix + PascalCase)
- **Static members**: `sm_MyStatic` (sm_ prefix + PascalCase)
- **Globals**: `g_MyGlobal` (g_ prefix + PascalCase)
- **Pointers**: `pMyPointer` (p prefix + PascalCase)
- **Locals**: `myLocal` (camelCase, no prefix)

```cpp
class CExample {
	int m_nHealth;              // Member variable
	static bool sm_bInitialized; // Static member
	CBaseEntity *m_pOwner;      // Pointer member
};

CExample *g_pGlobalExample;     // Global pointer
```

### Hungarian Notation (common in tier0/tier1)

Use type prefixes for clarity:
- `n` - int, count, index
- `f` - float
- `b` - bool
- `p` - pointer
- `sz` - null-terminated string
- `h` - handle
- `i` - iterator, index

```cpp
int nCount = 0;
float flDamage = 10.5f;
bool bIsAlive = true;
const char *szName = "test";
```

### Functions and Methods

- **Functions**: `PascalCase` (no prefix)
- **Getters**: `GetValue()` (not `get_value()`)
- **Setters**: `SetValue()` (not `set_value()`)
- **Predicates**: `IsValid()`, `HasWeapon()` (not `is_valid()`)

```cpp
void Spawn();
int GetHealth() const;
void SetHealth(int nHealth);
bool IsAlive() const;
```

### Constants and Macros

- **Enums**: `ENUM_VALUE` (SCREAMING_SNAKE_CASE)
- **Defines**: `MAX_PLAYERS` (SCREAMING_SNAKE_CASE)
- **Const values**: Match variable conventions

```cpp
enum MyEnum_t {
	MYENUM_NONE = 0,
	MYENUM_VALUE,
};

#define MAX_WEAPON_SLOTS 6

const float DAMAGE_MULTIPLIER = 1.5f;
```

## File Organization

### Header Guards

Use both `#ifndef` and `#pragma once`:

```cpp
#ifndef MYHEADER_H
#define MYHEADER_H

#ifdef _WIN32
#pragma once
#endif

// Header contents

#endif // MYHEADER_H
```

### Include Order

1. Corresponding header (for .cpp files)
2. Local project headers
3. Public SDK headers
4. Tier headers (tier0, tier1, mathlib)
5. System headers
6. **LAST**: `tier0/memdbgon.h` (memory debugging)

```cpp
#include "myclass.h"

#include "game/server/baseentity.h"
#include "public/tier1/utlvector.h"
#include "tier0/dbg.h"

#include <ctype.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"
```

## Code Patterns

### Error Handling

Use `Assert()` and `AssertMsg()` from `tier0/dbg.h`:

```cpp
Assert(pEntity != NULL);
AssertMsg(nHealth >= 0, "Health cannot be negative");
```

### Logging

Use `Msg()`, `Warning()`, `DevMsg()`:

```cpp
Msg("Player spawned at (%f, %f, %f)\n", x, y, z);
Warning("Failed to load model: %s\n", szModelName);
DevMsg("Debug info: entity index %d\n", entindex());
```

### Memory Allocation

Never use `new`/`delete` directly. Use tier0 allocators:

```cpp
// Correct
MyClass *pObj = new MyClass();  // OK in Source SDK

// Avoid raw malloc
void *pBuffer = malloc(1024);  // NO - use tier0
```

### Containers

Prefer Source containers over STL:

```cpp
#include "tier1/utlvector.h"
#include "tier1/utlmap.h"

CUtlVector<int> myVector;
CUtlMap<int, CBaseEntity*> myMap;
```

## Comments

### Header Comments

```cpp
//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Brief description of file purpose
//
//=============================================================================
```

### Function Comments

Use Doxygen-style for public APIs:

```cpp
/**
 * Spawns the entity into the world
 * @return true if spawn succeeded
 */
bool Spawn();
```

### Inline Comments

Explain **why**, not **what**:

```cpp
// Good: explains intent
// Disable touch functions during map cleanup to prevent crashes
sm_bDisableTouchFuncs = true;

// Bad: restates the code
// Set the disable touch funcs variable to true
sm_bDisableTouchFuncs = true;
```

## Platform Code

Use tier0 platform abstractions:

```cpp
#ifdef _WIN32
	// Windows-specific
#elif defined(POSIX)
	// Linux/Mac
#endif

// Better: use tier0 defines
#ifdef PLATFORM_WINDOWS
	// Windows
#elif defined(PLATFORM_LINUX)
	// Linux
#endif
```

## Modern C++ Guidelines (Source 1.5 additions)

Source 1.5 allows **C++17** where it improves code without breaking compatibility:

### Allowed

- `auto` for obvious types: `auto *pEntity = GetOwner();`
- Range-based for: `for (auto &item : collection)`
- `nullptr` instead of `NULL`
- `constexpr` for compile-time constants
- `static_assert` for compile-time checks

### Avoid

- Exceptions (disabled in Source SDK)
- RTTI (disabled in Source SDK)
- STL containers (use tier1 containers)
- `std::string` (use `CUtlString`)

## Testing

All new code in `framework/` must have unit tests:

```cpp
// tests/test_myclass.cpp
#include <catch2/catch_test_macros.hpp>
#include "framework/myclass.h"

TEST_CASE("MyClass basic functionality", "[myclass]") {
	MyClass obj;
	REQUIRE(obj.IsValid());
}
```

## References

- Existing Source SDK code (best reference)
- Valve Developer Wiki: https://developer.valvesoftware.com/
- This codebase's `CLAUDE.md` for project-specific rules
