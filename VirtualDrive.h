#pragma once

#include <string>
#include <fuse.h>

class VirtualDrive
{
public:
    VirtualDrive();
    ~VirtualDrive();

    bool mount(const std::string &sourceFolder, const std::string &mountPoint);
    bool unmount(); // optional, fuse runs in foreground and exits on Ctrl+C

private:
    std::string m_sourceFolder;
    std::string m_mountPoint;

    static struct fuse_operations fuseOps;

    // FUSE Callbacks
    static int getattrCallback(const char *path, struct fuse_stat *stbuf);
    static int readdirCallback(const char *path, void *buf, fuse_fill_dir_t filler,
                               fuse_off_t offset, struct fuse_file_info *fi);
};
