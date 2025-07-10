#include <iostream>
#include <string>
#include <windows.h>
#include "VirtualDrive.h"

// Check if WinFsp is available before including VirtualDrive.h
bool checkWinFspAvailability() {
    HMODULE hModule = LoadLibraryA("winfsp-x64.dll");
    if (hModule) {
        FreeLibrary(hModule);
        return true;
    }
    return false;
}

void printUsage() {
    std::cout << "Usage:\n";
    std::cout << "  mount <source_folder> <mount_point>   - Mount folder as virtual drive\n";
    std::cout << "Example:\n";
    std::cout << "  mount C:\\MyFolder M:\n";
    std::cout << "Note: Unmount by pressing Ctrl+C or using fusermount.exe\n";
}

int main(int argc, char* argv[]) {
    // Check WinFsp availability first
    if (!checkWinFspAvailability()) {
        std::cerr << "Error: WinFsp is not installed or not found in PATH.\n";
        std::cerr << "Please install WinFsp from: https://github.com/winfsp/winfsp/releases\n";
        return 1;
    }

    if (argc < 2) {
        printUsage();
        return 1;
    }

    std::string command = argv[1];

    if (command == "mount" && argc == 4) {
        std::string sourceFolder = argv[2];
        std::string mountPoint = argv[3];

        std::cout << "Mounting " << sourceFolder << " at " << mountPoint << std::endl;

        VirtualDrive drive;
        if (!drive.mount(sourceFolder, mountPoint)) {
            std::cerr << "Failed to mount." << std::endl;
            return 1;
        }

    } else {
        printUsage();
        return 1;
    }

    return 0;
}
