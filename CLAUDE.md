# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**Source SDK 2013** - Valve's C++ game engine development kit for creating Source Engine mods. Contains game code for Half-Life 2, HL2: Deathmatch, and Team Fortress 2.

**License:** SOURCE 1 SDK LICENSE - Non-commercial use only. Mods must be distributed free of charge. All contributions grant Valve perpetual, irrevocable rights.

**Languages:** C++ (primary), Python (build scripts), Protocol Buffers (.proto for networking)

**Supported Platforms:** Windows (Visual Studio 2022), Linux (Steam Runtime via Podman)

## Build System

### VPC (Valve Project Configuration)
VPC is Valve's build configuration system that generates platform-specific project files from .vpc scripts.

**Windows Build:**
```bat
cd src
createallprojects.bat
```
Generates `everything.sln` for Visual Studio 2022. Opens solution with all projects (HL2, HL2MP, TF2).

Requirements:
- Visual Studio 2022 with Desktop development with C++
- MSVC v143 toolset
- Windows 11 SDK (10.0.22621.0) or Windows 10 SDK (10.0.19041.0)
- Python 3.13+
- Source SDK 2013 Multiplayer (Steam appid 243750)

**Linux Build:**
```bash
cd src
./buildallprojects [debug|release]
```
Builds using Ninja in Steam Runtime container (Podman). Defaults to release mode. Generates `compile_commands.json` for IDE integration.

**VPC Commands:**
- `/hl2mp` - Build HL2 Deathmatch
- `/tf` - Build Team Fortress 2
- `/define:SOURCESDK` - Add SOURCESDK preprocessor define
- `+everything` - Generate all projects
- `/mksln <name>` - Create solution/makefile with specified name

**Key VPC Files:**
- `src/vpc_scripts/source_base.vpc` - Base configuration for all projects
- `src/vpc_scripts/source_dll_base.vpc` - DLL project template
- `src/vpc_scripts/source_exe_base.vpc` - Executable project template
- `src/vpc_scripts/projects.vgc` - Project group definitions

## Architecture Overview

### Client-Server-Shared Separation

The codebase uses strict **client-server architecture** with three layers:

1. **Server** (`src/game/server/`) - Game logic, physics, AI, entity spawning
   - Runs on game servers only
   - No rendering or UI code
   - Authoritative for all game state
   - Files: `baseentity.cpp`, `ai_basenpc.cpp`, `tf_player.cpp`

2. **Client** (`src/game/client/`) - Rendering, UI, input, prediction
   - Runs on player machines only
   - Receives updates from server
   - Client-side prediction for responsiveness
   - Files: `c_baseentity.cpp`, `c_baseplayer.cpp`, HUD elements
   - Naming convention: Files prefixed with `c_*`

3. **Shared** (`src/game/shared/`) - Code used by both client and server
   - Weapon definitions, game rules, shared entity logic
   - Must be deterministic (same results on client and server)
   - Files: `baseentity_shared.cpp`, `basecombatweapon_shared.cpp`
   - Naming convention: Files suffixed with `*_shared.*`
   - Uses `#ifdef CLIENT_DLL` and `#ifdef GAME_DLL` to differentiate

**Critical Rule:** Never put client-only code (rendering, HUD) in server files or server-only code (AI, physics authority) in client files. Shared code must be deterministic.

### Entity System

**Entity Hierarchy:**
```
IHandleEntity
└── CBaseEntity (shared base)
    ├── CBaseAnimating (animated entities)
    │   └── CBaseCombatCharacter
    │       └── CBasePlayer
    │           └── CTFPlayer (TF2 player)
    └── CBaseObject (TF2 buildings)
        ├── CObjectSentrygun
        ├── CObjectDispenser
        └── CObjectTeleporter
```

**Entity Registration:**
Use `LINK_ENTITY_TO_CLASS(mapname, classname)` to register entities for map spawning:
```cpp
LINK_ENTITY_TO_CLASS(item_healthkit_small, CHealthKitSmall);
```

**Entity Handles:**
- `CBaseHandle` - Low-level handle (index + serial number)
- `EHANDLE` - Type-safe handle wrapper that auto-validates
- Use handles instead of raw pointers - entities can be deleted at any time

**Edicts:**
- Server-side entity data structure
- Limited to `maxEntities` (see `CGlobalVars`)
- Networked entities consume edict slots

### Networking and Replication

**SendTables** (server → client):
Defined in server code using `IMPLEMENT_SERVERCLASS_ST`:
```cpp
IMPLEMENT_SERVERCLASS_ST(CTFPlayer, DT_TFPlayer)
    SendPropInt(SENDINFO(m_iHealth)),
    SendPropVector(SENDINFO(m_vecOrigin)),
END_SEND_TABLE()
```

**RecvTables** (client receives):
Defined in client code using `IMPLEMENT_CLIENTCLASS_DT`:
```cpp
IMPLEMENT_CLIENTCLASS_DT(C_TFPlayer, DT_TFPlayer, CTFPlayer)
    RecvPropInt(RECVINFO(m_iHealth)),
    RecvPropVector(RECVINFO(m_vecOrigin)),
END_RECV_TABLE()
```

**Network Proxies:**
- `SendVarProxyFn` - Transform data before sending (e.g., entity pointer → index)
- `RecvVarProxyFn` - Transform data after receiving
- Use for efficient serialization and data conversion

**Protocol Buffers:**
Modern structured messages for Steam integration (trading, matchmaking):
- `src/game/shared/tf/tf_gcmessages.proto` - TF2 Game Coordinator messages
- `src/game/shared/econ/econ_gcmessages.proto` - Economy system messages
- Located in shared/ because both client and server use them

### Tier System

**Foundation Libraries (use these everywhere):**

- **tier0** - Lowest-level platform abstractions (memory, threading, debugging)
  - `platform.h`, `dbg.h`, `memalloc.h`

- **tier1** - Core utilities
  - `bitbuf.h/cpp` - Network bitstream serialization
  - `KeyValues.h/cpp` - Configuration file format (.txt, .res, .vdf files)
  - `convar.h/cpp` - Console variables and commands
  - `checksum_*.cpp` - CRC, MD5, SHA1 hashing
  - `utlvector.h`, `utlmap.h` - Template containers (use instead of STL)

- **tier2** - Higher-level utilities
  - Use sparingly, check dependencies first

- **mathlib** - Vector math
  - `vector.h`, `mathlib.h` - Essential for all game code
  - Vector, QAngle, matrix operations

- **public** - Engine API headers (172 files)
  - `basehandle.h` - Entity handle system
  - `const.h` - Engine constants (MAX_EDICTS, etc.)
  - `edict.h` - Server entity data structures
  - `dt_send.h`, `dt_recv.h` - Network data table system

### Game-Specific Code Organization

**Team Fortress 2** (~740 files, largest game):
- `src/game/client/tf/` - TF2 client (251 files)
- `src/game/server/tf/` - TF2 server (162 files)
- `src/game/shared/tf/` - TF2 shared (326 files, largest subsystem)

**Key TF2 Systems:**
- **Game Rules:** `tf_gamerules.cpp/h` - Core game mode logic (CTF, CP, Payload, MvM)
- **Player Classes:** `tf_classdata.cpp` - Scout, Soldier, Pyro, Demoman, Heavy, Engineer, Medic, Sniper, Spy
- **Buildings:** `baseobject_shared.cpp`, `tf_obj_sentrygun.cpp`, `tf_obj_dispenser.cpp`, `tf_obj_teleporter.cpp`
- **Weapons:** `tf_weapon_*.cpp` files (30+ weapons)
- **Conditions:** `tf_condition.cpp` - Status effects (burning, jarate, invulnerability, etc.)
- **Achievements:** 16 `achievements_tf_*.cpp` files (~700KB total)
- **MvM (Mann vs Machine):** `player_vs_environment/` subdirectory
- **Economy/Trading:** `econ/` subdirectory with Steam integration

**Half-Life 2:**
- `src/game/client/hl2/` - HL2 client
- `src/game/server/hl2/` - HL2 server
- `src/game/shared/hl2/` - HL2 shared

**HL2 Deathmatch:**
- `src/game/client/hl2mp/` - HL2MP client
- `src/game/server/hl2mp/` - HL2MP server
- `src/game/shared/hl2mp/` - HL2MP shared

### Runtime Structure

**Game Directories:**
- `game/mod_tf/` - TF2 mod runtime
- `game/mod_hl2mp/` - HL2MP mod runtime

**gameinfo.txt:**
Defines asset mounting and search paths:
- `SteamAppId 243750` - Source SDK Base 2013 Multiplayer
- `DependsOnAppID 440` - Team Fortress 2 (for TF2 assets)
- VPK files mounted for materials, sounds, models
- `custom/` directory for user modifications
- `download/` for server-downloaded content

**Launching Mods (Windows):**
Right-click client project in VS → Set as Startup Project → Press F5
Default args: `-dev -w 1920 -h 1080 -windowed`

**Launching Mods (Linux):**
```bash
cd game
./mod_tf  # or ./mod_hl2mp
```

## Development Patterns

### Adding New Entities

1. **Server:** Create class in `src/game/server/[game]/`
   - Inherit from appropriate base (CBaseEntity, CBaseAnimating, etc.)
   - Use `LINK_ENTITY_TO_CLASS(mapname, ClassName)`
   - Implement `Spawn()`, `Precache()`, logic methods
   - Define SendTable for networked properties

2. **Client:** Create matching class in `src/game/client/[game]/`
   - Name with `C_` prefix (e.g., `C_MyEntity`)
   - Define RecvTable matching server's SendTable
   - Implement rendering, effects, client prediction

3. **Shared (if needed):** Create in `src/game/shared/[game]/`
   - Use `#ifdef CLIENT_DLL` / `#ifdef GAME_DLL` for client/server-specific code
   - Define shared enums, constants, weapon definitions

### Adding New Weapons

TF2 weapon example structure:
- Server: `src/game/server/tf/tf_weapon_[name].cpp`
- Client: `src/game/client/tf/c_tf_weapon_[name].cpp`
- Shared: `src/game/shared/tf/tf_weapon_[name].cpp` (base weapon logic)

Inherit from `CTFWeaponBase` (defined in shared). Register with weapon ID in `tf_shareddefs.h`.

### Console Variables (ConVars)

```cpp
ConVar my_cvar("my_cvar", "1.0", FCVAR_REPLICATED, "Description");
```

Flags:
- `FCVAR_REPLICATED` - Synced from server to clients
- `FCVAR_CHEAT` - Only works with sv_cheats 1
- `FCVAR_NOTIFY` - Notifies players when changed
- `FCVAR_ARCHIVE` - Saved to config.cfg

### Console Commands (ConCommands)

```cpp
void MyCommand_f()
{
    Msg("Command executed\n");
}
ConCommand my_command("my_command", MyCommand_f, "Description");
```

### KeyValues Configuration Files

Used for `.txt`, `.res`, `.vdf` files:
```cpp
KeyValues *pKV = new KeyValues("RootKey");
if (pKV->LoadFromFile(filesystem, "path/to/file.txt"))
{
    const char *value = pKV->GetString("keyname", "default");
    int num = pKV->GetInt("number", 0);
}
pKV->deleteThis();
```

### AI System

**Server only** - never in client code.

Base class: `CAI_BaseNPC` (in `src/game/server/ai_basenpc.cpp` - 427KB, largest file)

Key systems:
- **Schedules** - High-level AI behaviors (SCHED_IDLE, SCHED_COMBAT_FACE, etc.)
- **Tasks** - Atomic actions within schedules
- **Conditions** - AI state tracking (see enemy, hear sound, etc.)
- **Activities** - Animation states (ACT_IDLE, ACT_RUN, ACT_RELOAD, etc.)

TF2 uses **NextBot** framework for bot AI (in `NextBot/` subdirectories).

### Map Compilation Tools

Located in `src/utils/`:
- **vbsp** - Compile .vmf to .bsp (geometry, entities)
- **vvis** - Calculate visibility (area portals, hint brushes)
- **vrad** - Compile lighting (radiosity, HDR)

Standard compile chain: vbsp → vvis → vrad

### Working with Game Events

Events notify client/server of game occurrences (player death, round start, etc.).

Register listener:
```cpp
gameeventmanager->AddListener(this, "player_death", false);
```

Fire event (server):
```cpp
IGameEvent *event = gameeventmanager->CreateEvent("player_death");
if (event)
{
    event->SetInt("userid", pPlayer->GetUserID());
    event->SetInt("attacker", pAttacker->GetUserID());
    gameeventmanager->FireEvent(event);
}
```

## Common Issues

### Build Failures

**Windows:**
- Ensure VS 2022 with correct SDK versions installed
- Run `createallprojects.bat` from `src/` directory (not root)
- Check Python 3.13+ is in PATH

**Linux:**
- Podman must be installed and running
- Steam Runtime container downloads automatically on first build
- Build artifacts in `src/_vpc_/ninja/`

### Entity Not Spawning

- Check `LINK_ENTITY_TO_CLASS` macro is present
- Verify `Precache()` loads all required assets (models, sounds)
- Check entity is in .fgd file for Hammer editor
- Ensure server entity has matching client class if networked

### Network Variables Not Syncing

- SendTable property names must match member variables (use `SENDINFO` macro)
- RecvTable must exactly match SendTable (same order, types, names)
- Check `FCVAR_REPLICATED` flag on ConVars
- Verify entity is networked (has client-side class)

### Crashes on Entity Access

- Always use `EHANDLE` instead of raw pointers
- Check handle validity: `if (m_hEntity.Get())`
- Entities can be deleted at any time (round reset, map change)
- Never cache pointers across frames

## Code Style

Follow existing Valve conventions:
- **Indentation:** Tabs (not spaces)
- **Braces:** Allman style (opening brace on new line)
- **Naming:**
  - Classes: `CMyClass`
  - Interfaces: `IMyInterface`
  - Members: `m_MyMember` (prefix with `m_`)
  - Globals: `g_MyGlobal` (prefix with `g_`)
  - Pointers: `pMyPointer` (prefix with `p`)
  - Client classes: `C_MyClass` (prefix with `C_`)
- **Comments:** `//` for single-line, `/* */` for multi-line
- **Header guards:** `#ifndef FILENAME_H` / `#define FILENAME_H` / `#pragma once`

## Resources

- **Valve Developer Wiki:** https://developer.valvesoftware.com/wiki/Source_SDK_2013
- **Distribution Guide:** https://partner.steamgames.com/doc/sdk/uploading/distributing_source_engine
- **License:** See LICENSE file - non-commercial use only, free distribution required
- **Contributing:** See CONTRIBUTING file - all contributions grant Valve perpetual rights
