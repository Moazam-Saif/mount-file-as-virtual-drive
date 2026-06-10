# VirtualDriveMount

A Windows CLI tool for creating, mounting, and unmounting Virtual Hard Disk (VHD) files using `diskpart`, with a partially implemented FUSE-based virtual filesystem layer via WinFsp.

---

## Features

- **Create VHD** — generates a new expandable VHD file, attaches it, creates a primary partition, auto-assigns a free drive letter, formats it as NTFS, then detaches it cleanly
- **Mount VHD** — attaches an existing `.vhd` file so it appears as a drive in Windows Explorer
- **Unmount VHD** — detaches a mounted VHD
- **Auto drive letter selection** — scans from `Z:` downward, skipping `C:`, `D:`, and `E:`
- **WinFsp/FUSE scaffold** — `VirtualDrive` class wraps FUSE operations (currently exposes a stub filesystem with a single dummy file)

---

## Requirements

- Windows 10/11 (64-bit)
- [WinFsp](https://github.com/winfsp/winfsp/releases) installed at `C:\Program Files (x86)\WinFsp`
- [MSYS2](https://www.msys2.org/) with the UCRT64 toolchain (`g++`, `mingw32-make`)
- [CMake](https://cmake.org/) ≥ 3.16
- Administrator privileges (required by `diskpart`)

---

## Build

```bash
# From the project root
mkdir build && cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug \
         -DCMAKE_C_COMPILER=E:/msys64/ucrt64/bin/gcc.exe \
         -DCMAKE_CXX_COMPILER=E:/msys64/ucrt64/bin/g++.exe
mingw32-make
```

The executable `VirtualDriveMount.exe` and `winfsp-x64.dll` will appear in the `build/` directory.

---

## Usage

Run as **Administrator** (diskpart requires elevation):

```
Virtual Disk CLI
1. Create VHD
2. Mount VHD
3. Unmount VHD
4. Exit
Enter choice:
```

- When creating, provide a filename like `myDisk.vhd`. A relative path is resolved to a full path automatically.
- VHD size is fixed at **100 MB**, expandable type.
- When mounting/unmounting, provide the same `.vhd` filename used at creation.

---

## Project Structure

```
.
├── main.cpp          # CLI entry point — menu, VHD create/mount/unmount logic
├── VirtualDrive.h    # WinFsp/FUSE wrapper class declaration
├── VirtualDrive.cpp  # FUSE callbacks (getattr, readdir) — stub implementation
└── CMakeLists.txt    # Build configuration
```

---

## Known Limitations / TODOs

- **FUSE integration is a stub** — `VirtualDrive::mount()` calls `fuse_main()` but the filesystem only returns a single hardcoded `dummy.txt` entry and does not proxy the real source folder yet
- **VHD size is hardcoded** at 100 MB; no user input for size
- **No error recovery** if diskpart fails partway through creation
- `unmount()` on `VirtualDrive` is not implemented (FUSE exits via Ctrl+C)
- Temporary diskpart script files (`create_vhd.txt`, etc.) are written to the working directory and could collide if multiple instances run simultaneously

---

## License

Not specified.
