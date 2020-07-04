#pragma once

#include <cstdio>

namespace File_utility
{

void close_file_descriptor(int *file_descriptor);

off_t get_file_size(int file_descriptor);

} // File_utility