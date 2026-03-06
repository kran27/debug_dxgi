# External Patch System

This system allows defining byte patches in external text files located in the `patches/` directory.

## File Format

Files are read line-by-line. Key-value pairs are separated by a colon (`:`).
Comments start with `#`. Empty lines are ignored.

### Detection Fields
These fields determine if the patches in the file should be applied to the current game.

- **`Game`** (Required): Matches against the PDB path name or Product Name.
- **`Version`** (Optional): Matches the executable file version (e.g., `1.10.155.0`).
- **`Timestamp`** (Optional): Matches the executable PE timestamp (Hex or Decimal).

### Patch Fields
Patches are defined in blocks. A `Patch` key starts a new patch definition.

- **`Patch`**: Name/Description of the patch.
- **`Address`**: Hexadecimal offset from the base address.
- **`Bytes`**: Space-separated hex bytes to write.

## Example

```text
Game: Fallout4Beta
Version: 1.10.155.0

Patch: TCD Crash Fix
Address: 0x2DF17B8
Bytes: E9 97 03 00 00

Patch: Another Fix
Address: 0x123456
# Fixes the hang at startup.
Bytes: EB 90
```

## How It Works
1. When the DLL loads, it scans the `patches/` directory for `.txt` files.
2. For each file, it parses the headers (`Game`, `Version`, `Timestamp`).
3. If the properties match the currently running process:
    - It iterates through all defined patches.
    - Applies the bytes to the specified relative memory addresses.
