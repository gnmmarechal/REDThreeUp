#pragma once

#include <3ds.h>

#define FS_ATTRIBUTE_NONE 0

Result sdfs_init(char* r);
void sdfs_free();
Result sdfs_read2buf(const char* path, u64* size, char** bufptr);
Result sdfs_scandir(const char* path, char argv[][262], char extt[][16], u32* argc, char* ext);
Result sdfs_write2buf(const char* path, u64* size, char* bufptr);

