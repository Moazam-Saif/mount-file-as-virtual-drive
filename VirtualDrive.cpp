#include "VirtualDrive.h"
#include <iostream>
#include <cstring>
#include <sys/stat.h>
#include <dirent.h>

// Initialize FUSE operations with static callback pointers
struct fuse_operations VirtualDrive::fuseOps = {
    .getattr = VirtualDrive::getattrCallback,
    .readdir = VirtualDrive::readdirCallback
};

VirtualDrive::VirtualDrive() {}

VirtualDrive::~VirtualDrive() {}

bool VirtualDrive::mount(const std::string& sourceFolder, const std::string& mountPoint)
{
    m_sourceFolder = sourceFolder;
    m_mountPoint = mountPoint;

    // FUSE command-line args (you can add more)
    const char* fuseArgs[] = {
        "virtualdrive",
        "-f",                          // run in foreground
        mountPoint.c_str()             // mount point
    };
    int argc = sizeof(fuseArgs) / sizeof(fuseArgs[0]);
    char** argv = const_cast<char**>(fuseArgs);

    std::cout << "Mounting virtual drive at " << mountPoint << std::endl;
    return fuse_main(argc, argv, &fuseOps, nullptr) == 0;
}

bool VirtualDrive::unmount()
{
    // Not typically used with fuse_main() since FUSE runs until exit
    std::cout << "Unmount not implemented in FUSE wrapper." << std::endl;
    return false;
}

// Static Callbacks
int VirtualDrive::getattrCallback(const char* path, struct fuse_stat* stbuf)
{
    memset(stbuf, 0, sizeof(struct fuse_stat));
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        return 0;
    }
    return -ENOENT;
}

int VirtualDrive::readdirCallback(const char* path, void* buf, fuse_fill_dir_t filler,
                                  fuse_off_t offset, struct fuse_file_info* fi)
{
    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", nullptr, 0);
    filler(buf, "..", nullptr, 0);

    // For now just a dummy file
    filler(buf, "dummy.txt", nullptr, 0);

    return 0;
}
