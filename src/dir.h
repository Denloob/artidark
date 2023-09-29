#pragma once

#include <dirent.h>
#ifdef _WIN32
#define DIR_PATH_SEP '\\'
#else
#define DIR_PATH_SEP '/'
#endif

/**
 * @brief Get the path to an entry in a directory.
 *
 * @param entry The entry
 * @param parent_dir_path The path to the parent directory of the entry
 * @return Full path to the entry. Managed by the caller.
 */
char *dir_get_path_to_entry(const struct dirent *entry,
                            const char *parent_dir_path);
