#include "dir.h"
#include "utils.h"
#include <stdbool.h>
#include <string.h>

char *dir_get_path_to_entry(const struct dirent *entry,
                            const char *parent_dir_path)
{
    size_t parent_dir_path_len = strlen(parent_dir_path);

    bool need_path_sep =
        parent_dir_path[parent_dir_path_len - 1] != DIR_PATH_SEP;

    char *entry_path = xmalloc(parent_dir_path_len + strlen(entry->d_name) +
                               need_path_sep + 1);

    strcpy(entry_path, parent_dir_path);
    if (need_path_sep)
        strcat(entry_path, (char[]){DIR_PATH_SEP, 0});
    strcat(entry_path, entry->d_name);

    return entry_path;
}
