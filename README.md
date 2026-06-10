# VirtualDriveMount

A Windows CLI tool for creating, mounting, and unmounting Virtual Hard Disk (VHD) files using `diskpart`, with a partially implemented FUSE-based virtual filesystem layer via WinFsp.

## Essential Files

| File | Role |
|---|---|
| `main.cpp` | CLI entry point — all VHD create/mount/unmount logic via `diskpart` |
| `VirtualDrive.h` | `VirtualDrive` class declaration + FUSE callback signatures |
| `VirtualDrive.cpp` | FUSE `getattr`/`readdir` stubs; `fuse_main()` wrapper |
| `CMakeLists.txt` | Build config — WinFsp paths, linked libraries, DLL copy step |

> `build/` is entirely generated (CMake cache, Makefiles, object files, compiled binary). It is not needed to understand the code and should not be committed.

---

## Architecture

```
main.cpp
  └── VirtualDrive::mount(sourceFolder, mountPoint)
        └── fuse_main() ──► getattrCallback()   // returns stub dir at "/"
                       └──► readdirCallback()   // returns hardcoded "dummy.txt"
```

The `main.cpp` CLI operates independently of the FUSE layer — it shells out to `diskpart` using generated script files for all actual VHD operations. The `VirtualDrive` class is a separate, unused-by-main FUSE scaffold.

---

## Prerequisites

- Windows 10/11 (64-bit)
- [WinFsp](https://github.com/winfsp/winfsp/releases) at `C:\Program Files (x86)\WinFsp`
- MSYS2 UCRT64 toolchain (`g++`, `mingw32-make`)
- CMake ≥ 3.16
- Administrator privileges (required by `diskpart`)

---

## Build

```bash
mkdir build && cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug \
         -DCMAKE_C_COMPILER=E:/msys64/ucrt64/bin/gcc.exe \
         -DCMAKE_CXX_COMPILER=E:/msys64/ucrt64/bin/g++.exe
mingw32-make
```

Output: `build/VirtualDriveMount.exe` and `build/winfsp-x64.dll`.

---

## Usage

Run as **Administrator**:

```
Virtual Disk CLI
1. Create VHD
2. Mount VHD
3. Unmount VHD
4. Exit
```

- **Create**: provide a filename like `myDisk.vhd`. Relative paths are resolved to absolute automatically. VHD is 100 MB expandable, formatted NTFS, then detached.
- **Mount / Unmount**: provide the same `.vhd` filename used at creation.
- Drive letter is auto-selected scanning `Z:` downward, skipping `C:`, `D:`, `E:`.

---

## Known Limitations / TODOs

- `VirtualDrive` FUSE layer is a stub — `readdirCallback` returns only a hardcoded `dummy.txt`; the source folder is never proxied
- VHD size is hardcoded at 100 MB with no user input
- Temporary diskpart scripts (`create_vhd.txt` etc.) are written to the working directory and could collide if multiple instances run simultaneously
- `VirtualDrive::unmount()` is not implemented
