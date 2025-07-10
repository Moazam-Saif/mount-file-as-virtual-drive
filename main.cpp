#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

void printMenu() {
    std::cout << "\nVirtual Disk CLI\n";
    std::cout << "1. Create VHD\n";
    std::cout << "2. Mount VHD\n";
    std::cout << "3. Unmount VHD\n";
    std::cout << "4. Exit\n";
    std::cout << "Enter choice: ";
}

bool fileExists(const std::string& filename) {
    DWORD attrib = GetFileAttributesA(filename.c_str());
    return (attrib != INVALID_FILE_ATTRIBUTES && !(attrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool createVHD(const std::string& filename) {
    if (fileExists(filename)) {
        std::cout << "File already exists.\n";
        return false;
    }

    char driveLetter = getFreeDriveLetter();
    if (!driveLetter) {
        std::cout << "No free drive letter available.\n";
        return false;
    }

    // Create diskpart script
    std::ofstream script("create_vhd.txt");
    script << "create vdisk file=\"" << filename << "\" maximum=100 type=expandable\n";
    script << "select vdisk file=\"" << filename << "\"\n";
    script << "attach vdisk\n";
    script << "create partition primary\n";
    script << "assign letter=" << driveLetter << "\n";
    script.close();

    // Run diskpart
    system("diskpart /s create_vhd.txt");

    // Wait for the drive to appear
    std::string driveRoot = std::string(1, driveLetter) + ":\\";
    bool driveReady = false;
    for (int i = 0; i < 10; ++i) {
        DWORD attrib = GetFileAttributesA(driveRoot.c_str());
        if (attrib != INVALID_FILE_ATTRIBUTES) {
            driveReady = true;
            break;
        }
        Sleep(500);
    }
    if (!driveReady) {
        std::cout << "Drive " << driveLetter << ": was not assigned or is not ready.\n";
        return false;
    }

    // Format as NTFS
    std::cout << "Formatting VHD as NTFS...\n";
    std::string cmd = "format ";
    cmd += driveLetter;
    cmd += ": /FS:NTFS /Q /Y";
    system(cmd.c_str());

    // Detach VHD
    std::ofstream detach("detach_vhd.txt");
    detach << "select vdisk file=\"" << filename << "\"\n";
    detach << "detach vdisk\n";
    detach.close();
    system("diskpart /s detach_vhd.txt");

    // Clean up
    remove("create_vhd.txt");
    remove("detach_vhd.txt");

    std::cout << "VHD created, formatted, and detached.\n";
    return true;
}

bool mountVHD(const std::string& filename) {
    if (!fileExists(filename) || filename.substr(filename.size()-4) != ".vhd") {
        std::cout << "File does not exist or is not a .vhd file.\n";
        return false;
    }
    std::ofstream script("mount_vhd.txt");
    script << "select vdisk file=\"" << filename << "\"\n";
    script << "attach vdisk\n";
    script.close();
    system("diskpart /s mount_vhd.txt");
    remove("mount_vhd.txt");
    std::cout << "VHD mounted.\n";
    return true;
}

bool unmountVHD(const std::string& filename) {
    if (!fileExists(filename) || filename.substr(filename.size()-4) != ".vhd") {
        std::cout << "File does not exist or is not a .vhd file.\n";
        return false;
    }
    std::ofstream script("unmount_vhd.txt");
    script << "select vdisk file=\"" << filename << "\"\n";
    script << "detach vdisk\n";
    script.close();
    system("diskpart /s unmount_vhd.txt");
    remove("unmount_vhd.txt");
    std::cout << "VHD unmounted.\n";
    return true;
}

char getFreeDriveLetter() {
    DWORD drives = GetLogicalDrives();
    std::set<char> skip = {'C', 'D', 'E'};
    for (char letter = 'Z'; letter >= 'A'; --letter) {
        if (skip.count(letter)) continue;
        if (!(drives & (1 << (letter - 'A')))) {
            return letter;
        }
    }
    return 0; // No free letter found
}

int main() {
    std::string filename;
    while (true) {
        printMenu();
        int choice;
        std::cin >> choice;
        if (choice == 1) {
            std::cout << "Enter new VHD filename (with .vhd extension): ";
            std::cin >> filename;
            createVHD(filename);
        } else if (choice == 2) {
            std::cout << "Enter existing VHD filename (with .vhd extension): ";
            std::cin >> filename;
            mountVHD(filename);
        } else if (choice == 3) {
            std::cout << "Enter VHD filename to unmount (with .vhd extension): ";
            std::cin >> filename;
            unmountVHD(filename);
        } else if (choice == 4) {
            break;
        } else {
            std::cout << "Invalid choice.\n";
        }
    }
    return 0;
}