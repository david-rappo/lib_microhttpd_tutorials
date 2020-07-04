// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "file_utility.h"

// C++ Standard Library
#include <cstring>
#include <stdexcept>

// System
#include <sys/stat.h>
#include <sys/types.h>

#include <unistd.h>

namespace File_utility
{

void close_file_descriptor(int *file_descriptor)
{
    if (nullptr != file_descriptor)
    {
        close(*file_descriptor);
    }
}

off_t get_file_size(int file_descriptor)
{
    struct stat statistics;
    memset(&statistics, 0, sizeof(struct stat));
    if (-1 == fstat(file_descriptor, &statistics))
    {
        return 0;
    }
    
    return statistics.st_size;
}

} // File_utility