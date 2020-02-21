#ifndef FILESYSTEMFIXED_H
#define FILESYSTEMFIXED_H

#ifdef VP_USE_EXPERIMENTAL_FILESYSTEM
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;
#else
#include <filesystem>

namespace fs = std::filesystem;
#endif

#endif // FILESYSTEMFIXED_H
